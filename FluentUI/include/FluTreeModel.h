#pragma once

#include <QObject>
#include <QAbstractTableModel>
#include <QJsonArray>
#include <QVariant>
#include <QtQml/qqml.h>
#include "stdafx.h"


/*
 * FluTreeModel.h
 *
 * 提供树形数据模型及节点类，供 QML 中树视图组件使用：
 * - `FluTreeNode` 表示树上的单个节点，包含数据、深度、子节点和父节点引用，并提供用于遍历/查询的 Q_INVOKABLE 方法；
 * - `FluTreeModel` 继承自 `QAbstractTableModel`，将树结构展开为表格行以便用于 QML 的视图组件，暴露了行/列操作和节点操作接口。
 */

/**
 * FluTreeNode
 *
 * - 封装单个树节点的数据和层级信息
 * - 提供 `data()`, `depth()`, `isExpanded()` 等用于 QML 访问的接口
 */
class FluTreeNode : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap data READ data CONSTANT)
    Q_PROPERTY(int depth READ depth CONSTANT)
    Q_PROPERTY(bool isExpanded READ isExpanded CONSTANT)
    Q_PROPERTY(bool checked READ checked CONSTANT)
public:
    explicit FluTreeNode(QObject *parent = nullptr);

    [[nodiscard]] Q_INVOKABLE int depth() const {
        return _depth;
    };

    [[nodiscard]] Q_INVOKABLE bool isExpanded() const {
        return _isExpanded;
    };

    [[nodiscard]] Q_INVOKABLE QVariantMap data() const {
        return _data;
    };

    [[nodiscard]] Q_INVOKABLE bool hasChildren() const {
        return !_children.isEmpty();
    };

    Q_INVOKABLE bool hasNextNodeByIndex(int index) {
        FluTreeNode *p = this;
        for (int i = 0; i <= _depth - index - 1; i++) {
            p = p->_parent;
        }
        if (p->_parent->_children.indexOf(p) == p->_parent->_children.count() - 1) {
            return false;
        }
        return true;
    }

    [[nodiscard]] Q_INVOKABLE bool checked() const {
        if (!hasChildren()) {
            return _checked;
        }
        for (int i = 0; i <= _children.size() - 1; ++i) {
            auto item = _children.at(i);
            if (!item->checked()) {
                return false;
            }
        }
        return true;
    };

    Q_INVOKABLE bool hideLineFooter() {
        if (_parent) {
            auto childIndex = _parent->_children.indexOf(this);
            if (childIndex == _parent->_children.count() - 1) {
                return true;
            }
            if (_parent->_children.at(childIndex + 1)->hasChildren()) {
                return true;
            }
            return false;
        }
        return false;
    };

    [[nodiscard]] bool isShown() const {
        auto p = _parent;
        while (p) {
            if (!p->_isExpanded) {
                return false;
            }
            p = p->_parent;
        }
        return true;
    }

public:
    QString _title = "";
    int _depth = 0;
    bool _checked = false;
    bool _isExpanded = true;
    QVariantMap _data;
    QList<FluTreeNode *> _children;
    FluTreeNode *_parent = nullptr;
};

/*
 * FluTreeModel
 *
 * - 将树形节点展开为行，供 QML 列表/视图组件使用
 * - 提供插入、删除、展开/折叠、选中、刷新等操作的 Q_INVOKABLE 接口，便于 QML 调用
 */
class FluTreeModel : public QAbstractTableModel {
    Q_OBJECT
    Q_PROPERTY_AUTO(int, dataSourceSize)
    Q_PROPERTY_AUTO(QList<QVariantMap>, columnSource)
    QML_NAMED_ELEMENT(FluTreeModel)
public:
    enum TreeModelRoles { RowModel = 0x0101, ColumnModel = 0x0102 };

    explicit FluTreeModel(QObject *parent = nullptr);

    // 覆盖模型接口：行数、列数、数据与角色名
    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    // QML 可调用接口：行操作、节点访问与展开/折叠等
    Q_INVOKABLE void removeRows(int row, int count);
    Q_INVOKABLE void insertRows(int row, const QList<FluTreeNode *> &data);
    Q_INVOKABLE QObject *getRow(int row);
    Q_INVOKABLE void setRow(int row, QVariantMap data);
    Q_INVOKABLE void setData(QList<FluTreeNode *> data);
    Q_INVOKABLE void setDataSource(QList<QMap<QString, QVariant>> data);
    Q_INVOKABLE void collapse(int row);
    Q_INVOKABLE void expand(int row);
    Q_INVOKABLE FluTreeNode *getNode(int row);
    Q_INVOKABLE void refreshNode(int row);
    Q_INVOKABLE void checkRow(int row, bool checked);
    Q_INVOKABLE bool hitHasChildrenExpanded(int row);
    Q_INVOKABLE void allExpand();
    Q_INVOKABLE void allCollapse();
    Q_INVOKABLE QVariant selectionModel();

private:
    QList<FluTreeNode *> _rows;
    QList<FluTreeNode *> _dataSource;
    FluTreeNode *_root = nullptr;
};
