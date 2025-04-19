// MusicPlayer.h
#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H
#include <QGuiApplication>
#include <QApplication>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
//媒体播放器
#include <QMediaPlayer>
#include<QSqlDatabase>
//音频输出
#include <QAudioOutput>
//音频设备
#include <QIODevice>
//媒体元数据
#include <QMediaMetaData>

#include <QQueue>
#include <vector>
#include <string>
#include <mutex>
#include <stdio.h>
#include <iostream>
#include <string>
#include <queue>

#include <QThreadPool>
#include <thread>
#include <QRunnable>

//用与文件过滤Qt库
#include <QDirIterator>
#include <QStringList>
#include <QDir>
#include <QBuffer>

#include "Decryptor/KGMADecryptor.h"
#include "Decryptor/NCMDecryptor.h"

#include <QAudioSink>
#include <QAudioOutput>

#include <kissfft/kiss_fft.h>

#include "Sql/mysql.h"
#include "QFiledownload.h"
struct MusicFile {
    std::string filePath; // 音乐文件的完整路径
    std::string lyric_url;
};

class MusicPlayer : public QObject {
    Q_OBJECT
    friend class FAlbumArtManager;  // 声明 FAlbumArtManager 为友元类
public:


    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QAudioDevice device;
    //qml中选择的音乐文件路径;
    std::string music_path; //播放中的音乐文件路径
    std::string music_folder;

    QList<QJsonObject> q_files;  // 用于存储所有音乐文件的容器,用于qml


    //音乐文件路径列表
    std::vector<MusicFile> musicFiles; // 用于存储所有音乐文件的容器

    explicit MusicPlayer(QObject *parent = nullptr);

    ~MusicPlayer();



    Q_INVOKABLE void play(const QString &musicPath);
    Q_INVOKABLE void loaded_play();
    Q_INVOKABLE int Pre_play(int64_t current_index);
    Q_INVOKABLE int Next_play(int current_index);
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setMedia(const QString &filePath);
    Q_INVOKABLE void setPosition(int64_t position);
    Q_INVOKABLE void setVolume(int64_t volume);




    //搜索特定路径下的所有音乐文件
    Q_INVOKABLE void search_files(const char *path);

    //选择文件路径
    Q_INVOKABLE void select_folder_and_save_to_ini();

    Q_INVOKABLE std::string get_Directory(const QString& music_folder);

    void searchFiles(const std::string &directoryPath, const QStringList &nameFilters);


    //
    Q_INVOKABLE QList<QJsonObject> get_all_music_path();

    Q_INVOKABLE QString getTitle() const {
        return this->player->metaData().value(QMediaMetaData::Title).toString();
    }

    Q_INVOKABLE QString getArtist() const {
        return this->player->metaData().value(QMediaMetaData::ContributingArtist).toString();
    }

    Q_INVOKABLE QString getAlbum() const {
        return this->player->metaData().value(QMediaMetaData::AlbumTitle).toString();
    }

    Q_INVOKABLE qint64 getDuration() const {
        return this->player->metaData().value(QMediaMetaData::Duration).toInt(); // 返回秒
    }

    Q_INVOKABLE QString getAuthor() const {
        return this->player->metaData().value(QMediaMetaData::ContributingArtist).toString();
    }

    Q_INVOKABLE QString getThumbnailImage() const {
        QImage albumArt = this->player->metaData().value(QMediaMetaData::ThumbnailImage).value<QImage>();
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        albumArt.save(&buffer, "PNG");
        QString newBase64Data = byteArray.toBase64();
        return newBase64Data;
    }

    Q_INVOKABLE QJsonObject getMusicInfo() const ;


    Q_INVOKABLE void Set_Q_Current_Task(int index);

    Q_INVOKABLE qint64 Get_Q_Position() const;

    Q_INVOKABLE QList<QJsonObject> Get_Q_refresh_all_music();

    Q_INVOKABLE qint16 Get_Q_request_total() const {
        return this->musicFiles.size();
    }

    Q_INVOKABLE void Set_Q_current_page(qint16 page) {
        this->current_page = page - 1;
        std::cout << "当前页面：" << this->current_page << std::endl;
    }




    // ------------------------------------------mysql驱动调用接口music

    QList<QJsonObject> save_playlist(qint64 playlist_id);

    Q_INVOKABLE QList<QJsonObject> Get_All_Music();

    Q_INVOKABLE QList<QJsonObject> Get_Music_Type(QString type_name);


    // ---------------------------------------------mysql驱动调用接口Playlist

    Q_INVOKABLE QList<QJsonObject> Get_Playlist();

    Q_INVOKABLE QList<QJsonObject> Get_PlaylistDetail(qint64 playlist_id);

    // ----------------------------------------------mysql驱动调用接口user

    Q_INVOKABLE bool regist_user(QString user_name, QString user_password);

    Q_INVOKABLE UserFullInfo login_user(QString user_name, QString user_password);




    void test() {
        qDebug() << "Decoding started.";
        KGMA decoder;
        decoder.KGMADecrypt("D:/KuGou/KugouMusic/KugouMusic/Dripice_Escape.kgm","D:/KuGou/KugouMusic/KugouMusic/");
    }

    Q_INVOKABLE void decoded() {
        // 使用 std::thread 启动一个后台线程来执行 test 函数
        // std::thread decodeThread([this]() {
        //     this->test();
        // });
        // decodeThread.detach(); // 让线程独立运行，即使创建线程的线程已经退出
        // bool _write163Key = true;

        // NCM decoder;
        // decoder.NCMDecrypt("D:/KuGou/KugouMusic/123.ncm",true,"D:/KuGou/KugouMusic");

        // KGMA decoder;
        // decoder.KGMADecrypt("D:/KuGou/KugouMusic/KugouMusic/Dripice_Escape.kgm","D:/KuGou/KugouMusic/KugouMusic/");
    }


    // 播放器状态改变时调用，用于通知 QML 数据已更新，调用信号：metaDataChanged
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            player->setPosition(this->desiredPosition);
            // this->getMusicInfo();
            // emit metaDataChanged();  // 通知 QML 数据已更新
        }
    }


    // 播放器元数据改变时调用，用于通知 QML 数据已更新，调用信号：metaDataChanged
    void onMetaDataChanged() {
        std::cout << "metaData......" <<std::endl;
        emit metaDataChanged();  // 通知 QML 数据已更新
    }

    // 播放器播放位置改变时调用，用于通知 QML 数据已更新，调用信号：positionChanged
    void onPositionChanged(qint64 position) {
        // std::cout << "Position:" << this->player->position() << std::endl;
        emit positionChanged();  // 通知 QML 数据已更新
    }

    void onPlayingChanged(bool playing) {
        emit playingChanged(playing);  // 通知 QML 数据已更新
    }


private:

    // 当前任务索引
    int currentTaskIndex = -1;

    size_t current_page = 0; // 当前页码

    int64_t desiredPosition = 0; // 播放器位置
    // 在类定义中添加成员变量
    int64_t pendingPosition = -1;

    std::mutex mutex; // 用于线程安全的 q_files 操作

    Mysql mysql;

    FileDownloader downloader;

    UserFullInfo user; //user用户基本信息




signals:
    void metaDataChanged();

    void positionChanged();

    void musicfolderChanged();

    void musicInfoChanged();

    void playingChanged(bool playing);

    void userChanged(UserFullInfo c_user);





};



#endif // MUSICPLAYER_H