/**
* @file AudioProcessor.h
* @brief 音频处理与流体封面生成类
* @author zhou2004
* @date 2025-12-14 (Updated)
*/

#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QCoreApplication>
#include <QObject>
#include <QImage>
#include <QColor>
#include <QBuffer>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDir>
#include <QFileInfo>
#include <qimagewriter.h>
#include <QStandardPaths>
// 假设你有 kissfft，如果没有请确保链接了库或者包含相关源码
#include <kissfft/kiss_fft.h>

#include "interface.h"
#include "KGMADecryptor.h"
#include "NCMDecryptor.h"


// 基础
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>

// 格式特定 (必须包含才能转换)

#include <taglib/mpegfile.h>            // MP3
#include <taglib/id3v2tag.h>            // MP3/WAV 图片帧
#include <taglib/attachedpictureframe.h>// 图片帧结构
#include <taglib/flacfile.h>            // FLAC
#include <taglib/mp4file.h>             // M4A, AAC, MP4
#include <taglib/wavfile.h>             // WAV
#include <taglib/vorbisfile.h>          // OGG
#include <taglib/xiphcomment.h>         // OGG 评论

class AudioProcessor : public QObject
{
    Q_OBJECT

public:
    explicit AudioProcessor(QObject *parent = nullptr)
        : QObject(parent)
    {
        // --- 网络初始化 (用于下载网络封面) ---
        networkManager = new QNetworkAccessManager(this);
    }

    ~AudioProcessor()
    {}

    // --- 特征码定义 ---
    // KGMA (VPR Header): 05 28 BC 96 E9 E4 5A 43 91 AA BD D0 7A F5 36 31
    inline static QByteArray KGMA_VPR = QByteArray::fromHex("0528BC96E9E45A4391AABDD07AF53631");
    // KGMA (KGM Header): 7C D5 32 EB 86 02 7F 4B A8 AF A6 8E 0F FF 99 14
    inline static QByteArray KGMA_KGM = QByteArray::fromHex("7CD532EB86027F4BA8AFA68E0FFF9914");
    // NCM: CTENFDAM (43 54 45 4E 46 44 41 4D)
    inline static QByteArray NCM_HEADER = QByteArray::fromHex("4354454E4644414D");
    // FLAC: fLaC
    inline static QByteArray FLAC_HEADER = QByteArray::fromHex("664C6143");
    // MP3 (ID3v2): ID3
    inline static QByteArray MP3_ID3 = QByteArray::fromHex("494433");
    // WAV: RIFF ... WAVE
    inline static QByteArray WAV_RIFF = QByteArray::fromHex("52494646"); // "RIFF"

    // 定义格式枚举，用于后续分支判断
    enum class AudioFormat { Unknown, MP3, FLAC, M4A, WAV, KGMA, NCM, OGG };

    inline static QString cacheCovers = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/FMusic/cache/covers";

    inline static QString cacheSongs = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/FMusic/cache/songs";

    TrackModel static getAudioMetaData(QString &filePath);




    // --- 流体封面处理接口 ---
    /**
     * @brief 处理封面图（支持本地路径和网络URL）
     * 处理完成后会发送 fluidCoverReady 信号
     */
    Q_INVOKABLE void processFluidCover(const QString &path) {
        if (path.isEmpty()) return;

        QString cleanPath = path;
        // 清理 file:// 前缀，方便判断
        if (cleanPath.startsWith("file:///")) cleanPath.remove(0, 8);
        else if (cleanPath.startsWith("file://")) cleanPath.remove(0, 7);

        // 判断是网络 URL 还是本地路径
        if (path.startsWith("http") || path.startsWith("https")) {
            // === 网络图片：异步下载 ===
            QNetworkRequest request{ QUrl(path) };
            QNetworkReply *reply = networkManager->get(request);

            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                if (reply->error() == QNetworkReply::NoError) {
                    QByteArray data = reply->readAll();
                    QImage image;
                    if (image.loadFromData(data)) {
                        generateFluidBase64(image);
                    } else {
                        qWarning() << "AudioProcessor: Failed to load image from network data.";
                    }
                } else {
                    qWarning() << "AudioProcessor: Network error" << reply->errorString();
                }
                reply->deleteLater();
            });

        } else {
            // === 本地图片：直接处理 ===
            QImage image(cleanPath);
            if (!image.isNull()) {
                generateFluidBase64(image);
            } else {
                // 如果加载失败（可能是空封面），生成纯黑
                QImage black(16, 16, QImage::Format_RGB32);
                black.fill(Qt::black);
                generateFluidBase64(black);
            }
        }
    }

private:
    /**
     * @brief 核心算法：降采样 + 调色 + 转Base64
     */
    void generateFluidBase64(const QImage &original) {
        // 1. 极致降采样：强制缩小到 16x16
        // 使用 SmoothTransformation 确保缩小过程平滑（自带高斯模糊效果）
        QImage processed = original.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        // 2. 色彩增强 (模拟 Apple Music 高饱和度)
        for (int y = 0; y < processed.height(); ++y) {
            QRgb *line = (QRgb *)processed.scanLine(y);
            for (int x = 0; x < processed.width(); ++x) {
                QColor color(line[x]);

                int h, s, l, a;
                color.getHsl(&h, &s, &l, &a);

                // 1. 饱和度大幅提升 (x2.5)，让颜色“溢出”
                s = qMin(255, int(s * 2.5));

                // 2. 【核心】强制提亮暗部 (Shadow Lift)
                // 如果亮度低于 60 (255的23%)，强行拉到 60。
                // 这能确保无论专辑多黑，背景都是深灰色或深彩色，而不是死黑。
                // 同时限制最高亮度，防止过曝
                l = qBound(60, l, 200);

                color.setHsl(h, s, l, a);
                line[x] = color.rgb();
            }
        }

        // 3. 转换为 Base64 字符串
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        // 保存为 BMP 或 PNG 都可以，BMP 解码最快，PNG 兼容性好。
        // 这里用 BMP 因为 16x16 极小，无压缩更快。
        processed.save(&buffer, "BMP");

        QString base64 = QString::fromLatin1(byteArray.toBase64());
        QString dataUrl = QString("data:image/bmp;base64,") + base64;

        // 4. 发送结果给 QML
        emit fluidCoverReady(dataUrl);
    }


signals:
    // 【新增信号】当流体封面处理完毕后触发
    void fluidCoverReady(QString base64Data);


private:
    // 网络成员
    QNetworkAccessManager *networkManager;
};

#endif // AUDIOPROCESSOR_H