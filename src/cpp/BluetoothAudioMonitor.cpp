//
// Created by 周俊杰 on 2025/4/3.
//

#include "BluetoothAudioMonitor.h"

BluetoothAudioMonitor::BluetoothAudioMonitor(MusicPlayer* musicPlayer,QObject* parent) : QObject(parent), musicPlayer(musicPlayer) {
    // 初始化本地蓝牙设备
    localDevice = new QBluetoothLocalDevice();
    connect(localDevice, &QBluetoothLocalDevice::deviceConnected, this, &BluetoothAudioMonitor::onBlueToothConnected);

    connect(localDevice, &QBluetoothLocalDevice::deviceDisconnected, this, &BluetoothAudioMonitor::onBlueToothDisconnected);

}


BluetoothAudioMonitor::~BluetoothAudioMonitor() {
    delete localDevice;
}
