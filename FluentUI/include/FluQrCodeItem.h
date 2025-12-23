#pragma once

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QPainter>
#include "stdafx.h"

/*
 * FluQrCodeItem
 *
 * QML 可用的二维码绘制组件：根据 `text` 内容生成并绘制二维码。
 * 属性说明：
 *  - `text`：二维码内容字符串
 *  - `color`：二维码前景色
 *  - `bgColor`：背景色
 *  - `size`：目标大小（像素）
 */
class FluQrCodeItem : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY_AUTO(QString, text)
    Q_PROPERTY_AUTO(QColor, color)
    Q_PROPERTY_AUTO(QColor, bgColor)
    Q_PROPERTY_AUTO(int, size)
    QML_NAMED_ELEMENT(FluQrCodeItem)
public:
    explicit FluQrCodeItem(QQuickItem *parent = nullptr);

    // 绘制二维码的实现
    void paint(QPainter *painter) override;
};
