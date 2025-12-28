#include "AudioProcessor.h"

TrackModel AudioProcessor::getAudioMetaData(QString &filePath) {
    TrackModel t;

    // Strategy 1: Fast fail
    QFileInfo info(filePath);
    if (!info.exists()) return t;

    // Basic info
    t.id  = filePath;
    t.url = QUrl::fromLocalFile(filePath).toString();
    t.lyrics = "";

    // Ensure cache directories exist (Do this ONCE in your class constructor/init, not here)
    if (!QDir(cacheSongs).exists()) QDir().mkpath(cacheSongs);
    if (!QDir(cacheCovers).exists()) QDir().mkpath(cacheCovers);

    // =========================================================
    // Step 0: Magic Number Detection (Minimized I/O)
    // =========================================================

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return t;

    // Read only what is strictly necessary.
    // 16 bytes is enough for all your formats.
    QByteArray header = file.read(16);
    file.close();

    AudioFormat detectedFormat = AudioFormat::Unknown;

    // --- Fast Byte Comparison ---
    if (header.startsWith(KGMA_VPR) || header.startsWith(KGMA_KGM)) {
        detectedFormat = AudioFormat::KGMA;
    }
    else if (header.startsWith(NCM_HEADER)) {
        detectedFormat = AudioFormat::NCM;
    }
    else if (header.startsWith(FLAC_HEADER)) {
        detectedFormat = AudioFormat::FLAC;
    }
    else if (header.startsWith(MP3_ID3) || (header.size() >= 2 && (uint8_t)header[0] == 0xFF && ((uint8_t)header[1] & 0xE0) == 0xE0)) {
        detectedFormat = AudioFormat::MP3;
    }
    else if (header.size() >= 12 && header.startsWith(WAV_RIFF) && header.mid(8, 4) == "WAVE") {
        detectedFormat = AudioFormat::WAV;
    }
    else if (header.size() >= 8 && header.mid(4, 4) == "ftyp") {
        detectedFormat = AudioFormat::M4A;
    }
    else if (header.startsWith("OggS")) {
        detectedFormat = AudioFormat::OGG;
    }
    else {
        // Fallback to suffix only if header fails (rare)
        QString suffix = info.suffix().toLower();
        if (suffix == "mp3") detectedFormat = AudioFormat::MP3;
        else if (suffix == "flac") detectedFormat = AudioFormat::FLAC;
        // ... add others if needed
    }

    // =========================================================
    // Step 1: Handle Decryption
    // =========================================================
    if (detectedFormat == AudioFormat::KGMA || detectedFormat == AudioFormat::NCM) {
        QString decryptedPath;
        std::string stdPath = filePath.toStdString();
        std::string stdCache = cacheSongs.toStdString();

        if (detectedFormat == AudioFormat::KGMA) {
            decryptedPath = QString::fromStdString(KGMA::KGMADecrypt(stdPath, stdCache));
        } else {
            decryptedPath = QString::fromStdString(NCM::NCMDecrypt(stdPath, stdCache));
        }

        if (!decryptedPath.isEmpty() && QFile::exists(decryptedPath)) {
            // Recursion is fine here as it happens once per encrypted file
            return getAudioMetaData(decryptedPath);
        }
        return t; // Decryption failed
    }

    // =========================================================
    // Step 2: Metadata Extraction (TagLib)
    // =========================================================

    // Strategy: Determine Cache Key for Cover Art immediately
    // Using MD5 of the file path is safer than filename (avoids collisions) and faster than hashing content.
    // Or use completeBaseName if you are sure names are unique.
    const QString cacheKey = QCryptographicHash::hash(filePath.toUtf8(), QCryptographicHash::Md5).toHex();
    const QString coverPath = cacheCovers + "/" + cacheKey + ".jpg";

    // Strategy: Check if cover already exists.
    // If we have the cover, we might save time extracting it again?
    // Actually, TagLib reads metadata in one go, extracting the picture block
    // isn't expensive, but *writing* it to disk is.
    bool coverExistsInCache = QFile::exists(coverPath);
    if (coverExistsInCache) {
        t.cover = QUrl::fromLocalFile(coverPath).toString();
    }

#ifdef Q_OS_WIN
    TagLib::FileRef f(reinterpret_cast<const wchar_t*>(filePath.utf16()));
#else
    TagLib::FileRef f(filePath.toStdString().c_str());
#endif

    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();

        t.title = QString::fromStdString(tag->title().to8Bit(true));
        if (t.title.isEmpty()) t.title = info.completeBaseName();

        t.artist = QString::fromStdString(tag->artist().to8Bit(true));
        if (t.artist.isEmpty()) t.artist = QStringLiteral("Unknown Artist");

        t.album = QString::fromStdString(tag->album().to8Bit(true));
        if (t.album.isEmpty()) t.album = QStringLiteral("Unknown Album");

        if (f.audioProperties()) {
            t.duration = f.audioProperties()->lengthInSeconds();
        }

        // =========================================================
        // Step 3: Extract Cover Art (Only if not cached)
        // =========================================================
        if (!coverExistsInCache) {
            TagLib::ByteVector coverData;
            bool hasCover = false;

            // Only enter specific casting logic if we need the cover
            if (detectedFormat == AudioFormat::MP3) {
                if (auto mpegFile = dynamic_cast<TagLib::MPEG::File*>(f.file())) {
                    if (mpegFile->ID3v2Tag()) {
                        auto frameList = mpegFile->ID3v2Tag()->frameList("APIC");
                        if (!frameList.isEmpty()) {
                            auto frame = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frameList.front());
                            coverData = frame->picture();
                            hasCover = true;
                        }
                    }
                }
            }
            else if (detectedFormat == AudioFormat::FLAC) {
                if (auto flacFile = dynamic_cast<TagLib::FLAC::File*>(f.file())) {
                    const auto pictureList = flacFile->pictureList();
                    if (!pictureList.isEmpty()) {
                        coverData = pictureList[0]->data();
                        hasCover = true;
                    }
                }
            }
            else if (detectedFormat == AudioFormat::M4A) {
                if (auto mp4File = dynamic_cast<TagLib::MP4::File*>(f.file())) {
                    TagLib::MP4::ItemMap items = mp4File->tag()->itemMap();
                    if (items.contains("covr")) {
                        TagLib::MP4::CoverArtList covers = items["covr"].toCoverArtList();
                        if (!covers.isEmpty()) {
                            coverData = covers.front().data();
                            hasCover = true;
                        }
                    }
                }
            }
            // Add WAV logic if needed...

            // Write to disk
            if (hasCover && !coverData.isEmpty()) {
                // Direct buffer to QImage
                QImage albumArt;
                // Note: QImage loading is relatively expensive, but unavoidable for conversion
                if (albumArt.loadFromData(reinterpret_cast<const uchar*>(coverData.data()), coverData.size())) {
                    QImageWriter writer(coverPath, "jpg");
                    // Optimization: Set compression quality to 80-90 (default is usually good)
                    // writer.setQuality(90);
                    if (writer.write(albumArt)) {
                        t.cover = QUrl::fromLocalFile(coverPath).toString();
                    } else {
                        t.cover = QStringLiteral("qrc:/assets/defaultAlbum.jpg");
                    }
                } else {
                    t.cover = QStringLiteral("qrc:/assets/defaultAlbum.jpg");
                }
            } else {
                t.cover = QStringLiteral("qrc:/assets/defaultAlbum.jpg");
            }
        }
    } else {
        // TagLib failed
        t.title = info.completeBaseName();
        t.artist = QStringLiteral("Unknown Artist");
        t.album = QStringLiteral("Unknown Album");
        t.cover = QStringLiteral("qrc:/assets/defaultAlbum.jpg");
    }

    return t;
}





