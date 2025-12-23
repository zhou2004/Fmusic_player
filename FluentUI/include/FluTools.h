#pragma once

#include <QObject>
#include <QFile>
#include <QColor>
#include <QtQml/qqml.h>
#include <QQuickWindow>
#include "singleton.h"

/*
 * FluTools
 *
 * 应用级实用工具单例，向 QML 暴露大量与平台、路径、剪贴板、时间戳、哈希、Base64
 * 等相关的便捷方法。通过 `QML_SINGLETON` 在 QML 中以单例形式访问。
 *
 * 常用方法说明（简要）：
 *  - `qtMajor()`/`qtMinor()`：返回 Qt 的主/次版本号
 *  - `isMacos()`/`isLinux()`/`isWin()`：平台判断
 *  - `clipText(text)`：复制文本到剪贴板
 *  - `uuid()`：生成 UUID 字符串
 *  - `readFile(fileName)`：读取文本文件内容
 *  - `md5/sha256/toBase64/fromBase64`：常用字符串编码/哈希
 *  - `getApplicationDirPath()`、`toLocalPath()`、`getUrlByFilePath()`：文件/URL 相关转换
 *  - `imageMainColor()`：提取图像主色（可选亮度调整）
 */
class FluTools : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(FluTools)
    QML_SINGLETON

private:
    explicit FluTools(QObject *parent = nullptr);

public:
    SINGLETON(FluTools)

    static FluTools *create(QQmlEngine *, QJSEngine *) {
        return getInstance();
    }

    Q_INVOKABLE int qtMajor();
    Q_INVOKABLE int qtMinor();
    Q_INVOKABLE bool isMacos();
    Q_INVOKABLE bool isLinux();
    Q_INVOKABLE bool isWin();

    Q_INVOKABLE void clipText(const QString &text);
    Q_INVOKABLE QString uuid();
    Q_INVOKABLE QString readFile(const QString &fileName);

    Q_INVOKABLE void setQuitOnLastWindowClosed(bool val);
    Q_INVOKABLE void setOverrideCursor(Qt::CursorShape shape);
    Q_INVOKABLE void restoreOverrideCursor();

    Q_INVOKABLE QString html2PlantText(const QString &html);
    Q_INVOKABLE QString toLocalPath(const QUrl &url);

    Q_INVOKABLE void deleteLater(QObject *p);
    Q_INVOKABLE QString getFileNameByUrl(const QUrl &url);
    Q_INVOKABLE QRect getVirtualGeometry();
    Q_INVOKABLE QString getApplicationDirPath();
    Q_INVOKABLE QUrl getUrlByFilePath(const QString &path);

    Q_INVOKABLE QColor withOpacity(const QColor &, qreal alpha);
    Q_INVOKABLE QString md5(const QString &text);
    Q_INVOKABLE QString sha256(const QString &text);
    Q_INVOKABLE QString toBase64(const QString &text);
    Q_INVOKABLE QString fromBase64(const QString &text);

    Q_INVOKABLE bool removeDir(const QString &dirPath);
    Q_INVOKABLE bool removeFile(const QString &filePath);
    Q_INVOKABLE void showFileInFolder(const QString &path);

    Q_INVOKABLE bool isSoftware();
    Q_INVOKABLE qint64 currentTimestamp();
    Q_INVOKABLE QPoint cursorPos();
    Q_INVOKABLE QIcon windowIcon();
    Q_INVOKABLE int cursorScreenIndex();
    Q_INVOKABLE int windowBuildNumber();
    Q_INVOKABLE bool isWindows11OrGreater();
    Q_INVOKABLE bool isWindows10OrGreater();
    Q_INVOKABLE QRect desktopAvailableGeometry(QQuickWindow *window);
    Q_INVOKABLE QString getWallpaperFilePath();
    Q_INVOKABLE QColor imageMainColor(const QImage &image, double bright = 1);
};
