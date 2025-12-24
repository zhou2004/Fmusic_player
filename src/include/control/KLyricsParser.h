/**
* @file KLyricsParser.h
 *
 * @brief 歌词解析类，负责解析各种歌词类型，包括lrc,krc,qrc等。
 *
 * 已实现解析lrc歌词类型。
 * 已实现解析krc歌词类型。
 *
 * @author zhou2004
 * @date 2025-03-30
 */

#ifndef KLYRICSPARSER_H
#define KLYRICSPARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <regex>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <QObject>
#include <QVariant>
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>

// 定义内部结构体方便处理
struct K_Char {
    QString text;
    int start{};    // 相对行开始的时间
    int duration{};
};

struct K_Line {
    int startTime;
    int duration;
    std::vector<K_Char> chars;
    QString tlrc; // 翻译
    QString xlrc; // 谐音/音译 (新增)
};

// LRC临时结构体，用来存储同一时间点的原文和译文
struct LyricLine {
    QString text;
    QString translation;
};


// 歌词类型
enum class LyricType
{
    Lrc,
    Krc,
    Unknown
};

class K_LyricParser  : public QObject {
    Q_OBJECT
public:
    int max_time;
    std::map<std::tuple<int, int, int, int>, std::pair<int, int>> timestampMap;


    Q_INVOKABLE static QVariantList lrc_parseLyrics(const QString& data) {
        QVariantList result;

        // 使用 QMap 自动按时间(int 毫秒)排序
        QMap<int, LyricLine> lyricMap;

        // 1. 按行分割
        QStringList lines = data.split('\n', Qt::SkipEmptyParts);

        // 2. 正则匹配时间标签 [mm:ss.xx] 或 [mm:ss.xxx]
        // 捕获组1: 分钟, 捕获组2: 秒
        QRegularExpression regex(R"(\[(\d+):(\d+(\.\d+)?)\])");

        for (const QString& line : lines) {
            // 查找当前行中所有的匹配项（一行可能有多个时间标签，如 [00:10][00:20]重复歌词）
            QRegularExpressionMatchIterator i = regex.globalMatch(line);

            // 如果没有时间标签，跳过
            if (!i.hasNext()) continue;

            // 提取纯歌词内容：移除该行所有的时间标签，去除首尾空格
            QString content = line;
            content.replace(regex, "").trimmed();

            // 如果内容为空，暂时跳过（根据需求，也可以保留作为占位）
            if (content.isEmpty()) continue;

            // 遍历这行歌词所有的适用时间点
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();

                // 计算时间戳 (分*60*1000 + 秒*1000)
                double min = match.captured(1).toDouble();
                double sec = match.captured(2).toDouble();
                int totalMs = static_cast<int>((min * 60 * 1000) + (sec * 1000));

                // 【核心逻辑】区分原文和翻译
                if (lyricMap.contains(totalMs)) {
                    // 如果时间戳已存在，说明之前已经存了一行（通常是原文）
                    // 那么当前这行内容就是“翻译”
                    // 注意：这里假设文件顺序是 [原文] -> [译文]
                    lyricMap[totalMs].translation = content;
                } else {
                    // 如果时间戳不存在，创建新条目，存为原文
                    lyricMap[totalMs].text = content;
                    lyricMap[totalMs].translation = ""; // 此时翻译为空
                }
            }
        }

        // 3. 将 QMap 转换为 QVariantList (QMap 迭代器默认是按 Key 升序排列的)
        // 这样输出的数据已经是按时间排好序的
        QMap<int, LyricLine>::const_iterator i = lyricMap.constBegin();
        while (i != lyricMap.constEnd()) {
            QVariantMap map;
            map["time"] = i.key();           // 结构体中的 int time
            map["lyric"] = i.value().text;   // 结构体中的 string lyric
            map["tlrc"] = i.value().translation; // 结构体中的 string tlrc

            result.append(map);
            ++i;
        }

