#pragma once

#include <QObject>
#include <QtQml/qqml.h>
#include <QColor>
#include "stdafx.h"

/*
 * FluAccentColor
 *
 * 主题强调色（accent color）调色板封装：提供从 `darkest` 到 `lightest` 的多个颜色等级，便于在
 * 应用中统一使用和调整高对比或弱对比场景下的颜色。该类通过 `Q_PROPERTY_AUTO` 暴露属性，
 * 并使用 `QML_NAMED_ELEMENT` 使其在 QML 中可被直接访问（例如 `FluAccentColor { }` 或从样式表中引用）。
 *
 * 使用示例（QML）:
 *   import FluentUI 1.0
 *   Rectangle {
 *       color: FluAccentColor.normal
 *   }
 */
class FluAccentColor : public QObject {
    Q_OBJECT

    Q_PROPERTY_AUTO(QColor, darkest)
    Q_PROPERTY_AUTO(QColor, darker)
    Q_PROPERTY_AUTO(QColor, dark)
    Q_PROPERTY_AUTO(QColor, normal)
    Q_PROPERTY_AUTO(QColor, light)
    Q_PROPERTY_AUTO(QColor, lighter)
    Q_PROPERTY_AUTO(QColor, lightest)
    QML_NAMED_ELEMENT(FluAccentColor)
public:
    explicit FluAccentColor(QObject *parent = nullptr);
    // 构造函数：可用于在 C++ 侧初始化默认色值或从主题管理器中读取当前主题的强调色。
};
