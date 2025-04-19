//
// Created by 周俊杰 on 2025/4/17.
//

#ifndef QJSON_H
#define QJSON_H
#include <iostream>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkCookie>


class QJson  : public QObject {
    Q_OBJECT
public:
    QJson() : manager(new QNetworkAccessManager(this)) {
        // 连接信号和槽
        // connect(manager, &QNetworkAccessManager::finished, this, &QJson::onFinished);
        this->cookieValue = "Clion-328fd12f=ff2d3a54-a332-437f-9c19-d466eb8133f7; Clion-aa5a3901=40dc87a7-09ac-4f48-90f2-6d194e9f007a; zbx_session=eyJzZXNzaW9uaWQiOiJmY2Y0ODEwZjM3MGU3ZWIyZDYyOTUxMTg5NDA1YThjOSIsInNlcnZlckNoZWNrUmVzdWx0Ijp0cnVlLCJzZXJ2ZXJDaGVja1RpbWUiOjE3NDI0MzkwMzYsInNpZ24iOiI4Y2FlNGJmOTVhYjEzN2U1YzI1NzM4NmVmMDdlYmExNjI3MDc4ZjIyZjhiM2FmYjZiZjJkNDI0MzQ4YmZjNGVmIn0%3D; Idea-33ca3ab2=8faa4c4c-a4ff-45bf-ba49-38d505b847f5; KUGOU_API_PLATFORM=undefined; token=55e3003fdca031aa498d78dbb77aa942b7646808937d5f0ba54bac3445a6364f; userid=634877158; vip_type=6; vip_token=1605f1613cb551c1644e693872124e91; _tea_utm_cache_10000007=undefined"; // 替换为实际的Cookie值
        this->userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36 Edg/131.0.0.0"; // 替换为实际的user-agent值

    }

    QString kugou_url = "http://127.0.0.1:3000";

