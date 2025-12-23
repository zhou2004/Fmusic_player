#pragma once

#include <QObject>
#include <QtQml/qqml.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QColor>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QMutex>
#include "FluAccentColor.h"
#include "stdafx.h"
#include "singleton.h"

/*
 * FluTheme
 *
 * 应用主题管理单例：负责跟踪系统/应用的配色、暗黑模式、壁纸路径、动画与模糊等主题相关设置，
 * 并在需要时通知 QML 侧更新（通过 `darkChanged()` 等信号）。
 *
 * 主要职责：
 *  - 暴露大量颜色属性（primaryColor、backgroundColor、fontPrimaryColor 等）给 QML 使用；
 *  - 提供 `accentColor` 单例引用，用于访问强调色逻辑；
 *  - 监听系统壁纸/配置文件的变化（`QFileSystemWatcher`）并在变化时刷新颜色/壁纸信息；
 *  - 通过 `timerEvent` 或 `eventFilter` 处理运行时需要的更新逻辑。
 */
class FluTheme : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool dark READ dark NOTIFY darkChanged)
    Q_PROPERTY_AUTO_P(FluAccentColor *, accentColor)
    Q_PROPERTY_AUTO(QColor, primaryColor)
    Q_PROPERTY_AUTO(QColor, backgroundColor)
    Q_PROPERTY_AUTO(QColor, dividerColor)
    Q_PROPERTY_AUTO(QColor, windowBackgroundColor)
    Q_PROPERTY_AUTO(QColor, windowActiveBackgroundColor)
    Q_PROPERTY_AUTO(QColor, fontPrimaryColor)
    Q_PROPERTY_AUTO(QColor, fontSecondaryColor)
    Q_PROPERTY_AUTO(QColor, fontTertiaryColor)
    Q_PROPERTY_AUTO(QColor, itemNormalColor)
    Q_PROPERTY_AUTO(QColor, frameColor)
    Q_PROPERTY_AUTO(QColor, frameActiveColor)
    Q_PROPERTY_AUTO(QColor, itemHoverColor)
    Q_PROPERTY_AUTO(QColor, itemPressColor)
    Q_PROPERTY_AUTO(QColor, itemCheckColor)
    Q_PROPERTY_AUTO(QString, desktopImagePath)
    Q_PROPERTY_AUTO(int, darkMode)
    Q_PROPERTY_AUTO(bool, nativeText)
    Q_PROPERTY_AUTO(bool, animationEnabled)
    Q_PROPERTY_AUTO(bool, blurBehindWindowEnabled)
    QML_NAMED_ELEMENT(FluTheme)
    QML_SINGLETON

private:
    explicit FluTheme(QObject *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *event) override;

    void refreshColors();

protected:
    void timerEvent(QTimerEvent *event) override;

    void checkUpdateDesktopImage();

public:
    SINGLETON(FluTheme)

    Q_SIGNAL void darkChanged();

    static FluTheme *create(QQmlEngine *, QJSEngine *) {
        return getInstance();
    }

    bool dark() const;

private:
    bool _systemDark;
    QFileSystemWatcher _watcher;
    QMutex _mutex;
};
