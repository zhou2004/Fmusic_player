#pragma once

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QPainter>
#include "stdafx.h"

/*
 * FluCaptcha
 *
 * 图形验证码组件：基于 QQuickPaintedItem 绘制验证码图像，支持噪点、干扰线、大小和是否忽略大小写等配置。
 * 方法：`refresh()` 生成新的验证码，`verify(code)` 验证传入的字符串是否与当前验证码匹配。
 */
class FluCaptcha : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY_AUTO(QFont, font)
    Q_PROPERTY_AUTO(bool, ignoreCase)
    Q_PROPERTY_AUTO(int, noiseCount)
    Q_PROPERTY_AUTO(qreal, noiseRadius)
    Q_PROPERTY_AUTO(int, lineCount)
    Q_PROPERTY_AUTO(qreal, lineWidthMin)
    Q_PROPERTY_AUTO(qreal, lineWidthMax)

    QML_NAMED_ELEMENT(FluCaptcha)

public:
    explicit FluCaptcha(QQuickItem *parent = nullptr);

    // 绘制验证码图像
    void paint(QPainter *painter) override;

    // 生成新的验证码
    Q_INVOKABLE void refresh();

    // 校验输入的验证码字符串
    [[maybe_unused]] Q_INVOKABLE bool verify(const QString &code);

private:
    QString _code;
};