    QString cookieValue;
    QString userAgent;
    QJsonDocument get(QString url) {
        QNetworkAccessManager manager;
        // 创建请求对象
        QNetworkRequest request;
        request.setUrl(QUrl(url));

        // 设置Cookie请求头
        request.setRawHeader("Cookie", cookieValue.toUtf8());

        // 设置User-Agent请求头
        request.setRawHeader("User-Agent", userAgent.toUtf8());

        QNetworkReply* reply = manager.get(request);

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            // 读取响应数据
            QByteArray responseData = reply->readAll();
            std::string content(responseData.constData(),  responseData.length());

            std::cout << "内容: " << content << std::endl;
            // 解析 JSON 数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            reply->deleteLater();
            return QJsonDocument();

        }else {
            reply->deleteLater();
            return QJsonDocument();
        }
    }



    //通过关键字搜索音乐
    Q_INVOKABLE void search_music(const QString keyword) {
        QNetworkAccessManager manager;
        QString seach_api = "https://search.kuwo.cn/r.s?all=%0&ft=music&%20itemset=web_2013&client=kt&pn=%1&rn=%2&rformat=json&encoding=utf8";
        QString url = seach_api.arg(keyword).arg(0).arg(30);
        QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(url)));

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            // 读取响应数据
            QByteArray responseData = reply->readAll();
            std::string content(responseData.constData(),  responseData.length());
            // 将单引号替换为双引号
            responseData.replace('\'', '"').replace("&nbsp;", " ").replace("&quot;", "|");
            std::cout << "内容: " << content << std::endl;
            // 解析 JSON 数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

            QJsonObject jsonObj = jsonDoc.object();

            QJsonArray abslist = jsonObj["abslist"].toArray();

            if (jsonObj.contains("abslist") && jsonObj["abslist"].isArray()) {
                // 遍历 "abslist" 数组
                for (const QJsonValue& value : abslist) {
                    QJsonObject item = value.toObject();
                    // 处理每个 JSON 对象
                    std::cout << "Item: " << item["MUSICRID"].toString().remove(0, 6).toStdString() << " " << "pic: " << item["MVPIC"].toString().toStdString() << item["DURATION"].toString().toInt() << " " << item["ALBUM"].toString().toStdString() <<std::endl;
                }
            } else {
                qDebug() << "abslist field not found or not an array";
            }

        } else {

        }
        reply->deleteLater();
    }


    Q_INVOKABLE void kugou_search_music(const QString keyword) {
        QNetworkAccessManager manager;
        QString seach_api = kugou_url + "/search?keywords=%0&page=%1&pagesize=%2";
        QString url = seach_api.arg(keyword).arg(0).arg(30);
        QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(url)));

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            // 读取响应数据
            QByteArray responseData = reply->readAll();
            // std::string content(responseData.constData(),  responseData.length());
            //
            // std::cout << "内容: " << content << std::endl;
            // 解析 JSON 数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

            QJsonObject jsonObj = jsonDoc.object();

            QJsonArray lists = jsonObj["data"].toObject()["lists"].toArray();

            if (!lists.isEmpty()) {
                // 遍历 "lists" 数组
                for (const QJsonValue& value : lists) {
                    QJsonObject item = value.toObject();
                    QString seach_api = kugou_url + "/song/url?hash=%0&quality=%1";
                    QString url = seach_api.arg(item["SQ"].toObject()["Hash"].toString()).arg("high");
                    // 处理每个 JSON 对象
                    get(url);
                    // QString play_url = get_kugou_url(item["SQ"].toObject()["Hash"].toString());
                    // std::cout << "Item: " << play_url.toStdString() << " " << item["Image"].toString().toStdString() << " " << "SingerName: " << item["SingerName"].toString().toStdString() << " " << item["Duration"].toInt() << " " << item["AlbumName"].toString().toStdString() << " "<< item["OriSongName"].toString().toStdString() <<std::endl;
                }
            } else {
                qDebug() << "abslist field not found or not an array";
            }

        } else {

        }
        reply->deleteLater();
    }


    QString get_kugou_url(QString Hash,QString quality="high") {
        QNetworkAccessManager manager;
        QString seach_api = kugou_url + "/song/url?hash=%0&quality=%1";
        QString url = seach_api.arg(Hash).arg(quality);

        // 创建请求对象
        QNetworkRequest request;
        request.setUrl(QUrl(url));

        // 设置Cookie请求头
        request.setRawHeader("Cookie", cookieValue.toUtf8());

        // 设置User-Agent请求头
        request.setRawHeader("User-Agent", userAgent.toUtf8());

        QNetworkReply* reply = manager.get(request);

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            // 读取响应数据
            QByteArray responseData = reply->readAll();
            std::string content(responseData.constData(),  responseData.length());

            std::cout << "内容: " << content << std::endl;
            // 解析 JSON 数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

            QJsonObject jsonObj = jsonDoc.object();
            reply->deleteLater();
            if (jsonObj.contains("backupUrl")) return jsonObj["backupUrl"].toArray()[0].toString();
            return QString();
        } else {
            reply->deleteLater();
            return QString();
        }
    }

    QString get_kugou_lyric(QString Hash, QString type="krc") {
        QNetworkAccessManager manager;
        QString seach_api = kugou_url + "/search/lyric?hash=%0";
        QString url = seach_api.arg(Hash);

        // 创建请求对象
        QNetworkRequest request;
        request.setUrl(QUrl(url));

        // 设置Cookie请求头
        request.setRawHeader("Cookie", cookieValue.toUtf8());

        // 设置User-Agent请求头
        request.setRawHeader("User-Agent", userAgent.toUtf8());

        QNetworkReply* reply = manager.get(request);

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            // 读取响应数据
            QByteArray responseData = reply->readAll();
            std::string content(responseData.constData(),  responseData.length());

            std::cout << "内容: " << content << std::endl;
            // 解析 JSON 数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

            QJsonObject jsonObj = jsonDoc.object();
            reply->deleteLater();
            if (jsonObj.contains("lyric")) return jsonObj["lyric"].toString();
            return QString();
        } else {
            reply->deleteLater();
            return QString();
        }

    }

    void get_token() {
        QNetworkAccessManager manager;
        QString seach_api = kugou_url + "/login/token?token=55e3003fdca031aa498d78dbb77aa942b7646808937d5f0ba54bac3445a6364f&userid=634877158";
        // QString url = seach_api.arg(Hash).arg(quality);
        QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(seach_api)));

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            std::string content(responseData.constData(),  responseData.length());

            std::cout << "内容: " << content << std::endl;
            reply->deleteLater();
        }
        reply->deleteLater();

    }

public slots:
    void onFinished(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {

        } else {

        }
        reply->deleteLater();
    }

private:
    QNetworkAccessManager *manager;
    QFile file;

};



#endif //QJSON_H
