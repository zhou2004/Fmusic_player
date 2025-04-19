//
// Created by 周俊杰 on 2025/2/16.
//
#include "FAlbumArtManager.h"
#include <QDebug>
#include <kissfft/kiss_fft.h> // 使用 KissFFT 库
FAlbumArtManager::FAlbumArtManager(MusicPlayer* musicPlayer, QObject *parent): QObject(parent), musicPlayer(musicPlayer) {
    threadPool.setMaxThreadCount(4);  // 设置线程池的最大线程数
    threadPool.stackSize(); //即将对象放在堆内存分配
}

FAlbumArtManager::~FAlbumArtManager() {
    threadPool.waitForDone();  // 等待所有任务完成
}


void FAlbumArtManager::test() {
    qDebug() << "Test";
}
