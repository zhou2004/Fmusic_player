//
// Created by 周俊杰 on 2025/3/22.
//

#ifndef LYRICS_H
#define LYRICS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QFile>

#include <QVariant>
#include <QObject>
#include <QThreadPool>
#include <algorithm>

#include "MusicPlayer.h"
#include "Decryptor/KRCDecryptor.h"
#include "Decryptor/KLyricsParser.h"

#include "Iconvert.h"

// 定义存储歌词的结构体
struct Lyric {
    float timestamp; // 时间戳（毫秒）
    std::string content; // 歌词内容
};

// 歌词类型
enum LyricType
{
    Lrc,
    Krc,
    Unknown
};

class Lyrics : public QObject {
    Q_OBJECT

public:
    std::vector<Lyric> lyrics;  // 用于存储歌词的容器
    std::string Lyric_path;

    LyricType type;

    KRCDecoder Kdecoder;
    K_LyricParser KlyricParser;

    explicit Lyrics(MusicPlayer* musicPlayer, QObject *parent = nullptr);
    // ~Lyrics();


    LyricType detectLyricType(const std::filesystem::path& filepath) {
        // 转换为小写比较（避免大小写敏感问题）
        std::string ext = filepath.extension().string();
        std::transform(ext.begin(),  ext.end(),  ext.begin(),
                      [](unsigned char c){ return std::tolower(c); });

        if (ext == ".lrc") return Lrc;
        if (ext == ".krc") return Krc;
        return Unknown;
    }


    // 解析歌词内容
    void parseLyricsContent(const std::string& content) {
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            //将windows的GBK编码转换为Linux的UTF-8编码
            // line = Iconvert::convert_encoding(line, Win_CODE, Linux_CODE);
            if (line.empty()  || line[0] != '[') continue; // 跳过空行和非歌词行
            size_t pos = line.find(']');
            if (pos == std::string::npos) continue; // 跳过格式不正确的行
            // 解析时间戳
            std::string timeStr = line.substr(1,  pos - 1);

            if (timeStr[5] != '.') {
                // 提取歌词内容
                std::string content = line.substr(pos  + 1);
                this->lyrics.emplace_back(Lyric{float(0),  timeStr});
                continue; // 非时间戳行，返回 0
            }
            float minutes = std::stof(timeStr.substr(0,  2));
            float seconds = std::stof(timeStr.substr(3,  2));
            float milliseconds = std::stof(timeStr.substr(6,  2));
            float timestamp = minutes * 60000 + seconds * 1000 + milliseconds * 10;
            // 提取歌词内容
            std::string content = line.substr(pos  + 1);
            // 去除所有换行符（包括 \r 和 \n）
            content.erase(std::remove_if(content.begin(),  content.end(),  [](char c) {
                return (c == '\n' || c == '\r');
            }), content.end());
            // 存入容器
            this->lyrics.emplace_back(Lyric{timestamp,  content});
        }
    }



signals:
    // 歌词改变时调用，用于通知 QML 数据已更新，调用信号：lyricChanged
    void lyricChanged();

    void lyrictypeChanged(QString type);

public slots:

    void test() {
        printf("this is a test!\n");
    }

    bool isFilePath(const QString& filename) {
        QFileInfo fileInfo(filename);
        return fileInfo.exists() && fileInfo.isFile();
    }

    bool isUrlPath(const QString& filename) {
        QUrl url(filename);
        return url.isValid() && url.scheme() != "";
    }




    QVariant Q_getLyrics() {
        if (this->type == Lrc) {
            std::cout << "Lrc" << std::endl;
            QList<QJsonObject> q_lyrics;
            QJsonObject music_detail_info;
            for (const auto &i: this->lyrics) {
                music_detail_info["timestamp"] = qint16(i.timestamp);
                music_detail_info["content"] = QString::fromStdString(i.content);
                q_lyrics.append(music_detail_info);
            }
            return QVariant::fromValue(q_lyrics);  // QList<QJsonObject>
        }else if (this->type == Krc) {
            std::cout << "Krc" << std::endl;
            return QVariant::fromValue(this->KlyricParser.Q_getLyrics());   //QList<QList<QJsonObject>>
        }else {
            std::cout << "Unknown" << std::endl;

        }

    }

    // 解析歌词文件（支持本地路径和网络 URL）
    void parseLyric(QString filename) {
        if (isFilePath(filename) || isUrlPath(filename)) {
            // filename = "D:\\pycharm\\pythonProject\\Fmusic_player\\bin\\Debug\\decoded_output.krc";
            this->type = detectLyricType(std::filesystem::path(filename.toStdString()));

            if (this->type == Lrc) {
                emit this->lyrictypeChanged(QString("Lrc"));
                parseLyrics* task = new parseLyrics(this,filename.toStdString());
                threadPool.start(task);
            }else if (this->type == Krc) {
                emit this->lyrictypeChanged(QString("Krc"));
                // std::cout << "Krc" << std::endl;
                bool flag = Kdecoder._load(filename.toStdString());
                if (flag) {
                    std::string decodedData = Kdecoder.getDecoded();
                    KlyricParser.parseLyrics(decodedData);
                    emit this->lyricChanged();  // 通知 QML Lyrics数据已更新
                }

                // std::map<std::tuple<int, int, int, int>, std::pair<int, int>> timestampMap = KlyricParser.buildTimestampMap(lyricData);

                // int playbackPosition = 215000;
                //
                // K_LyricParser::findLyricAtTime(lyricData, timestampMap, playbackPosition);
                //
                // KlyricParser.Q_getLyrics();

            }else {

            }
        }
        else {
            this->type = Krc;
            std::vector<unsigned char> out;
            if (Kdecoder.Base64Decode(filename.toStdString(),out)) {
                std::string decodedData = Kdecoder.decode(out);
                KlyricParser.parseLyrics(decodedData);
                emit this->lyricChanged();  // 通知 QML Lyrics数据已更新
                std::vector<unsigned char>().swap(out);
            }else {
                KlyricParser.parseLyrics("[0,1]<0,1,0>暂无歌词，敬请期待...");
                emit this->lyricChanged();  // 通知 QML Lyrics数据已更新
            }
        }


    }

    // 查找歌词位置
    QVariant get_Lyricspos(qint32 position) {
        if (this->type == Lrc) {
            // 使用 std::upper_bound 查找第一个 <= position 的时间戳
            auto it = std::upper_bound(this->lyrics.begin(),  this->lyrics.end(),  float(position),
        [](float pos, const Lyric& lyric) {
            return pos < lyric.timestamp;
            });

            if (it != this->lyrics.begin())  {
                // return it->content; // 返回对应的歌词内容
                // 获取 it 后面一个元素的迭代器
                auto prevIt = std::prev(it);
                size_t prevIndex = std::distance(this->lyrics.begin(),  prevIt);
                // printf("pos：%d\n",prevIndex);
                // fflush(stdout);
                return QVariant::fromValue(prevIndex);
            }
            return -1; // 如果没有找到，返回默认值
        } else if (this->type == Krc) {
            return QVariant::fromValue(this->KlyricParser.findLyricAtTime(position));
        } else {
            return -1;
        }

    }

