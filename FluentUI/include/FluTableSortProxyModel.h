#pragma once

#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QtQml/qqml.h>
#include <QJSValue>
#include "stdafx.h"

/*
 * FluTableSortProxyModel
 *
 * 基于 QSortFilterProxyModel 的表格代理模型，扩展了可由 QML/JS 提供的比较器与过滤器接口。
 * - `setComparator` 和 `setFilter` 接受 QJSValue（JavaScript 函数），由 C++ 在 `lessThan` / `filterAcceptsRow` 中调用。
 * - 提供对行的访问/修改方法（getRow/setRow/insertRow/removeRow），以便在 QML 中进行更灵活的数据操作。
 */
class FluTableSortProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY_AUTO_P(QVariant, model)
    QML_NAMED_ELEMENT(FluTableSortProxyModel)
public:
    explicit FluTableSortProxyModel(QSortFilterProxyModel *parent = nullptr);

    // 代理模型覆盖：过滤与排序逻辑
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;

    // QML 可调用的行操作接口
    [[maybe_unused]] Q_INVOKABLE QVariant getRow(int rowIndex);
    [[maybe_unused]] Q_INVOKABLE void setRow(int rowIndex, const QVariant &val);
    [[maybe_unused]] Q_INVOKABLE void insertRow(int rowIndex, const QVariant &val);
    [[maybe_unused]] Q_INVOKABLE void removeRow(int rowIndex, int rows);

    // 设置 JS 侧的 comparator/filter 回调
    [[maybe_unused]] Q_INVOKABLE void setComparator(const QJSValue &comparator);
    [[maybe_unused]] Q_INVOKABLE void setFilter(const QJSValue &filter);

private:
    QJSValue _filter;
    QJSValue _comparator;
};
