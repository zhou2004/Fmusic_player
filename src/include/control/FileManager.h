/**
 * @file FileManager.h
 *
 * @brief 文件管理类，管理所有文件。
 *
 * 该类使用 Qt 网络模块实现文件下载功能，支持进度跟踪和错误处理。
 * 实现了下载进度信号，方便界面显示下载进度。
 * 实现了下载完成和下载失败信号，方便界面处理结果。
 * 管理文件的打开和关闭，确保资源正确释放，避免内存泄漏。
 * @TODO 需要实现文件上传和多线程下载，能够合理管理静态资源，包括音频，图片等。
 *
 * @author zhou2004
 * @date 2025-12-18
 */

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QDebug>

class FileManager : public QObject {
    Q_OBJECT

public:
    FileManager() : manager(new QNetworkAccessManager(this)) {
        // 连接信号和槽
        connect(manager, &QNetworkAccessManager::finished, this, &FileManager::onFinished);
    }

    ~FileManager() {
        if (file.isOpen()) {
            file.close();
        }
    }

public slots:
    void downloadFile(const QUrl &url, const QString &outputFileName) {
        file.setFileName(outputFileName);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "无法打开文件：" << outputFileName;
            return;
        }

        QNetworkReply *reply = manager->get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::downloadProgress, this, &FileManager::onDownloadProgress);
    }

public slots:
    void onFinished(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            file.write(reply->readAll());
            qDebug() << "文件下载完成，保存到：" << file.fileName();
            emit downloadFinished(file.fileName());
        } else {
            qDebug() << "下载失败：" << reply->errorString();
            emit downloadFailed(reply->errorString());
        }
        reply->deleteLater();
        file.close();
    }

    void onDownloadProgress(qint64 bytesRead, qint64 totalBytes) {
        qDebug() << "下载进度：" << bytesRead << " / " << totalBytes;
        emit downloadProgress(bytesRead, totalBytes);
    }

signals:
    void downloadProgress(qint64 bytesRead, qint64 totalBytes);
    void downloadFinished(const QString &filePath);
    void downloadFailed(const QString &error);

private:
    QNetworkAccessManager *manager;
    QFile file;
};

// int main(int argc, char *argv[]) {
//     QCoreApplication app(argc, argv);
//
//     FileDownloader downloader;
//     QUrl url("http://example.com/path/to/your/file");
//     QString outputFileName = "downloaded_file";
//
//     downloader.downloadFile(url, outputFileName);
//
//     return app.exec();
// }

#endif //FILEMANAGER_H