        return result;
    }

    // 辅助函数：解析 lyricContent JSON数组为字符串列表
    static QStringList extractLyricList(const QJsonArray& lyricContentArr) {
        QStringList result;
        for (const QJsonValue& lineVal : lyricContentArr) {
            QJsonArray segmentArr = lineVal.toArray();
            QString lineText;
            // lyricContent 的每一项也是一个数组（片段），需要拼接
            for (const QJsonValue& segVal : segmentArr) {
                lineText.append(segVal.toString());
            }
            result.append(lineText);
        }
        return result;
    }

    Q_INVOKABLE static QVariantList parseLyrics(const QString& data) {
        QVariantList result;
        std::vector<K_Line> krcLines;

        // 用于存储解析出来的翻译和音译列表（按行索引排序）
        QStringList tlrcList; // type: 1
        QStringList xlrcList; // type: 0

        // 1. 正则表达式
        // KRC行: [start,duration]...
        QRegularExpression krcLineRegex(R"(\[(\d+),(\d+)\](.*))");
        // KRC字: <start,dur,0>text
        QRegularExpression krcCharRegex(R"(<(\d+),(\d+),(\d+)>([^<]+))");
        // Language Base64块: [language:Base64String]
        QRegularExpression langRegex(R"(\[language:([^\]]+)\])");

        // 2. 按行分割处理
        QStringList rawLines = data.split('\n', Qt::SkipEmptyParts);

        for (const QString& line : rawLines) {
            QRegularExpressionMatch krcMatch = krcLineRegex.match(line);
            QRegularExpressionMatch langMatch = langRegex.match(line);

            if (krcMatch.hasMatch()) {
                // --- A. 解析 KRC 逐字行 ---
                K_Line kLine;
                kLine.startTime = krcMatch.captured(1).toInt();
                kLine.duration = krcMatch.captured(2).toInt();
                kLine.tlrc = "";
                kLine.xlrc = "";

                QString remain = krcMatch.captured(3);

                // 解析具体的字
                QRegularExpressionMatchIterator i = krcCharRegex.globalMatch(remain);
                while (i.hasNext()) {
                    QRegularExpressionMatch charMatch = i.next();
                    K_Char kChar;
                    kChar.start = charMatch.captured(1).toInt();
                    kChar.duration = charMatch.captured(2).toInt();
                    kChar.text = charMatch.captured(4);
                    kLine.chars.push_back(kChar);
                }

                // 存入 KRC 行列表
                krcLines.push_back(kLine);

            } else if (langMatch.hasMatch()) {
                // --- B. 解析 Language Base64 块 ---
                QString base64Str = langMatch.captured(1);
                QByteArray jsonBytes = QByteArray::fromBase64(base64Str.toUtf8());

                QJsonDocument doc = QJsonDocument::fromJson(jsonBytes);
                QJsonObject rootObj = doc.object();

                if (rootObj.contains("content") && rootObj["content"].isArray()) {
                    QJsonArray contentArr = rootObj["content"].toArray();

                    for (const QJsonValue& val : contentArr) {
                        QJsonObject item = val.toObject();
                        int type = item["type"].toInt();
                        QJsonArray lyricContent = item["lyricContent"].toArray();

                        if (type == 1) {
                            // type 1: 中文翻译 (TLRC)
                            tlrcList = extractLyricList(lyricContent);
                        } else if (type == 0) {
                            // type 0: 音译/谐音 (XLRC)
                            xlrcList = extractLyricList(lyricContent);
                        }
                    }
                }
            }
        }

        // 3. 合并逻辑：基于行索引 (Index) 匹配
        // KRC 的行数通常与 translation 的行数一致
        size_t lineCount = krcLines.size();

        for (size_t i = 0; i < lineCount; ++i) {
            // 匹配翻译 (tlrc)
            if (i < static_cast<size_t>(tlrcList.size())) {
                // 去除可能存在的空白符，避免显示空行
                QString t = tlrcList[static_cast<int>(i)].trimmed();
                if (t != " " && !t.isEmpty()) {
                    krcLines[i].tlrc = t;
                }
            }

            // 匹配音译 (xlrc)
            if (i < static_cast<size_t>(xlrcList.size())) {
                QString x = xlrcList[static_cast<int>(i)].trimmed();
                if (x != " " && !x.isEmpty()) {
                    krcLines[i].xlrc = x;
                }
            }
        }

        // 4. 封装为 QVariantList 返回给 QML
        for (const auto& kLine : krcLines) {
            QVariantMap lineMap;
            lineMap["lineStartTime"] = kLine.startTime;
            lineMap["lineDuration"] = kLine.duration;
            lineMap["tlrc"] = kLine.tlrc; // 翻译
            lineMap["xlrc"] = kLine.xlrc; // 音译 (新增)

            QVariantList lyricsList;
            for (const auto& kChar : kLine.chars) {
                QVariantMap m;
                m["text"] = kChar.text;
                m["start_time"] = kChar.start; // 相对时间
                m["duration"] = kChar.duration;
                lyricsList.append(m);
            }
            lineMap["lyrics"] = lyricsList;
            result.append(lineMap);
        }

        return result;
    }


};

#endif //KLRICSPARSER_H
