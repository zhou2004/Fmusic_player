//
// Created by 周俊杰 on 2025/3/26.
//

#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QCoreApplication>
#include <QMediaPlayer>
#include <QAudioBufferOutput>
#include <QAudioBuffer>
#include <kissfft/kiss_fft.h>
#include <iostream>
#include <stdio.h>

#include "MusicPlayer.h"

class AudioSpectrumAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit AudioSpectrumAnalyzer(MusicPlayer* musicPlayer, QObject *parent = nullptr) : QObject(parent), musicPlayer(musicPlayer)
    {

        audioBufferOutput = new QAudioBufferOutput(this);

        this->musicPlayer->player->setAudioBufferOutput(audioBufferOutput);

        this->spectrumList.resize(32);

        // 连接音频缓冲信号到处理槽函数
        connect(audioBufferOutput, &QAudioBufferOutput::audioBufferReceived, this, &AudioSpectrumAnalyzer::handleAudioBuffer);

        // 初始化 kissfft
        nfft = 1024; // FFT 点数，可以根据需要调整
        cfg = kiss_fft_alloc(nfft, 0, 0, 0);
        cx_in = new kiss_fft_cpx[nfft];
        cx_out = new kiss_fft_cpx[nfft];
    }

    ~AudioSpectrumAnalyzer()
    {
        kiss_fft_free(cfg);
        delete[] cx_in;
        delete[] cx_out;
    }

    void setUrl(const QUrl &url)
    {
        this->musicPlayer->player->setSource(url);
        this->musicPlayer->player->play();
    }

public slots:
    QList<float> get_spectrumList() const {
        return this->spectrumList;
    }

signals:
    void spectrumListChanged();

private slots:
    void handleAudioBuffer(const QAudioBuffer &buffer)
    {
        // 获取音频数据
        const char *data = buffer.constData<char>();
        int size = buffer.byteCount();

        // 将音频数据转换为 kissfft 的输入格式
        int samples = size / 2; // 假设 16 位音频数据
        for (int i = 0; i < samples && i < nfft; ++i)
        {
            int16_t sample;
            memcpy(&sample, data + i * 2, sizeof(int16_t));
            cx_in[i].r = sample / 32768.0; // 归一化到 [-1, 1]
            cx_in[i].i = 0.0;
        }

        // 执行 FFT
        kiss_fft(cfg, cx_in, cx_out);

        // 计算频谱并分段
        calculateSpectrum();
    }

    void calculateSpectrum()
    {
        // 计算频谱大小
        float spectrum[nfft / 2];
        for (int i = 0; i < nfft / 2; ++i)
        {
            spectrum[i] = sqrt(cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i);
        }

        // 将频谱分成 32 个频段
        int bands = 32;
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
            spectrumBands[i] = sum / bandSize; // 平均值
            this->spectrumList.replace(i, spectrumBands[i]);
        }

        // 输出频谱数据（用于调试或显示）
        // for (int i = 0; i < bands; ++i)
        // {
        //     std::cout << "Band" << i << ":" << spectrumBands[i];
        //     fflush(stdout);
        // }
        // printf("\n");
        // emit spectrumListChanged();
    }

private:
    MusicPlayer* musicPlayer;  // MusicPlayer 的实例
    QAudioBufferOutput *audioBufferOutput;
    QList<float> spectrumList = {30};
    int nfft;
    kiss_fft_cfg cfg;
    kiss_fft_cpx *cx_in;
    kiss_fft_cpx *cx_out;
};

#endif //AUDIOPROCESSOR_H
