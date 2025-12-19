#ifndef LOCALTRACKTABLE_H
#define LOCALTRACKTABLE_H

#include <QObject>
#include <QVariantList>
#include <QSqlDatabase>

#include "../control/interface.h"   // TrackModel 定义

// LocalTrackTable: 操作 local_tracks 表
// 负责：
//  - 创建表结构
//  - 插入/替换本地扫描得到的歌曲
//  - 统计总数
//  - 分页查询（返回 QVariantList，字段与 TrackModel / QML 一致）
class LocalTrackTable : public QObject
{
    Q_OBJECT
public:
    explicit LocalTrackTable(QObject *parent = nullptr);

    // 确保表已创建
    bool ensureTable();

    // 清空全部本地歌曲
    bool clearAll();

    // 批量插入一批 TrackModel（用于扫描结果落库）
    bool insertOrReplaceBatch(const QVector<TrackModel> &tracks);

    // 总数
    int count() const;

    // 分页查询（offset 从 0 开始，limit > 0）
    QVariantList page(int offset, int limit) const;

private:
    QSqlDatabase db() const;
};

#endif // LOCALTRACKTABLE_H

