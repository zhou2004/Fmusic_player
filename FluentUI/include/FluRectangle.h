#pragma once

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QPainter>
#include "stdafx.h"

/*
 * FluRectangle
 *
 * 一个可绘制的矩形 QML 组件（基于 QQuickPaintedItem），支持：
 *  - `color`：填充颜色（QColor）
 *  - `radius`：四角圆角半径列表（QList<int>），可指定四个角的半径，缺省值为 0
 *  - `borderWidth`：描边宽度（qreal），当宽度 >= 1 且颜色有效时会绘制描边
 *  - `borderColor`：描边颜色（QColor）
 *  - `borderStyle`：描边样式（Qt::PenStyle，例如实线、虚线）
 *  - `dashPattern`：自定义虚线模式（QVector<qreal>），在使用自定义虚线时生效
 *
 * QML 元素名：`FluRectangle`。
 *
 * 使用说明（示例）：
 *   FluRectangle {
 *       width: 200; height: 100
 *       color: "#ffffff"
 *       radius: [8,8,8,8]
 *       borderWidth: 2
 *       borderColor: "#000000"
 *   }
 *
 * 实现细节：组件通过构造 QPainterPath 来绘制带独立圆角的矩形，然后使用 fillPath 填充并（可选）使用 strokePath 描边。
 */
class FluRectangle : public QQuickPaintedItem {
    Q_OBJECT

    // 填充颜色
    Q_PROPERTY_AUTO(QColor, color)
    // 四角圆角半径，顺序按实现约定（通常为 top-left, top-right, bottom-right, bottom-left）
    Q_PROPERTY_AUTO(QList<int>, radius)
    // 边框宽度
    Q_PROPERTY_AUTO(qreal, borderWidth)
    // 边框颜色
    Q_PROPERTY_AUTO(QColor, borderColor)
    // 边框样式（实线/虚线等）
    Q_PROPERTY_AUTO(Qt::PenStyle, borderStyle)
    // 虚线模式（仅在使用 DashLine/CustomDashLine 时有效）
    Q_PROPERTY_AUTO(QVector<qreal>, dashPattern)

    QML_NAMED_ELEMENT(FluRectangle)

public:
    // 构造函数：parent 默认为 nullptr
    explicit FluRectangle(QQuickItem *parent = nullptr);

    // 判断当前是否需要绘制边框（根据 borderWidth 与 color 的有效性）
    bool borderValid() const;

    // QQuickPaintedItem 的绘制实现，使用 QPainter 绘制带圆角的矩形
    void paint(QPainter *painter) override;
};
