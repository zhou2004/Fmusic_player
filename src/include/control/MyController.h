//
// Created by 周俊杰 on 2025/1/14.
//

// MyController.h
#ifndef MYCONTROLLER_H
#define MYCONTROLLER_H

#include <QObject>
#include <qdebug.h>
#include <iostream>
#include <thread>


// #include "singleton.h"
// #include "stdafx.h"
class MyController : public QObject {
    Q_OBJECT
public:
    explicit MyController(QObject *parent = nullptr) : QObject(parent) {}
    void test(int i ) {
        std::cout << "test:" << i << std::endl;
    }
    // SINGLETON(AppInfo)
public slots:
    void myFunction() {
        std::thread t([this] {
            test(3);
        });
        t.detach();
    }
};


class sss {
public:
    void add() {
        printf("111111");
    }

};

#endif // MYCONTROLLER_H