#ifndef FLUTABLEMODEL_H
#define FLUTABLEMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QtQml/qqml.h>
#include "stdafx.h"

/*
 * FluTableModel
 *
 * 基于 QAbstractTableModel 的通用表格模型，暴露给 QML 用于显示表格数据。
 * - `columnSource` 定义列的信息（例如列名、键名等）
 * - `rows` 保存实际的数据行（QVariantMap 列表）
 *
 * 提供常用的行操作接口（getRow/setRow/insertRow/removeRow/appendRow）以便在 QML 中操作表格数据。
 */
class FluTableModel : public QAbstractTableModel {
    Q_OBJECT
    Q_PROPERTY_AUTO(QList<QVariantMap>, columnSource)
    Q_PROPERTY_AUTO(QList<QVariantMap>, rows)
    Q_PROPERTY(int rowCount READ rowCount CONSTANT)
    QML_NAMED_ELEMENT(FluTableModel)
public:
    enum TableModelRoles { RowModel = 0x0101, ColumnModel = 0x0102 };

    explicit FluTableModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    // 模型操作接口
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariant getRow(int rowIndex);
    Q_INVOKABLE void setRow(int rowIndex, QVariant row);
    Q_INVOKABLE void insertRow(int rowIndex, QVariant row);
    Q_INVOKABLE void removeRow(int rowIndex, int rows = 1);
    Q_INVOKABLE void appendRow(QVariant row);
};


#endif // FLUTABLEMODEL_H
