#include "FluRectangle.h"
#include <QPainterPath>

/*
 * FluRectangle
 *
 * 基于 QQuickPaintedItem 的可绘制矩形组件，支持：
 *  - 四角不同的圆角半径
 *  - 填充颜色
 *  - 可选边框（支持实线、虚线、自定义 dash pattern）
 *
 * 该实现通过构造 QPainterPath 来绘制带圆角的矩形路径，然后填充并（可选）描边。
 */

FluRectangle::FluRectangle(QQuickItem *parent) : QQuickPaintedItem(parent) {
    // 默认属性初始化
    color(Qt::white);                 // 默认填充白色
    radius({0, 0, 0, 0});             // 每个角的半径（顺序按类定义）
    borderWidth(0);                   // 默认无边框
    borderColor(Qt::black);           // 边框颜色默认黑色
    borderStyle(Qt::SolidLine);       // 边框默认实线
    dashPattern({});                  // 虚线模式默认空

    // 属性变化时触发重绘
    connect(this, &FluRectangle::colorChanged, this, [=] { update(); });
    connect(this, &FluRectangle::radiusChanged, this, [=] { update(); });
    connect(this, &FluRectangle::borderWidthChanged, this, [=] { update(); });
    connect(this, &FluRectangle::borderColorChanged, this, [=] { update(); });
    connect(this, &FluRectangle::borderStyleChanged, this, [=] { update(); });
    connect(this, &FluRectangle::dashPatternChanged, this, [=] { update(); });
}

/*
 * borderValid
 *
 * 判断是否需要绘制边框：
 * - 边框宽度四舍五入后 >= 1
 * - 颜色有效且不完全透明
 */
bool FluRectangle::borderValid() const {
    return qRound(_borderWidth) >= 1 && _color.isValid() && _color.alpha() > 0;
}

/*
 * paint
 *
 * 主绘制方法：
 * - 计算绘制矩形（如果需要描边则内缩半个边框宽度，避免描边被裁切）
 * - 构造带四角圆弧的 QPainterPath
 * - 使用 fillPath 填充颜色
 * - 若需要描边则使用 strokePath 绘制边框（支持 dash pattern）
 */
void FluRectangle::paint(QPainter *painter) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 初始绘制区域为项的包围盒
    QRectF rect = boundingRect();

    // 判断是否绘制边框，并在需要时对绘制区域进行内缩，保证描边在项内部
    bool drawBorder = borderValid();
    if (drawBorder) {
        qreal halfBorderWidth = _borderWidth / 2.0;
        rect.adjust(halfBorderWidth, halfBorderWidth, -halfBorderWidth, -halfBorderWidth);
    }

    QPainterPath path;
    QList<int> r = _radius;

    // 确保有四个半径值（缺省补 0）
    while (r.size() < 4) {
        r.append(0);
    }

    // 构造路径（从右下开始，顺时针或按实现的顺序）
    path.moveTo(rect.bottomRight() - QPointF(0, r[2]));
    path.lineTo(rect.topRight() + QPointF(0, r[1]));
    path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(r[1] * 2, 0)), QSize(r[1] * 2, r[1] * 2)), 0, 90);

    path.lineTo(rect.topLeft() + QPointF(r[0], 0));
    path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(r[0] * 2, r[0] * 2)), 90, 90);

    path.lineTo(rect.bottomLeft() - QPointF(0, r[3]));
    path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, r[3] * 2)), QSize(r[3] * 2, r[3] * 2)), 180, 90);

    path.lineTo(rect.bottomRight() - QPointF(r[2], 0));
    path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(r[2] * 2, r[2] * 2)), QSize(r[2] * 2, r[2] * 2)), 270, 90);

    // 填充内部
    painter->fillPath(path, _color);

    // 如需描边，设置 QPen 并描边，支持自定义 dash pattern
    if (drawBorder) {
        QPen pen(_borderColor, _borderWidth, _borderStyle);
        if (_borderStyle == Qt::DashLine || _borderStyle == Qt::CustomDashLine) {
            pen.setDashPattern(_dashPattern);
        }
        painter->strokePath(path, pen);
    }

    painter->restore();
}
