//
// Created by 周俊杰 on 2025/4/17.
//

#include "QJson.h"


QList<QJsonObject> QJson::kugou_recommend() {
    QList<QJsonObject> q_files;
    QJsonObject musicInfo;
    QString url = kugou_url + "/everyday/recommend";

    QJsonDocument jsonDoc = get(url);
    QJsonObject jsonObj = jsonDoc.object();
    if (jsonObj["data"].toObject()["song_list"].toArray().isEmpty()) {
        return q_files;
    }
    QJsonArray lists = jsonObj["data"].toObject()["song_list"].toArray();
    for (const QJsonValue& value : lists) {
        QJsonObject item = value.toObject();

        musicInfo["hash"] = item["hash"].toString();
        musicInfo["music_name"] = item["songname"].toString();
        musicInfo["art_name"] = item["author_name"].toString();
        musicInfo["cover_url"] =item["sizable_cover"].toString().replace("{size}", "%0").arg(720);
        // musicInfo["lyric_url"] = get_kugou_lyric(item["hash"].toString());
        // musicInfo["play_url"] = get_kugou_url(item["hash"].toString());
        musicInfo["album"] = item["album_name"].toString();
        musicInfo["duration"] = item["time_length"].toInt();
        musicInfo["album_audio_id"] = item["album_audio_id"].toInt();
        // std::cout << "Item: "<< "hash: " << item["hash"].toString().toStdString() << musicInfo["play_url"].toString().toStdString() << " " << item["sizable_cover"].toString().replace("{size}", "%0").arg(720).toStdString() << " " << "SingerName: " << item["author_name"].toString().toStdString() << " " << item["time_length"].toInt() << " " << item["album_name"].toString().toStdString() << " "<< item["songname"].toString().toStdString() <<std::endl;
        q_files.append(musicInfo);
    }
    return q_files;

}

QList<QJsonObject> QJson::kugou_ai_recommend(qint16 album_audio_id) {
    QList<QJsonObject> q_files;
    QJsonObject musicInfo;
    QString searchapi = kugou_url + "/ai/recommend?album_audio_id=%0";
    QString url = searchapi.arg(album_audio_id);
    QJsonDocument jsonDoc = get(url);
    QJsonObject jsonObj = jsonDoc.object();
    if (jsonObj["data"].toObject()["song_list"].toArray().isEmpty()) {
        return q_files;
    }

    QJsonArray lists = jsonObj["data"].toObject()["song_list"].toArray();
    for (const QJsonValue& value : lists) {
        QJsonObject item = value.toObject();
        if (!item["relate_goods"].toArray().isEmpty()) {
            musicInfo["cover_url"] =item["relate_goods"].toArray()[0].toObject()["trans_param"].toObject()["union_cover"].toString().replace("{size}", "%0").arg(720);
            musicInfo["album"] = item["relate_goods"].toArray()[0].toObject()["albumname"].toString();
        }

        musicInfo["hash"] = item["hash"].toString();
        musicInfo["music_name"] = item["songname"].toString();
        musicInfo["art_name"] = item["author_name"].toString();
        // musicInfo["lyric_url"] = get_kugou_lyric(item["hash"].toString());
        // musicInfo["play_url"] = get_kugou_url(item["hash"].toString());
        musicInfo["duration"] = item["time_length"].toInt();
        musicInfo["album_audio_id"] = item["album_audio_id"].toInt();
        // std::cout << "Item: "<< "hash: " << item["hash"].toString().toStdString() << musicInfo["play_url"].toString().toStdString() << " " << item["sizable_cover"].toString().replace("{size}", "%0").arg(720).toStdString() << " " << "SingerName: " << item["author_name"].toString().toStdString() << " " << item["time_length"].toInt() << " " << item["album_name"].toString().toStdString() << " "<< item["songname"].toString().toStdString() <<std::endl;
        q_files.append(musicInfo);
    }
    return q_files;
}
