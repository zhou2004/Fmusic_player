/**
* @file AudioProcessor.h
* @brief 音频处理与流体封面生成类
* @author zhou2004
* @date 2025-12-14 (Updated)
*/

#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QCoreApplication>
#include <QMediaPlayer>
#include <QAudioBufferOutput>
#include <QAudioBuffer>
#include <QObject>
#include <QImage>
#include <QColor>
#include <QBuffer>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <cmath>
#include <iostream>

// 假设你有 kissfft，如果没有请确保链接了库或者包含相关源码
#include <kissfft/kiss_fft.h>

#include "MusicPlayer.h"

class AudioProcessor : public QObject
{
    Q_OBJECT

public:
    explicit AudioProcessor(MusicPlayer* musicPlayer, QObject *parent = nullptr)
        : QObject(parent), musicPlayer(musicPlayer)
    {
        // --- 音频初始化 ---
        audioBufferOutput = new QAudioBufferOutput(this);
        this->spectrumList.resize(32);
        connect(audioBufferOutput, &QAudioBufferOutput::audioBufferReceived, this, &AudioProcessor::handleAudioBuffer);

        // 初始化 kissfft
        int nfft = 1024;
        cfg = kiss_fft_alloc(nfft, 0, 0, 0);
        cx_in = new kiss_fft_cpx[nfft];
        cx_out = new kiss_fft_cpx[nfft];

        // --- 网络初始化 (用于下载网络封面) ---
        networkManager = new QNetworkAccessManager(this);
    }

    ~AudioProcessor()
    {
        kiss_fft_free(cfg);
        delete[] cx_in;
        delete[] cx_out;
    }

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

public slots:
    QList<float> get_spectrumList() const {
        return this->spectrumList;
    }

signals:
    void spectrumListChanged();

    // 【新增信号】当流体封面处理完毕后触发
    void fluidCoverReady(QString base64Data);

private slots:
    void handleAudioBuffer(const QAudioBuffer &buffer)
    {
        // ... (保持原有的 FFT 逻辑不变) ...
        const char *data = buffer.constData<char>();
        int size = buffer.byteCount();

        int samples = size / 2;
        for (int i = 0; i < samples && i < nfft; ++i)
        {
            int16_t sample;
            memcpy(&sample, data + i * 2, sizeof(int16_t));
            cx_in[i].r = sample / 32768.0;
            cx_in[i].i = 0.0;
        }

        kiss_fft(cfg, cx_in, cx_out);
        calculateSpectrum();
    }

    void calculateSpectrum()
    {
        // ... (保持原有的频谱计算逻辑不变) ...
        float spectrum[nfft / 2];
        for (int i = 0; i < nfft / 2; ++i)
        {
            spectrum[i] = sqrt(cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i);
        }

        constexpr int bands = 32;
        float bandSize = (nfft / 2) / bands;
        float spectrumBands[bands];

        for (int i = 0; i < bands; ++i)
        {
            float sum = 0;
            for (int j = 0; j < bandSize; ++j)
            {
                int index = i * bandSize + j;
                sum += spectrum[index];
            }
            spectrumBands[i] = sum / bandSize;
            this->spectrumList.replace(i, spectrumBands[i]);
        }
        emit spectrumListChanged();
    }

private:
    MusicPlayer* musicPlayer;
    QAudioBufferOutput *audioBufferOutput;
    QList<float> spectrumList = {30};

    // FFT 成员
    static constexpr int nfft = 1024;
    kiss_fft_cfg cfg;
    kiss_fft_cpx *cx_in;
    kiss_fft_cpx *cx_out;

    // 网络成员
    QNetworkAccessManager *networkManager;
};

#endif // AUDIOPROCESSOR_H