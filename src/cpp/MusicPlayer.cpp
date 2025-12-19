/**
 * @file MusicPlayer.cpp
 *
 * @brief 音乐播放器核心实现,包含播放控制、播放列表管理和本地音乐扫描功能。
 *
 * 该类使用 Qt Multimedia 模块实现音频播放，支持播放控制接口、音乐列表管理和信号等处理。
 * 实现了本地音乐文件扫描功能，并将扫描结果存储到 SQLite 数据库中，专门用于本地音乐播放。
 * 实现了播放模式（顺序、循环、单曲循环、随机）和音量控制等功能。
 * 实现了与 QML 的交互接口，方便在 QML 界面中使用播放器功能。
 * 实现了当前播放曲目信息的访问接口，供界面显示使用。
 * 实现了播放进度和状态变化的信号通知，供界面实时更新使用。
 * 实现了播放列表的增删改查接口，支持动态管理播放列表。
 *
 * @author zhou2004
 * @date 2025-12-18
 */

#include "MusicPlayer.h"
#include <QVariant>
#include <QUrl>
#include <QFileDialog>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

MusicPlayer::MusicPlayer(QObject *parent)
    : QObject(parent)
{
    // 初始化默认 AudioState，里面存储当前播放队列，当前歌曲索引，声音等信息
    m_state.currentSongIndex = 0;
    m_state.volume = 50;       // 0~100
    m_state.quality = "exhigh";

    // 初始化底层播放器
    m_audioOutput = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);

    // 默认音量
    m_audioOutput->setVolume(m_state.volume / 100.0f);

    // 连接内部信号到外部 signal，供 QML 绑定
    connect(m_player, &QMediaPlayer::positionChanged,
            this, &MusicPlayer::handlePositionChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, &MusicPlayer::handlePlaybackStateChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::mediaStatusChanged);
}

MusicPlayer::~MusicPlayer() = default;

// ======================= 播放控制接口 =======================

void MusicPlayer::play()
{
    // 如果当前列表为空，直接返回
    if (m_state.trackList.isEmpty())
        return;

    // 如果当前索引越界，则重置到 0
    if (m_state.currentSongIndex < 0
        || m_state.currentSongIndex >= m_state.trackList.size()) {
        m_state.currentSongIndex = 0;
        emit currentSongIndexChanged(m_state.currentSongIndex);
    }

    // 加载并播放当前曲目
    if (!m_player->hasAudio()) loadCurrentTrack();
    m_player->play();
}

void MusicPlayer::playIndex(int index)
{
    if (index < 0 || index >= m_state.trackList.size())
        return;

    setCurrentSongIndex(index);
    loadCurrentTrack();
    m_player->play();
}

void MusicPlayer::playUrl(const QString &url)
{
    if (url.isEmpty())
        return;

    m_player->setSource(QUrl(url));
    m_audioOutput->setVolume(m_state.volume / 100.0f);
    m_player->play();
}

void MusicPlayer::pause()
{
    m_player->pause();
}

void MusicPlayer::stop()
{
    m_player->stop();
}

void MusicPlayer::previous()
{
    if (m_state.trackList.isEmpty())
        return;

    // 只计算一次上一曲索引，并通过 playIndex 触发播放
    int newIndex = m_state.currentSongIndex - 1;
    if (newIndex < 0) {
        // 列表循环：跳到最后一首
        newIndex = m_state.trackList.size() - 1;
    }
    playIndex(newIndex);
}

void MusicPlayer::next()
{
    if (m_state.trackList.isEmpty())
        return;

    // 只计算一次下一曲索引，并通过 playIndex 触发播放
    int newIndex = (m_state.currentSongIndex + 1) % m_state.trackList.size();
    playIndex(newIndex);
}

void MusicPlayer::seek(qint64 positionMs)
{
    if (!m_player)
        return;
    m_player->setPosition(positionMs);
}

