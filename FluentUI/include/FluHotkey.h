#ifndef FLUHOTKEY_H
#define FLUHOTKEY_H

#include <QObject>
#include <QQuickItem>
#include "qhotkey/qhotkey.h"
#include "stdafx.h"

/*
 * FluHotkey
 *
 * QML 可用的全局/本地热键封装，基于第三方 `qhotkey` 库。
 * 属性：
 *  - `sequence`：热键字符串（例如 "Ctrl+Shift+H"）
 *  - `name`：描述或标识
 *  - `isRegistered`（只读）：是否已成功注册到系统
 *
 * 信号：`activated()` 在热键触发时发出。
 */
class FluHotkey : public QObject {

    Q_OBJECT
    Q_PROPERTY_AUTO(QString, sequence)
    Q_PROPERTY_AUTO(QString, name)
    Q_PROPERTY_READONLY_AUTO(bool, isRegistered)
    QML_NAMED_ELEMENT(FluHotkey)
public:
    explicit FluHotkey(QObject *parent = nullptr);
    ~FluHotkey();
    Q_SIGNAL void activated();

private:
    QHotkey *_hotkey = nullptr;
};

#endif // FLUHOTKEY_H
