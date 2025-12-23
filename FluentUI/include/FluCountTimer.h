#pragma once

#include <QObject>
#include <QtQml/qqml.h>
#include <QTimer>
#include "stdafx.h"

/*
 * FluCountTimer
 *
 * 提供可在 QML 中使用的计时器组件，支持正计时（Countup）和倒计时（Countdown）。
 * 属性说明：
 *  - `countType`：计时类型（Countdown / Countup）
 *  - `value`：当前计时值（以毫秒或实现约定的单位）
 *  - `interval`：定时器间隔
 *  - `running`：计时器是否正在运行
 *  - `format`：格式化输出格式（用于生成 `time` 字符串）
 *  - `hour`/`minute`/`second`/`millisecond`/`time`：只读的时间分解与格式化字符串
 *
 * 方法：`start()` / `stop()` / `reset()`，并在倒计时结束时发出 `finished()` 信号。
 */
class FluCountTimer : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(FluCountTimer)
public:
    enum CountType {
        Countdown,
        Countup
    };
    Q_ENUM(CountType)
    Q_PROPERTY_AUTO(CountType, countType)
    Q_PROPERTY_AUTO(int, value)
    Q_PROPERTY_AUTO(int, interval)
    Q_PROPERTY_AUTO(bool, running)
    Q_PROPERTY_AUTO(QString, format)
    Q_PROPERTY_READONLY_AUTO(int, hour)
    Q_PROPERTY_READONLY_AUTO(int, minute)
    Q_PROPERTY_READONLY_AUTO(int, second)
    Q_PROPERTY_READONLY_AUTO(int, millisecond)
    Q_PROPERTY_READONLY_AUTO(QString, time)

public:
    explicit FluCountTimer(QObject *parent = nullptr);

public Q_SLOTS:
    void start();
    void stop();
    void reset(int baseValue = 0);

Q_SIGNALS:
    void finished();

private Q_SLOTS:
    void onTimeout();
    void onRunningChanged();

private:
    void updateTime();
    QString formatTime() const;

    QTimer *_timer;
};