// ======================= 播放列表 / 当前页歌曲管理 =======================

void MusicPlayer::setTrackList(const QVariantList &tracks)
{
    // 清空现有列表
    m_state.trackList.clear();

    // 重新添加新列表
    for (const QVariant &v : tracks) {
        const QVariantMap map = v.toMap();
        TrackModel t;
        t.id        = map.value("id").toString();
        t.title     = map.value("title").toString();
        t.artist    = map.value("artist").toString();
        t.album     = map.value("album").toString();
        t.cover     = map.value("cover").toString();
        t.url       = map.value("url").toString();
        t.duration  = map.value("duration").toInt();
        t.likeStatus = map.value("likeStatus").toInt();
        t.lyrics    = map.value("lyrics").toString();
        m_state.trackList.append(t);
    }

    // 重置索引策略，如果新列表非空则从 0 开始，否则设为 -1
    if (!m_state.trackList.isEmpty()) {
        m_state.currentSongIndex = 0;
        emit currentSongIndexChanged(0);
    } else {
        m_state.currentSongIndex = -1;
        emit currentSongIndexChanged(-1);
    }
}

//@TODO: 目前是通过遍历检查重复添加，效率较低，后续可以改为通过哈希表检查
void MusicPlayer::addTracks(const QVariantList &tracks)
{
    // 检查重复添加，避免重复 id
    for (const QVariant &v : tracks) {
        const QVariantMap map = v.toMap();
        const QString id = map.value("id").toString();

        // 检查是否已存在相同 id
        bool exists = false;
        for (const TrackModel &t : std::as_const(m_state.trackList)) {
            if (t.id == id) {
                exists = true;
                break;
            }
        }

        // 关键：跳过已存在的曲目，避免重复添加
        if (exists)
            continue;

        // 添加新曲目
        TrackModel t;
        t.id        = id;
        t.title     = map.value("title").toString();
        t.artist    = map.value("artist").toString();
        t.album     = map.value("album").toString();
        t.cover     = map.value("cover").toString();
        t.url       = map.value("url").toString();
        t.duration  = map.value("duration").toInt();
        t.likeStatus = map.value("likeStatus").toInt();
        t.lyrics    = map.value("lyrics").toString();
        m_state.trackList.append(t);
    }

    // 如果之前是空列表，则从 0 开始
    if (m_state.currentSongIndex < 0 && !m_state.trackList.isEmpty()) {
        m_state.currentSongIndex = 0;
        emit currentSongIndexChanged(0);
    }
}

//@TODO: 目前是通过遍历查找并删除，效率较低，后续可以改为通过索引删除
void MusicPlayer::deleteTrack(const QString &id)
{
    // 查找并删除指定 id 的曲目，不是通过索引，删除效率较低
    for (int i = 0; i < m_state.trackList.size(); ++i) {
        if (m_state.trackList[i].id == id) {
            m_state.trackList.removeAt(i);

            // 调整 currentSongIndex
            if (m_state.currentSongIndex >= m_state.trackList.size()) {
                m_state.currentSongIndex = m_state.trackList.size() - 1;
            }
            emit currentSongIndexChanged(m_state.currentSongIndex);
            break;
        }
    }
}

void MusicPlayer::setCurrentPageSongs(const QVariantList &songs)
{
    // 清空现有列表
    m_currentPageSongs.clear();
    // 重新添加新列表
    for (const QVariant &v : songs) {
        const QVariantMap map = v.toMap();
        TrackModel t;
        t.id        = map.value("id").toString();
        t.title     = map.value("title").toString();
        t.artist    = map.value("artist").toString();
        t.album     = map.value("album").toString();
        t.cover     = map.value("cover").toString();
        t.url       = map.value("url").toString();
        t.duration  = map.value("duration").toInt();
        t.likeStatus = map.value("likeStatus").toInt();
        t.lyrics    = map.value("lyrics").toString();
        m_currentPageSongs.append(t);
    }
}

