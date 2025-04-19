//
// Created by 周俊杰 on 2025/4/3.
//

#ifndef BLUETOOTHAUDIOMONITOR_H
#define BLUETOOTHAUDIOMONITOR_H

#include <QCoreApplication>
#include <QBluetoothLocalDevice>
#include <QAudioDevice>
#include <QMediaDevices>
#include "MusicPlayer.h"

class BluetoothAudioMonitor : public QObject{
    Q_OBJECT

public:
    explicit BluetoothAudioMonitor(MusicPlayer* musicPlayer, QObject *parent = nullptr);

    ~BluetoothAudioMonitor();

public slots:
    void onBlueToothConnected(const QBluetoothAddress &address) {
        std::cout << "Bluetooth device connected: " << address.toString().toStdString() << std::endl;
    }

    void onBlueToothDisconnected(const QBluetoothAddress &address) {
        std::cout << "Bluetooth device disconnected: " << address.toString().toStdString() << std::endl;
    }

signals:



private:
    MusicPlayer* musicPlayer;
    QBluetoothLocalDevice *localDevice;
};

#endif //BLUETOOTHAUDIOMONITOR_H
