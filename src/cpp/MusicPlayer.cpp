// MusicPlayer.cpp
#include "MusicPlayer.h"

MusicPlayer::MusicPlayer(QObject *parent) : QObject(parent) {
    // 初始化 QMediaPlayer 和 QAudioOutput
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    device = audioOutput->device();

    // 获取设备支持的音频格式信息
    // QAudioFormat format = device.preferredFormat();
    // format.sampleRate();
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::onMediaStatusChanged);
    connect(player, &QMediaPlayer::metaDataChanged, this, &MusicPlayer::onMetaDataChanged);
    // 连接信号
    connect(player, &QMediaPlayer::positionChanged, this, &MusicPlayer::onPositionChanged);

    connect(player, &QMediaPlayer::playingChanged, this, &MusicPlayer::onPlayingChanged);

}

MusicPlayer::~MusicPlayer() {
    delete player;
    player = nullptr;

    delete audioOutput;
    audioOutput = nullptr;
}

bool MusicPlayer::play(const QString &musicPath) {
    player->stop(); // 停止当前播放的音乐

    this->music_path = musicPath.toStdString();
    this->player->setSource(QUrl(musicPath));
    this->audioOutput->setVolume(100);

    // 检查媒体状态
    QMediaPlayer::MediaStatus status = player->mediaStatus();
    if (status != QMediaPlayer::LoadingMedia) {
        // 无效的媒体文件
        return false;
    }
    this->player->play(); // 开始播放
    return true;
}

bool MusicPlayer::repeat_play(const QString &musicPath) {
    this->music_path = musicPath.toStdString();
    this->player->setSource(QUrl(musicPath));
    this->audioOutput->setVolume(100);
    this->player->play(); // 开始播放
    return true;
}

//加载过media的播放
void MusicPlayer::loaded_play() {
    //直接播放
    this->player->play(); // 开始播放
}

void MusicPlayer::pause() {
    this->player->pause();
}

void MusicPlayer::stop() {
    this->player->play();
    this->player->stop();
}

int MusicPlayer::Pre_play(int64_t current_index) {
    if (current_index <= 0) {
        return 1;
    }

    player->stop(); // 停止当前播放的音乐

    this->music_path = this->musicFiles[int(current_index)-1].filePath.c_str();
    this->player->setSource(QUrl(QString::fromStdString(this->music_path)));
    this->audioOutput->setVolume(100);
    this->player->play(); // 开始播放
    this->currentTaskIndex = int(current_index) -1;

    return 0;
}

int MusicPlayer::Next_play(int current_index) {
    if (current_index >= this->musicFiles.size() - 1) {
        return 1;
    }
    player->stop(); // 停止当前播放的音乐
    this->music_path = this->musicFiles[int(current_index)+1].filePath.c_str();
    this->player->setSource(QUrl(QString::fromStdString(this->music_path)));
    this->audioOutput->setVolume(100);
    this->player->play(); // 开始播放
    this->currentTaskIndex = int(current_index) + 1;

    return 0;
}


void MusicPlayer::setMedia(const QString &filePath) {
    this->player->setSource(QUrl(filePath));
}

void MusicPlayer::setPosition(int64_t position) {
    this->desiredPosition = position; // 存储目标位置

    // 直接尝试定位（可能因缓冲不足失败）
    player->setPosition(position);

    // 连接信号以处理缓冲完成后的重试
    connect(player, &QMediaPlayer::bufferProgressChanged,
            this, [this](float progress) {
        if (progress >= 0.8 && this->desiredPosition != -1) {
            player->setPosition(this->desiredPosition);
            this->desiredPosition = -1; // 重置标记
        }
    });
}

void MusicPlayer::setVolume(int64_t volume) {
    this->audioOutput->setVolume(volume);
}

void MusicPlayer::search_files(const char *path) {

}

// 弹出文件夹选择对话框并保存路径到INI文件
void MusicPlayer::select_folder_and_save_to_ini() {

}

// 从qml中获取music_folder路径传回c++中
std::string MusicPlayer::get_Directory(const QString& music_folder) {
    // 直接将 QString 转换为 std::string
    std::string folder = music_folder.toStdString();

    // 保存路径到类的成员变量（如果需要）
    this->music_folder = folder;

    // 使用 printf 输出 std::string
    printf("C++文件路径: %s\n", this->music_folder.c_str());

    // 刷新标准输出缓冲区
    fflush(stdout);

    QStringList filters;
    filters << "*.mp3" << "*.flac" << "*.ogg";
    // std::thread t([this, filters] {
    //     this->searchFiles(this->music_folder, filters);
    // });
    // t.detach();
    this->searchFiles(this->music_folder,filters);

    return folder;
}


