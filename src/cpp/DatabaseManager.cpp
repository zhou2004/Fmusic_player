#include "../include/db/DatabaseManager.h"

#include <QStandardPaths>
#include <QDir>
#include <QSqlError>
#include <QDebug>

static const char *kDefaultConnectionName = "fmusic_default";

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

QSqlDatabase DatabaseManager::db() const
{
    return QSqlDatabase::database(kDefaultConnectionName);
}

bool DatabaseManager::init()
{
    if (m_inited)
        return true;

    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/FMusic";
    if (dataDir.isEmpty()) {
        qWarning() << "DatabaseManager: AppLocalDataLocation is empty";
        return false;
    }
    QDir().mkpath(dataDir);
    const QString dbPath = dataDir + "/fmusic.db";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", kDefaultConnectionName);
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qWarning() << "DatabaseManager: open sqlite failed:" << db.lastError().text();
        return false;
    }

    m_inited = true;
    qDebug() << "DatabaseManager: initialized at" << dbPath;
    return true;
}

