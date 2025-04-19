#ifndef FALBUMARTMANAGER_H
#define FALBUMARTMANAGER_H

#include <QObject>
#include <QImage>
#include <QBuffer>
#include <QByteArray>
#include "MusicPlayer.h"  // 包含 MusicPlayer 的头文件

class FAlbumArtManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString base64Data READ albumArtData NOTIFY albumArtDataChanged)

public:
    explicit FAlbumArtManager(MusicPlayer* musicPlayer, QObject *parent = nullptr);

    ~FAlbumArtManager();

    QString albumArtData() const {
        return base64Data;
    }

public slots:
    // 更新专辑图片信息
    void updateAlbumArt() {
        AlbumArtTask* task = new AlbumArtTask(this);
        threadPool.start(task);
    }

    QImage getThumbnailImage() const {
        // 使用 musicPlayer 的方法获取专辑图片
        return this->musicPlayer->player->metaData().value(QMediaMetaData::ThumbnailImage).value<QImage>();
    }

    void test();

signals:
    void albumArtDataChanged();

private:
    QString base64Data;  // 用于存储专辑图片的 Base64 编码数据

    QString someOtherData;  // 用于存储其他数据

    MusicPlayer* musicPlayer;  // MusicPlayer 的实例

    QThreadPool threadPool;  // 线程池

    class AlbumArtTask : public QRunnable {
    public:
        explicit AlbumArtTask(FAlbumArtManager* manager) : manager(manager) {
            // 设置任务完成后自动删除
            setAutoDelete(true);
        }

        void run() override {
            while (true) {
                if (manager->musicPlayer->player->mediaStatus() == QMediaPlayer::LoadedMedia) {
                    QImage albumArt = manager->getThumbnailImage();
                    QByteArray byteArray;
                    QBuffer buffer(&byteArray);
                    albumArt.save(&buffer, "PNG");
                    QString newBase64Data = byteArray.toBase64();

                    manager->base64Data = newBase64Data;

                    // 使用信号和槽机制在主线程中更新数据
                    emit manager->albumArtDataChanged();

                    break;
                }
            }

        }

    private:
        FAlbumArtManager* manager;
    };

    class SomeOtherDataTask : public QRunnable {
    public:
        explicit SomeOtherDataTask(FAlbumArtManager* manager) : manager(manager) {
            // 设置任务完成后自动删除
            setAutoDelete(true);
        }

        void run() override {
            // 模拟一些其他数据的更新
            QString someOtherData = "New Data";

            // 使用信号和槽机制在主线程中更新数据
            QMetaObject::invokeMethod(manager, [this, someOtherData]() {
                manager->someOtherData = someOtherData;
                // emit manager->someOtherDataChanged();
            }, Qt::QueuedConnection);
        }

    private:
        FAlbumArtManager* manager;
    };


};

#endif // FALBUMARTMANAGER_H