void MusicPlayer::searchFiles(const std::string &directoryPath, const QStringList &nameFilters) {
    // 清空 musicFiles 并释放内存
    std::vector<MusicFile>().swap(this->musicFiles);
    this->current_page = 0;
    std::cout << "Searching directory: " << directoryPath << std::endl;
    std::cout << "Filters: ";
    for (const auto& filter : nameFilters) {
        std::cout << filter.toStdString() << " ";
    }
    std::cout << std::endl;

    // 将 URI 格式的路径转换为标准的文件系统路径
    QUrl url(QString::fromStdString(directoryPath));
    QString localPath = url.toLocalFile();  // 转换为本地文件路径
    std::cout << "Converted local path: " << localPath.toStdString() << std::endl;

    QDir dir(localPath);
    if (!dir.exists()) {
        std::cerr << "Directory does not exist: " << localPath.toStdString() << std::endl;
        return;
    }

    QStringList filesFound;
    QDirIterator it(dir.path(), nameFilters, QDir::NoFilter, QDirIterator::Subdirectories);


    while (it.hasNext()) {
        QString filePath = it.next();
        filesFound.append(filePath);
    }

    // 将找到的所有符合的文件存入std::vector<MusicFile>中
    for (const QString &file : filesFound) {
        this->musicFiles.emplace_back(MusicFile{file.toStdString()});
    }
    // 打印所有找到的文件
    for (const auto& file : this->musicFiles) {
        std::cout << "Music File: " << file.filePath << std::endl;
    }
    emit this->musicfolderChanged();  // 通知 QML 数据已更新
}



QList<QJsonObject> MusicPlayer::Get_Q_refresh_all_music() {
    return this->q_files;
}

QList<QJsonObject> MusicPlayer::get_all_music_path() {
    QList<QJsonObject> q_files;
    std::vector<std::vector<std::string>> fileGroups;
    // 将文件路径分组
    size_t numFiles = 10;
    size_t numThreads = 10;
    size_t groupSize = (numFiles + numThreads - 1) / numThreads; // 每个线程处理的文件数量
    std::vector<QList<QJsonObject>> threadResults(numThreads); // 为每个线程分配一个结果列表


    for (size_t i = 0; i < numThreads; ++i) {
        std::vector<std::string> group;
        size_t start = i * groupSize + this->current_page * numFiles;
        size_t end = std::min(start + groupSize,this->musicFiles.size());
        for (size_t j = start; j < end; ++j) {
            group.push_back(musicFiles[j].filePath);
        }
        fileGroups.push_back(group);
        std::vector<std::string>().swap(group); // 释放局部变量
    }

    // 创建线程
    std::vector<std::thread> threads;
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([this, &fileGroups, &threadResults, i]() {
            QMediaPlayer localPlayer;
            QAudioOutput localAudioOutput;
            localPlayer.setAudioOutput(&localAudioOutput);

            QList<QJsonObject> q_file; // 每个线程的局部结果列表
            for (const auto& filePath : fileGroups[i]) {
                localPlayer.setSource(QUrl::fromLocalFile(filePath.c_str()));
                localPlayer.play();
                localPlayer.pause();

                // 等待元数据加载完成
                QEventLoop loop;
                connect(&localPlayer, &QMediaPlayer::mediaStatusChanged, &loop, [&loop](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::LoadedMedia) {
                        loop.quit();
                    }
                });
                loop.exec();

                if (localPlayer.mediaStatus() != QMediaPlayer::LoadedMedia) {
                    QJsonObject musicInfo;
                    QImage albumArt = localPlayer.metaData().value(QMediaMetaData::ThumbnailImage).value<QImage>();
                    QByteArray byteArray;
                    QBuffer buffer(&byteArray);
                    albumArt.save(&buffer, "PNG");
                    QString newBase64Data = byteArray.toBase64();
                    musicInfo["albumimage"] = newBase64Data;
                    musicInfo["title"] = localPlayer.metaData().value(QMediaMetaData::Title).toString();
                    musicInfo["Author"] = localPlayer.metaData().value(QMediaMetaData::ContributingArtist).toString();
                    musicInfo["album"] = localPlayer.metaData().value(QMediaMetaData::AlbumTitle).toString();
                    musicInfo["duration"] = localPlayer.metaData().value(QMediaMetaData::Duration).toInt();
                    musicInfo["filepath"] = QString::fromStdString(filePath);
                    q_file.append(musicInfo);

                    std::cout << "Music File: " << musicInfo["filepath"].toString().toStdString()
                              << " title: " << musicInfo["title"].toString().toStdString() << std::endl;
                }
            }
            threadResults[i] = q_file; // 将局部结果存储到对应的位置

            // 释放资源
            localPlayer.stop();
            localPlayer.setSource(QUrl());
            localPlayer.deleteLater();
            localAudioOutput.deleteLater();



        });
    }

    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }

    // 按顺序合并结果
    for (size_t i = 0; i < numThreads; ++i) {
        q_files.append(threadResults[i]);
    }

    std::vector<std::thread>().swap(threads); // 释放线程变量
    std::vector<QList<QJsonObject>>().swap(threadResults); // 释放局部变量
    std::vector<std::vector<std::string>>().swap(fileGroups); // 释放局部变量)
    return q_files;
}

