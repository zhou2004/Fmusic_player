//
// Created by 周俊杰 on 2025/3/22.
//

#include "Lyrics.h"

Lyrics::Lyrics(MusicPlayer* musicPlayer, QObject *parent): QObject(parent), musicPlayer(musicPlayer) {
    // threadPool.setMaxThreadCount(4);  // 设置线程池的最大线程数
    // threadPool.stackSize(); //即将对象放在堆内存分配
}
