//
// Created by 周俊杰 on 2025/4/9.
//

#ifndef QFILEDOWNLOAD_H
#define QFILEDOWNLOAD_H

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QDebug>

class FileDownloader : public QObject {
    Q_OBJECT

public:
    FileDownloader() : manager(new QNetworkAccessManager(this)) {
        // 连接信号和槽
        connect(manager, &QNetworkAccessManager::finished, this, &FileDownloader::onFinished);
    }

    ~FileDownloader() {
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
        connect(reply, &QNetworkReply::downloadProgress, this, &FileDownloader::onDownloadProgress);
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

#endif //QFILEDOWNLOAD_H