QJsonArray MusicPlayer::currentPageSongsJson() const
{
    QJsonArray arr;
    for (const TrackModel &t : m_currentPageSongs) {
        arr.append(trackToJson(t));
    }
    return arr;
}

// ======================= 状态访问接口 =======================

void MusicPlayer::setCurrentSongIndex(int index)
{
    if (m_state.currentSongIndex == index)
        return;

    m_state.currentSongIndex = index;
    emit currentSongIndexChanged(index);
}

void MusicPlayer::setVolume(int volume)
{
    if (volume < 0)
        volume = 0;
    if (volume > 100)
        volume = 100;

    if (m_state.volume == volume)
        return;

    m_state.volume = volume;
    emit volumeChanged(volume);

    if (m_audioOutput)
        m_audioOutput->setVolume(static_cast<float>(volume) / 100.0f);
}

QJsonObject MusicPlayer::currentTrackInfo() const
{
    if (m_state.currentSongIndex < 0
        || m_state.currentSongIndex >= m_state.trackList.size()) {
        return QJsonObject();
    }
    return trackToJson(m_state.trackList[m_state.currentSongIndex]);
}


void MusicPlayer::setPlayMode(int mode)
{
    if (m_playMode == mode)
        return;
    m_playMode = mode;
    emit playModeChanged(mode);
}

// ======================= 本地音乐目录扫描 =======================

void MusicPlayer::openLocalMusicFolder()
{
    const QString startPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    QString dirPath = QFileDialog::getExistingDirectory(nullptr,
                                                        tr("Select Music Folder"),
                                                        startPath);
    if (dirPath.isEmpty())
        return;

    scanLocalDirectory(dirPath);
}

void MusicPlayer::scanLocalDirectory(const QString &path)
{
    m_localTracks.clear();

    // 支持 file:/// 开头的 URL
    QString localPath = path;
    if (localPath.startsWith("file:")) {
        QUrl url(localPath);
        localPath = url.toLocalFile();
    }

    QDir dir(localPath);
    if (!dir.exists()) {
        qWarning() << "MusicPlayer::scanLocalDirectory directory not exist:" << localPath;
        emit localTracksChanged();
        return;
    }

    QDirIterator it(localPath,
                    QStringList() << "*.mp3" << "*.flac" << "*.wav" << "*.m4a" << "*.ogg" << "*.aac",
                    QDir::Files,
                    QDirIterator::Subdirectories);

    // 使用 TrackModel 收集扫描结果，然后批量写入 SQLite
    QVector<TrackModel> scannedTracks;

    while (it.hasNext()) {
        const QString filePath = it.next();
        QFileInfo info(filePath);
        // @TODO: 这里可以使用 QMediaMetaData 读取元数据，获取标题、艺术家、专辑、封面等信息，这里先简化处理
        TrackModel t;
        t.id        = filePath;
        t.title     = info.completeBaseName();
        t.artist    = QStringLiteral("Local Artist");
        t.album     = QStringLiteral("Local Album");
        t.cover     = QStringLiteral("qrc:/assets/defaultAlbum.jpg");
        t.url       = QUrl::fromLocalFile(filePath).toString();
        t.duration  = 0;
        // 此处留了两个基础字段，方便后续扩展，可以在本地音乐中实现喜欢功能和歌词显示，例如可以导出歌词文件(这些功能都需要基于SQLite实现)
        t.likeStatus = 0;
        t.lyrics    = QString();

        scannedTracks.append(t);

    }

    // 将扫描结果写入 SQLite（通过 LocalTrackTable 封装，不在 MusicPlayer 里直接写 SQL）
    if (!m_localTrackTable.ensureTable()) {
        qWarning() << "MusicPlayer: ensure local_tracks table failed";
    } else {
        if (!m_localTrackTable.clearAll()) {
            qWarning() << "MusicPlayer: clear local_tracks failed";
        }
        if (!m_localTrackTable.insertOrReplaceBatch(scannedTracks)) {
            qWarning() << "MusicPlayer: insert scanned tracks to sqlite failed";
        }
    }

    emit localTracksChanged();
    qDebug() << "MusicPlayer scanned local tracks count:" << m_localTracks.size();
}

