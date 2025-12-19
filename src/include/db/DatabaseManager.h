#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>

// DatabaseManager: 负责整个应用的 SQLite 连接生命周期
// - 提供全局唯一的 QSqlDatabase 连接
// - 初始化数据库文件和公共配置
class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager &instance();

    // 获取已打开的数据库连接（确保已经 init）
    QSqlDatabase db() const;

    // 显式初始化（可在应用启动时调用一次）
    bool init();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    Q_DISABLE_COPY(DatabaseManager)

    bool m_inited = false;
};

#endif // DATABASEMANAGER_H

