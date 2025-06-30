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

#include "Decryptor/KRCDecryptor.h"
#define TOKEN_PATH "./token.json"

class QJson  : public QObject {
    Q_OBJECT
public:
    QJson() : manager(new QNetworkAccessManager(this)) {
        // 连接信号和槽
        // connect(manager, &QNetworkAccessManager::finished, this, &QJson::onFinished);
        this->cookieValue = "Clion-328fd12f=ff2d3a54-a332-437f-9c19-d466eb8133f7; Clion-aa5a3901=40dc87a7-09ac-4f48-90f2-6d194e9f007a; Idea-33ca3ab2=8faa4c4c-a4ff-45bf-ba49-38d505b847f5; KUGOU_API_PLATFORM=undefined; _tea_utm_cache_10000007=undefined; token=55e3003fdca031aa498d78dbb77aa94262aed977474b075bcbecb0322af6733d; userid=634877158; vip_type=6; vip_token=1605f1613cb551c1644e693872124e91"; // 替换为实际的Cookie值
        this->userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36 Edg/131.0.0.0"; // 替换为实际的user-agent值

    }

    QString kugou_url = "http://127.0.0.1:3000";

    QString cookieValue;
    QString userAgent;

    int request_total = 0;

    Q_INVOKABLE int get_music_total() {
        return this->request_total;
    }
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
            // std::string content(responseData.constData(),  responseData.length());
            //
            // std::cout << "内容: " << content << std::endl;
            // 解析 JSON 数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            reply->deleteLater();
            return jsonDoc;

        }else {
            reply->deleteLater();
            return QJsonDocument();
        }
    }


    Q_INVOKABLE void verify_checkcode(const QString mobile_phone, const QString checkcode) {
        QNetworkAccessManager manager;
        QString seach_api = kugou_url + "/login/cellphone?mobile=%0&code=%1";
        QString url = seach_api.arg(mobile_phone).arg(checkcode);
        QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(url)));

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            // 读取响应数据
            QByteArray responseData = reply->readAll();
            writeDataToFile(responseData);
        }
        else {
            qDebug() << "Error sending check code:" << reply->errorString();
        }
    }




    void writeDataToFile(const QString &data) {
        // 将 QString 转换为标准字符串
        std::string stdFilePath = TOKEN_PATH;
        std::string stdData = data.toStdString();

        // 打开文件以写入
        std::ofstream outFile(stdFilePath, std::ios::out | std::ios::trunc);
        if (!outFile.is_open()) {
            qDebug() << "无法打开文件以写入：" << TOKEN_PATH;
            return;
        }

        // 写入数据
        outFile << stdData;

        // 关闭文件
        outFile.close();

        qDebug() << "数据已成功写入文件：" << TOKEN_PATH;
    }


    void extractToken() {
        // 打开文件
        QFile file(TOKEN_PATH);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开文件：" << TOKEN_PATH;
            return;
        }

        // 读取文件内容
        QByteArray fileContent = file.readAll();
        file.close();

        // 解析 JSON 数据
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "JSON 解析错误：" << parseError.errorString();
            return;
        }

        // 获取根对象
        QJsonObject jsonObject = jsonDoc.object();

        // 提取 token 字段
        QString token = jsonObject.value("data").toObject().value("token").toString();

        // 输出 token
        std::cout << "Token: " << token.toStdString() << std::endl;
        // qDebug() << "Token:" << token;
    }

    Q_INVOKABLE void send_kugou_checkcode(const QString mobile_phone) {
        QNetworkAccessManager manager;
        QString seach_api = kugou_url + "/captcha/sent?mobile=%0";
        QString url = seach_api.arg(mobile_phone);
        QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(url)));

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            // 读取响应数据
            QByteArray responseData = reply->readAll();
        }
        else {
            qDebug() << "Error sending check code:" << reply->errorString();
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
            // std::string content(responseData.constData(),  responseData.length());
            // 将单引号替换为双引号
            responseData.replace('\'', '"').replace("&nbsp;", " ").replace("&quot;", "|");
            // std::cout << "内容: " << content << std::endl;
            // 解析 JSON 数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

            QJsonObject jsonObj = jsonDoc.object();

            QJsonArray abslist = jsonObj["abslist"].toArray();

            if (jsonObj.contains("abslist") && jsonObj["abslist"].isArray()) {
                // 遍历 "abslist" 数组
                for (const QJsonValue& value : abslist) {
                    QJsonObject item = value.toObject();
                    // 处理每个 JSON 对象
                    // std::cout << "Item: " << item["MUSICRID"].toString().remove(0, 6).toStdString() << " " << "pic: " << item["MVPIC"].toString().toStdString() << item["DURATION"].toString().toInt() << " " << item["ALBUM"].toString().toStdString() <<std::endl;
                }
            } else {
                qDebug() << "abslist field not found or not an array";
            }

        } else {

        }
        reply->deleteLater();
    }


    Q_INVOKABLE QList<QJsonObject> kugou_search_music(const QString keyword,int page=0) {
        QList<QJsonObject> q_files;
        QNetworkAccessManager manager;
        QString seach_api = kugou_url + "/search?keywords=%0&page=%1&pagesize=%2";
        QString url = seach_api.arg(keyword).arg(page).arg(10);
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
            if (jsonObj["data"].toObject()["total"].isNull()) {
                reply->deleteLater();
                emit requestTotalChanged(0);
                return q_files;
            }
            this->request_total = jsonObj["data"].toObject()["total"].toInt();
            std:: cout << "request_total: " << this->request_total << std::endl;
            QJsonArray lists = jsonObj["data"].toObject()["lists"].toArray();

            if (!lists.isEmpty()) {
                QJsonObject musicInfo;
                int size = 720;
                // 遍历 "lists" 数组
                for (const QJsonValue& value : lists) {
                    QJsonObject item = value.toObject();
                    // QString seach_api = kugou_url + "/song/url?hash=%0&quality=%1";
                    // QString url = seach_api.arg(item["SQ"].toObject()["Hash"].toString()).arg("high");
                    musicInfo["hash"] = item["SQ"].toObject()["Hash"].toString();
                    musicInfo["music_name"] = item["OriSongName"].toString();
                    musicInfo["art_name"] = item["SingerName"].toString();
                    musicInfo["cover_url"] = item["Image"].toString().replace("{size}", "%0").arg(size);
                    // musicInfo["lyric_url"] = get_kugou_lyric(item["SQ"].toObject()["Hash"].toString());
                    // musicInfo["play_url"] = get_kugou_url(item["SQ"].toObject()["Hash"].toString());
                    musicInfo["album"] = item["AlbumName"].toString();
                    musicInfo["duration"] = item["Duration"].toInt();
                    musicInfo["album_audio_id"] = item["MixSongID"].toInt();
                    q_files.append(musicInfo);
                    // 处理每个 JSON 对象
                    // get(url);
                    // get_kugou_lyric(item["SQ"].toObject()["Hash"].toString());
                    // QString play_url = get_kugou_url(item["SQ"].toObject()["Hash"].toString());
                    // std::cout << "Item: "<< "hash: " << item["SQ"].toObject()["Hash"].toString().toStdString() << musicInfo["play_url"].toString().toStdString() << " " << item["Image"].toString().toStdString() << " " << "SingerName: " << item["SingerName"].toString().toStdString() << " " << item["Duration"].toInt() << " " << item["AlbumName"].toString().toStdString() << " "<< item["OriSongName"].toString().toStdString() <<std::endl;
                }
                reply->deleteLater();
                emit requestTotalChanged(request_total);
                return q_files;
            } else {
                emit requestTotalChanged(0);
                reply->deleteLater();
                return q_files;
            }

        } else {
            emit requestTotalChanged(0);
            reply->deleteLater();
            return q_files;
        }
    }

    Q_INVOKABLE QList<QJsonObject> kugou_recommend();

    Q_INVOKABLE QList<QJsonObject> kugou_ai_recommend(qint16 album_audio_id);


    Q_INVOKABLE QString get_kugou_url(QString Hash,QString quality="high") {
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
            // std::string content(responseData.constData(),  responseData.length());
            //
            // std::cout << "内容: " << content << std::endl;
            // 解析 JSON 数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

            QJsonObject jsonObj = jsonDoc.object();
            reply->deleteLater();
            if (jsonObj.contains("backupUrl") && !jsonObj["backupUrl"].toArray()[0].isNull()) return jsonObj["backupUrl"].toArray()[0].toString();
            if (jsonObj.contains("url") && !jsonObj["url"].toArray()[0].isNull()) return jsonObj["url"].toArray()[0].toString();
            return QString();
        } else {
            reply->deleteLater();
            return QString();
        }
    }

    Q_INVOKABLE QString get_kugou_lyric(QString Hash, QString type="krc") {
        QNetworkAccessManager manager;
        QString seach_api = kugou_url + "/search/lyric?hash=%0";
        QString url = seach_api.arg(Hash);

        QJsonDocument jsonDoc = get(url);
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.contains("candidates") && jsonObj["candidates"].isArray() && !jsonObj["candidates"][0].toObject()["id"].isNull()) {
            QJsonArray candidates = jsonObj["candidates"].toArray();
            QString id = candidates[0].toObject()["id"].toString();
            QString accesskey = candidates[0].toObject()["accesskey"].toString();
            seach_api = kugou_url + "/lyric?id=%0&accesskey=%1&fmt=krc";
            // std::cout << "id: " << id.toStdString() << "accesskey: " << accesskey.toStdString() << std::endl;
            url = seach_api.arg(id).arg(accesskey);
            jsonDoc = get(url);
            return jsonDoc.object()["content"].toString();


        }else {
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

signals:
    void requestTotalChanged(int total);

private:
    QNetworkAccessManager *manager;
    QFile file;

};



#endif //QJSON_H