int MusicPlayer::localTracksCount() const
{
    // 直接从 SQLite 的 local_tracks 表统计总数
    return m_localTrackTable.count();
}

QVariantList MusicPlayer::localTracksPage(int offset, int limit) const
{
    // 安全检查留给表类或这里简单防御
    if (offset < 0 || limit <= 0)
        return QVariantList();

    // 直接从 SQLite 的 local_tracks 表分页查询
    return m_localTrackTable.page(offset, limit);
}

// ======================= 内部回调 & 辅助函数实现 =======================

void MusicPlayer::handlePositionChanged(qint64 position)
{
    // 将底层 QMediaPlayer 的 positionChanged 直接转发给 QML
    emit positionChanged(position);
}

void MusicPlayer::handlePlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    const bool playing = (state == QMediaPlayer::PlayingState);
    emit playingChanged(playing);

    // 只在自然停止（播放结束）时根据播放模式切歌，避免和手动点击 next/previous 冲突
    if (state == QMediaPlayer::StoppedState && m_player && m_player->position() > 0) {
        if (m_playMode == 2) {
            // 单曲循环
            playIndex(m_state.currentSongIndex);
        } else if (m_playMode == 1 || m_playMode == 0) {
            // 列表循环或顺序播放：自动下一首
            next();
        } else if (m_playMode == 3) {
            // 随机播放
            if (!m_state.trackList.isEmpty()) {
                int newIndex = QRandomGenerator::global()->bounded(m_state.trackList.size());
                playIndex(newIndex);
            }
        }
    }
}

void MusicPlayer::loadCurrentTrack()
{
    if (m_state.currentSongIndex < 0 || m_state.currentSongIndex >= m_state.trackList.size()) {
        return;
    }

    const TrackModel &t = m_state.trackList[m_state.currentSongIndex];

    // 这里假设 url 字段已经是完整的可播放 URL（本地或网络）
    m_player->setSource(QUrl(t.url));

    // 播放前同步设置音量
    m_audioOutput->setVolume(static_cast<float>(m_state.volume) / 100.0f);

    // 通知当前曲目信息变化（附带一个 durationMs 字段，给进度条总时长使用）
    QJsonObject info = trackToJson(t);
    // info["duration"] = m_player->metaData().value(QMediaMetaData::Duration).toInt();
    // qDebug () << "Title:" << m_player->metaData().value(QMediaMetaData::AlbumTitle).toString();
    // qDebug () << "duration ms:"<< info["duration"].toInt();
    // 如果 trackModel 中没有真实时长，这里可以先用 duration 字段的秒数估算
    // if (!info.contains("durationMs")) {
    //     int sec = m_player->metaData().value(QMediaMetaData::Duration).toInt();
    //     // int sec = info.value("duration").toInt();
    //     info["durationMs"] = static_cast<qint64>(sec) * 1000;
    // }
    emit currentTrackInfoChanged(info);
}

// @TODO: 该函数可以提取到 Utils 里复用
// 辅助函数：将 TrackModel 转为 QJsonObject，后续可以写到Utils里复用
QJsonObject MusicPlayer::trackToJson(const TrackModel &t)
{
    QJsonObject obj;
    obj["id"]        = t.id;
    obj["title"]     = t.title;
    obj["artist"]    = t.artist;
    obj["album"]     = t.album;
    obj["cover"]     = t.cover;
    obj["url"]       = t.url;
    obj["duration"]  = t.duration;
    obj["likeStatus"] = t.likeStatus;
    obj["lyrics"]    = t.lyrics;
    return obj;
}
