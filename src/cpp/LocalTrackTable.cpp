#include "../include/db/LocalTrackTable.h"
#include "../include/db/DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

LocalTrackTable::LocalTrackTable(QObject *parent)
    : QObject(parent)
{
}

QSqlDatabase LocalTrackTable::db() const
{
    return DatabaseManager::instance().db();
}

bool LocalTrackTable::ensureTable()
{
    if (!DatabaseManager::instance().init())
        return false;

    QSqlDatabase database = db();
    QSqlQuery q(database);
    const char *sql =
        "CREATE TABLE IF NOT EXISTS local_tracks ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"  // 自增主键
        "  file_path TEXT UNIQUE,"                 // 文件路径，唯一
        "  title TEXT,"
        "  artist TEXT,"
        "  album TEXT,"
        "  cover TEXT,"
        "  url TEXT,"
        "  duration INTEGER,"
        "  likeStatus INTEGER,"
        "  lyrics TEXT"
        ")";

    if (!q.exec(QString::fromUtf8(sql))) {
        qWarning() << "LocalTrackTable: create table failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool LocalTrackTable::clearAll()
{
    if (!DatabaseManager::instance().init())
        return false;

    QSqlDatabase database = db();
    QSqlQuery q(database);
    if (!q.exec("DELETE FROM local_tracks")) {
        qWarning() << "LocalTrackTable: clearAll failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool LocalTrackTable::insertOrReplaceBatch(const QVector<TrackModel> &tracks)
{
    if (tracks.isEmpty())
        return true;

    if (!DatabaseManager::instance().init())
        return false;

    QSqlDatabase database = db();
    QSqlQuery q(database);
    q.prepare(
        "INSERT OR REPLACE INTO local_tracks "
        "(file_path, title, artist, album, cover, url, duration, likeStatus, lyrics) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

    bool allOk = true;

    for (const TrackModel &t : tracks) {
        q.bindValue(0, t.id);         // file_path
        q.bindValue(1, t.title);
        q.bindValue(2, t.artist);
        q.bindValue(3, t.album);
        q.bindValue(4, t.cover);
        q.bindValue(5, t.url);
        q.bindValue(6, t.duration);
        q.bindValue(7, t.likeStatus);
        q.bindValue(8, t.lyrics);

        if (!q.exec()) {
            allOk = false;
            qWarning() << "LocalTrackTable: insert failed:"
                       << q.lastError().text()
                       << "file_path:" << t.id;
        }
    }

    return allOk;
}

int LocalTrackTable::count() const
{
    if (!DatabaseManager::instance().init())
        return 0;

    QSqlDatabase database = db();
    QSqlQuery q(database);
    if (!q.exec("SELECT COUNT(*) FROM local_tracks")) {
        qWarning() << "LocalTrackTable: count failed:" << q.lastError().text();
        return 0;
    }
    if (q.next())
        return q.value(0).toInt();
    return 0;
}

QVariantList LocalTrackTable::page(int offset, int limit) const
{
    QVariantList list;
    if (offset < 0 || limit <= 0)
        return list;

    if (!DatabaseManager::instance().init())
        return list;

    QSqlDatabase database = db();
    QSqlQuery q(database);
    q.prepare(
        "SELECT id, file_path, title, artist, album, cover, url, duration, likeStatus, lyrics "
        "FROM local_tracks "
        "ORDER BY title COLLATE NOCASE "
        "LIMIT ? OFFSET ?");
    q.addBindValue(limit);
    q.addBindValue(offset);

    if (!q.exec()) {
        qWarning() << "LocalTrackTable: page query failed:" << q.lastError().text();
        return list;
    }

    while (q.next()) {
        QVariantMap map;
        // 这里可以让 QML 继续用 file_path 当作 id，以兼容你现有逻辑
        const QString filePath = q.value(1).toString(); // file_path
        map["id"]        = filePath;                    // 对外仍然叫 id
        map["title"]     = q.value(2).toString();
        map["artist"]    = q.value(3).toString();
        map["album"]     = q.value(4).toString();
        map["cover"]     = q.value(5).toString();
        map["url"]       = q.value(6).toString();
        map["duration"]  = q.value(7).toInt();
        map["likeStatus"] = q.value(8).toInt();
        map["lyrics"]    = q.value(9).toString();
        list.append(map);
    }

    return list;
}
