#pragma once

#include <QObject>
#include <QtQml/qqml.h>

#include "FluAccentColor.h"
#include "stdafx.h"
#include "singleton.h"

/*
 * FluColors
 *
 * 颜色与强调色集合单例，向 QML 暴露一组预定义颜色（Grey 系列、Black/White 等）以及若干 `FluAccentColor` 实例
 *（Yellow/Orange/Red/...）。方便在 QML 中统一引用应用颜色。
 *
 * 提供 `createAccentColor` 用于基于给定主色创建临时/动态的强调色对象。
 */
class FluColors : public QObject {
    Q_OBJECT

    Q_PROPERTY_AUTO(QColor, Transparent)
    Q_PROPERTY_AUTO(QColor, Black)
    Q_PROPERTY_AUTO(QColor, White)
    Q_PROPERTY_AUTO(QColor, Grey10)
    Q_PROPERTY_AUTO(QColor, Grey20)
    Q_PROPERTY_AUTO(QColor, Grey30)
    Q_PROPERTY_AUTO(QColor, Grey40)
    Q_PROPERTY_AUTO(QColor, Grey50)
    Q_PROPERTY_AUTO(QColor, Grey60)
    Q_PROPERTY_AUTO(QColor, Grey70)
    Q_PROPERTY_AUTO(QColor, Grey80)
    Q_PROPERTY_AUTO(QColor, Grey90)
    Q_PROPERTY_AUTO(QColor, Grey100)
    Q_PROPERTY_AUTO(QColor, Grey110)
    Q_PROPERTY_AUTO(QColor, Grey120)
    Q_PROPERTY_AUTO(QColor, Grey130)
    Q_PROPERTY_AUTO(QColor, Grey140)
    Q_PROPERTY_AUTO(QColor, Grey150)
    Q_PROPERTY_AUTO(QColor, Grey160)
    Q_PROPERTY_AUTO(QColor, Grey170)
    Q_PROPERTY_AUTO(QColor, Grey180)
    Q_PROPERTY_AUTO(QColor, Grey190)
    Q_PROPERTY_AUTO(QColor, Grey200)
    Q_PROPERTY_AUTO(QColor, Grey210)
    Q_PROPERTY_AUTO(QColor, Grey220)
    Q_PROPERTY_AUTO_P(FluAccentColor *, Yellow)
    Q_PROPERTY_AUTO_P(FluAccentColor *, Orange)
    Q_PROPERTY_AUTO_P(FluAccentColor *, Red)
    Q_PROPERTY_AUTO_P(FluAccentColor *, Magenta)
    Q_PROPERTY_AUTO_P(FluAccentColor *, Purple)
    Q_PROPERTY_AUTO_P(FluAccentColor *, Blue)
    Q_PROPERTY_AUTO_P(FluAccentColor *, Teal)
    Q_PROPERTY_AUTO_P(FluAccentColor *, Green)
    QML_NAMED_ELEMENT(FluColors)
    QML_SINGLETON

private:
    explicit FluColors(QObject *parent = nullptr);

public:
    SINGLETON(FluColors)

    [[maybe_unused]] Q_INVOKABLE FluAccentColor *createAccentColor(const QColor &primaryColor);

    static FluColors *create(QQmlEngine *, QJSEngine *) {
        return getInstance();
    }
};