QJsonObject MusicPlayer::getMusicInfo() const {
    QJsonObject musicInfo;
    musicInfo["title"] = this->getTitle();
    musicInfo["artist"] = this->getArtist();
    musicInfo["album"] = this->getAlbum();
    musicInfo["albumimage"] = this->getThumbnailImage();
    musicInfo["duration"] = QString::number(this->getDuration());
    musicInfo["Author"] = this->getAuthor();
    // std::cout << "curent_task:" << this->currentTaskIndex << std::endl;
    return musicInfo;
}

void MusicPlayer::Set_Q_Current_Task(int index) {
    currentTaskIndex = index;
    std::cout << "curent_task:" << currentTaskIndex << std::endl;
    fflush(stdout);
}


//qml中获取播放进度
qint64 MusicPlayer::Get_Q_Position() const {
    return this->player->position();
}



//mysql驱动调用部分

QList<QJsonObject> MusicPlayer::save_playlist(qint64 playlist_id) {
    QList<QJsonObject> PlaylistDetail = mysql.query_PlaylistDetail(playlist_id);
    //清空之前保存的歌曲
    std::vector<MusicFile>().swap(this->musicFiles);
    for (auto &temp :PlaylistDetail ) {
        this->musicFiles.emplace_back(MusicFile{temp["play_url"].toString().toStdString(),temp["lyric_url"].toString().toStdString()});
    }
    // 打印所有找到的文件
    // for (const auto& file : this->musicFiles) {
    //     std::cout << "Music File: " << file.filePath << std::endl;
    // }
    return PlaylistDetail;
}

QJsonArray MusicPlayer::Get_All_Music() {
    return mysql.query_musicTable();
}

QJsonArray MusicPlayer::Get_Music_Type(QString type_name) {
    return mysql.query_musictype(type_name);
}

QList<QJsonObject> MusicPlayer::Get_Playlist() {
    return mysql.query_Playlist();
}

QList<QJsonObject> MusicPlayer::Get_PlaylistDetail(qint64 playlist_id) {
    return this->save_playlist(playlist_id);
}


bool MusicPlayer::regist_user(QString user_name, QString user_password) {
    UserCreateInfo newUser;
    newUser.nickname = user_name;
    newUser.password = user_password;
    qint64 userId = mysql.createUser(newUser);
    if (userId == -1) {
        qCritical() << "创建用户失败";
        return false;
    }
    return true;

}

UserFullInfo MusicPlayer::login_user(QString user_name, QString user_password) {
    UserFullInfo user = mysql.getUserByname(user_name);
    if (mysql.verifyPassword(user_password,user.password)) {
        this->user = user;
        emit userChanged(this->user);
        return user;
    }else {
        return UserFullInfo();
    }

}



bool MusicPlayer::add_user_collections(qint64 userId, qint64 musicId) {
    return mysql.addUserCollections(userId, musicId);
}


bool MusicPlayer::del_user_collections(qint64 userId, qint64 musicId) {
    return mysql.delUserCollections(userId, musicId);
}


QList<QJsonObject> MusicPlayer::get_user_collections(qint64 userId,int page) {
    return mysql.getUserCollections(userId,page);
}


//D:\KuGou\KugouMusic