private:
    MusicPlayer* musicPlayer;  // MusicPlayer 的实例

    QThreadPool threadPool;  // 线程池

    class parseLyrics : public QRunnable {
    public:
        std::string filePath;
        explicit parseLyrics(Lyrics* parseLyric,std::string filename) : parseLyric(parseLyric) {
            // 设置任务完成后自动删除
            setAutoDelete(true);
            this->filePath = filename;
        }

        void run() override {
            std::vector<Lyric>().swap(this->parseLyric->lyrics); // 清空之前的歌词
            std::cout << "解析歌词文件: " << this->filePath << std::endl;
            fflush(stdout);
            //
            if (filePath.find("http://")  == 0 || filePath.find("https://")  == 0) {
                // 处理网络 URL
                QNetworkAccessManager manager;
                QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(QString::fromStdString(filePath))));

                QEventLoop loop;
                QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
                loop.exec();

                if (reply->error() == QNetworkReply::NoError) {
                    QByteArray data = reply->readAll();
                    std::string content(data.constData(),  data.length());
                    this->parseLyric->parseLyricsContent(content);
                } else {
                    std::cerr << "网络请求失败: " << reply->errorString().toStdString() << std::endl;
                    return ;
                }
                reply->deleteLater();

                emit this->parseLyric->lyricChanged();  // 通知 QML Lyrics数据已更新
            }
            else {
                // 处理本地文件
                std::ifstream file(std::filesystem::path(filePath),std::ios::binary);
                if (!file.is_open())  {
                    std::cerr << "无法打开文件: " << filePath << std::endl;
                    printf("打不开\n");
                    return ;
                }

                std::string line;
                while (std::getline(file, line)) {

                    //将windows的GBK编码转换为Linux的UTF-8编码
                    // line = Iconvert::convert_encoding(line, Win_CODE, Linux_CODE);

                    if (line.empty()  || line[0] != '[') continue; // 跳过空行和非歌词行

                    size_t pos = line.find(']');
                    if (pos == std::string::npos) continue; // 跳过格式不正确的行

                    // std::cout << "内容: " << line;           // 解析时间戳
                    std::string timeStr = line.substr(1,  pos - 1);
                    // 检查字符串格式
                    if (timeStr[5] != '.') {
                        // 提取歌词内容
                        std::string content = line.substr(pos  + 1);
                        this->parseLyric->lyrics.push_back({float(0),  timeStr});
                        continue; // 非时间戳行，返回 0
                    }

                    float minutes = std::stof(timeStr.substr(0,  2));
                    float seconds = std::stof(timeStr.substr(3,  2));
                    float milliseconds = std::stof(timeStr.substr(6,  2));
                    float timestamp = minutes * 60000 + seconds * 1000 + milliseconds * 10;

                    // 提取歌词内容
                    std::string content = line.substr(pos  + 1);
                    // 去除所有换行符（包括 \r 和 \n）
                    content.erase(std::remove_if(content.begin(),  content.end(),  [](char c) {
                        return (c == '\n' || c == '\r');
                    }), content.end());
                    // 存入容器
                    this->parseLyric->lyrics.push_back({timestamp,  content});
                }

                file.close();
            }


            for (const auto &i: this->parseLyric->lyrics) {
                std::cout << "时间戳：" << i.timestamp << "内容: " << i.content<< std::endl;
                fflush(stdout);
            }

            emit this->parseLyric->lyricChanged();  // 通知 QML Lyrics数据已更新
        }
    private:
        Lyrics* parseLyric;
    };

};

#endif //LYRICS_H
