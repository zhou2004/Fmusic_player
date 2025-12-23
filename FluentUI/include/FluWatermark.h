#pragma once

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QPainter>
#include "stdafx.h"

/*
 * FluWatermark
 *
 * 用于在画面上绘制水印文本的可绘制 QML 组件。
 * 属性说明：
 *  - `text`：要显示的水印文本
 *  - `gap`：水印之间的间距（QPoint）
 *  - `offset`：整体偏移量
 *  - `textColor`：文字颜色
 *  - `rotate`：旋转角度（度）
 *  - `textSize`：文字大小（像素）
 *
 * 在 paint() 中按属性绘制重复的、可旋转的文字图案以形成水印效果。
 */
class FluWatermark : public QQuickPaintedItem {
Q_OBJECT
Q_PROPERTY_AUTO(QString, text)
Q_PROPERTY_AUTO(QPoint, gap)
Q_PROPERTY_AUTO(QPoint, offset);
Q_PROPERTY_AUTO(QColor, textColor);
Q_PROPERTY_AUTO(int, rotate);
Q_PROPERTY_AUTO(int, textSize);
    QML_NAMED_ELEMENT(FluWatermark)
public:
    explicit FluWatermark(QQuickItem *parent = nullptr);

    // 绘制实现：绘制重复排列并可旋转的水印文本
    void paint(QPainter *painter) override;
};
