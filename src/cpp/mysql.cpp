//
// Created by Q2305 on 2025/4/1.
//

#include "Sql/mysql.h"

#include <iostream>
#include <ostream>

// ------------ 数据库配置定义 ------------
const char* DB_HOST = "127.0.0.1";
const char* DB_USER = "admin";
const char* DB_PASS = "123456";
const char* DB_NAME = "fmusic";
const unsigned int DB_PORT = 3306;



Mysql::Mysql(QObject *parent) : QObject(parent)
{
    this->init_db_connection();
}

//连接数据库
bool Mysql::init_db_connection()
{
    // 添加 MySQL 数据库连接
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName(DB_HOST);  // 数据库主机地址
    m_db.setPort(DB_PORT);             // 数据库端口
    m_db.setDatabaseName(DB_NAME);  // 数据库名称
    m_db.setUserName(DB_USER);     // 数据库用户名
    m_db.setPassword(DB_PASS);     // 数据库密码

    // 打开数据库连接
    if (m_db.open()) {
        std::cout << "connect to mysql m_db success" << std::endl;
        return true;
    } else {
        qDebug() << "connect to mysql m_db error" ;
        return false;
    }

}


//查询art表的所有数据
void Mysql::queryartTable() {
    QSqlQuery query;
    if (query.exec("SELECT * FROM art")) {
        while (query.next()) {
            std::cout << "音乐 ID：" << query.value("art_id").toString().toStdString()
                     << "音乐名称：" << query.value("art_name").toString().toStdString()
                     << "专辑：" << query.value("album").toString().toStdString()
                    << "专辑url：" << query.value("cover_url").toString().toStdString()<<std::endl;
        }
    } else {
        qDebug() << "Query failed:" << query.lastError().text();
    }

}


QList<QJsonObject> Mysql::query_musicTable()
{
    QSqlQuery query;
    QList<QJsonObject> q_files;
    QJsonObject musicInfo;
    if (query.exec("SELECT * FROM music"))
    {
        while (query.next())
        {
            musicInfo["music_id"] = query.value("music_id").toString();
            musicInfo["music_name"] = query.value("music_name").toString();
            musicInfo["art_name"] = query.value("art_name").toString();
            musicInfo["cover_url"] = query.value("cover_url").toString();
            musicInfo["lyric_url"] = query.value("lyric_url").toString();
            musicInfo["type_name"] = query.value("type_name").toString();
            musicInfo["play_count"] = query.value("play_count").toInt();
            musicInfo["collect_count"] = query.value("collect_count").toInt();
            musicInfo["play_url"] = query.value("play_url").toString();
            musicInfo["album"] = query.value("album").toString();
            musicInfo["duration"] = query.value("duration").toInt();

            // std::cout << "music_name: " << query.value("music_name").toString().toStdString() << std::endl;
            q_files.append(musicInfo);
        }
        return q_files;
    }
    return q_files;
}

QList<QJsonObject> Mysql::query_musictype(QString type_name)
{
    QSqlQuery query;
    QList<QJsonObject> q_files;
    QJsonObject musicInfo;
    if (!query.exec(QString("SELECT * ""FROM music ""WHERE type_name = '%1'").arg(type_name))) {
        std::cout << "查询失败：" << query.lastError().text().toStdString();
        return q_files;
    }
    else {
             while (query.next()) {
                 musicInfo["music_id"] = query.value("music_id").toString();
                 musicInfo["music_name"] = query.value("music_name").toString();
                 musicInfo["art_name"] = query.value("art_name").toString();
                 musicInfo["cover_url"] = query.value("cover_url").toString();
                 musicInfo["lyric_url"] = query.value("lyric_url").toString();
                 musicInfo["type_name"] = query.value("type_name").toString();
                 musicInfo["play_count"] = query.value("play_count").toInt();
                 musicInfo["collect_count"] = query.value("collect_count").toInt();
                 musicInfo["play_url"] = query.value("play_url").toString();
                 musicInfo["album"] = query.value("album").toString();
                 musicInfo["duration"] = query.value("duration").toInt();
                 q_files.append(musicInfo);
             }
            return q_files;
        }
}

QList<QJsonObject> Mysql::query_Playlist() {
    QSqlQuery query;
    QList<QJsonObject> q_files;
    QJsonObject musicInfo;
    if (!query.exec("SELECT * FROM Playlist")) {
        std::cout << "查询失败：" << query.lastError().text().toStdString();
        return q_files;
    } else {
        while (query.next()) {
            musicInfo["playlist_id"] = query.value("playlist_id").toInt();
            musicInfo["playlist_name"] = query.value("playlist_name").toString();
            musicInfo["cover_url"] = query.value("cover_url").toString();
            musicInfo["description"] = query.value("description").toString();
            musicInfo["user_id"] = query.value("user_id").toInt();
            musicInfo["create_time"] = query.value("create_time").toDate().toString();
            musicInfo["tag_id"] = query.value("tag_id").toString();
            q_files.append(musicInfo);
        }

        return q_files;
    }

}


QList<QJsonObject> Mysql::query_PlaylistDetail(qint64 playlist_id) {
    QSqlQuery query;
    QList<QJsonObject> q_files;
    QJsonObject musicInfo;
    if (!query.exec(QString("SELECT m.* "
        "FROM fmusic.music  m "
        "JOIN fmusic.PlaylistDetails pd ON m.music_id  = pd.music_id  "
        "WHERE pd.playlist_id  = '%1'"
        ).arg(playlist_id))) {

        std::cout << "查询失败：" << query.lastError().text().toStdString();
        return q_files;

    } else {
        while (query.next()) {

            musicInfo["music_id"] = query.value("music_id").toString();
            musicInfo["music_name"] = query.value("music_name").toString();
            musicInfo["art_name"] = query.value("art_name").toString();
            musicInfo["cover_url"] = query.value("cover_url").toString();
            musicInfo["lyric_url"] = query.value("lyric_url").toString();
            musicInfo["type_name"] = query.value("type_name").toString();
            musicInfo["play_count"] = query.value("play_count").toInt();
            musicInfo["collect_count"] = query.value("collect_count").toInt();
            musicInfo["play_url"] = query.value("play_url").toString();
            musicInfo["album"] = query.value("album").toString();
            musicInfo["duration"] = query.value("duration").toInt();
            q_files.append(musicInfo);
        }
        return q_files;

    }
}




//---------------------------------------------------------------------------------



// 创建用户（数据库自动生成ID）
qint64 Mysql::createUser(const UserCreateInfo &user) {
    if (!checkUniqueField("phone", user.phone) ||
        !checkUniqueField("email", user.email) ||
        !checkUniqueField("nickname", user.nickname)) {
        return -1;
        }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO user ("
        "  user_password, phone, email, nickname"
        ") VALUES ("
        "  :password, :phone, :email, :nickname"
        ")");

    QString encryptedPassword = encryptPassword(user.password);
    query.bindValue(":password", encryptedPassword);
    query.bindValue(":phone", user.phone);
    query.bindValue(":email", user.email);
    query.bindValue(":nickname", user.nickname);

    qDebug() << "执行 SQL:" << query.lastQuery();
    qDebug() << "绑定值:" << query.boundValues();

    if (!query.exec()) {
        qCritical() << "插入用户失败:" << query.lastError().text();
        return -1;
    }

    qint64 newUserId = query.lastInsertId().toLongLong();
    if (newUserId == -1) {
        qCritical() << "获取用户ID失败";
        return -1;
    }

    qInfo() << "用户创建成功，ID:" << newUserId;
    return newUserId;
}


// 通用更新方法
bool Mysql::genericUpdate(qint64 userId, const QString &field, const QVariant &value)
{
    QSqlQuery query(m_db);
    query.prepare(QString("UPDATE user SET %1 = :value WHERE user_id = :userId").arg(field));
    query.bindValue(":value", value);
    query.bindValue(":userId", userId);
    return query.exec();
}

// 各字段单独更新方法
bool Mysql::updatePassword(qint64 userId, const QString &newPassword)
{
    return genericUpdate(userId, "user_password", encryptPassword(newPassword));
}

bool Mysql::updatePhone(qint64 userId, const QString &newPhone)
{
    if (!checkUniqueField("phone", newPhone, userId)) return false;
    return genericUpdate(userId, "phone", newPhone);
}

bool Mysql::updateEmail(qint64 userId, const QString &newEmail)
{
    if (!checkUniqueField("email", newEmail, userId)) return false;
    return genericUpdate(userId, "email", newEmail);
}

bool Mysql::updateNickname(qint64 userId, const QString &newNickname)
{
    if (!checkUniqueField("nickname", newNickname, userId)) return false;
    return genericUpdate(userId, "nickname", newNickname);
}

bool Mysql::updateGender(qint64 userId, int newGender)
{
    if (newGender != 1 && newGender != -1 && newGender != 0) {
        qWarning() << "Invalid gender value:" << newGender;
        return false;
    }
    return genericUpdate(userId, "gender", newGender);
}

bool Mysql::updateAge(qint64 userId, uint newAge)
{
    if (newAge > 120) {
        qWarning() << "Invalid age value:" << newAge;
        return false;
    }
    return genericUpdate(userId, "age", newAge);
}

// 根据用户ID查询完整信息
UserFullInfo Mysql::getUserById(qint64 userId)
{
    UserFullInfo userInfo;
    userInfo.userId = -1; // 默认无效ID

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM user WHERE user_id = :userId");
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qWarning() << "查询用户失败 (ID:" << userId << "):" << query.lastError().text();
        return userInfo;
    }

    if (query.next()) {
        std::cout << query.value("nickname").toString().toStdString() <<std::endl;
        userInfo.userId = query.value("user_id").toLongLong();
        userInfo.password = query.value("user_password").toString();
        userInfo.phone = query.value("phone").toString();
        userInfo.email = query.value("email").toString();
        userInfo.nickname = query.value("nickname").toString();
        userInfo.user_avatar_url = query.value("user_avatar_url").toString();
        userInfo.gender = query.value("gender").toInt();
        userInfo.age = query.value("age").toUInt();
    } else {
        qDebug() << "未找到用户 (ID:" << userId << ")";
    }

    return userInfo;
}


UserFullInfo Mysql::getUserByname(QString nickname)
{
    UserFullInfo userInfo;
    userInfo.userId = -1; // 默认无效ID

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM user WHERE nickname = :nickname");
    query.bindValue(":nickname", nickname);

    if (!query.exec()) {
        qWarning() << "查询用户失败 (ID:" << nickname << "):" << query.lastError().text();
        return userInfo;
    }

    if (query.next()) {
        std::cout << query.value("nickname").toString().toStdString() <<std::endl;
        userInfo.userId = query.value("user_id").toLongLong();
        userInfo.password = query.value("user_password").toString();
        userInfo.phone = query.value("phone").toString();
        userInfo.email = query.value("email").toString();
        userInfo.nickname = query.value("nickname").toString();
        userInfo.user_avatar_url = query.value("user_avatar_url").toString();
        userInfo.gender = query.value("gender").toInt();
        userInfo.age = query.value("age").toUInt();
    } else {
        qDebug() << "未找到用户 (ID:" << nickname << ")";
    }

    return userInfo;
}

// 根据手机号查询完整信息
UserFullInfo Mysql::getUserByPhone(const QString &phone)
{
    UserFullInfo userInfo;
    userInfo.userId = -1; // 默认无效ID

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM user WHERE phone = :phone");
    query.bindValue(":phone", phone);

    if (!query.exec()) {
        qWarning() << "查询用户失败 (手机号:" << phone << "):" << query.lastError().text();
        return userInfo;
    }

    if (query.next()) {
        userInfo.userId = query.value("user_id").toLongLong();
        userInfo.password = query.value("user_password").toString();
        userInfo.phone = query.value("phone").toString();
        userInfo.email = query.value("email").toString();
        userInfo.nickname = query.value("nickname").toString();
        userInfo.user_avatar_url = query.value("user_avatar_url").toString();
        userInfo.gender = query.value("gender").toInt();
        userInfo.age = query.value("age").toUInt();
    } else {
        qDebug() << "未找到用户 (手机号:" << phone << ")";
    }

    return userInfo;
}

// 密码加密
QString Mysql::encryptPassword(const QString &password)
{
    if (password.isEmpty()) {
        qWarning() << "密码不能为空";
        return QString();
    }



    return QString(QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
    ).toHex());
}

bool Mysql::verifyPassword(const QString& inputPassword, const QString& storedHashValue) {
    QByteArray inputHashValue = QCryptographicHash::hash(inputPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
    if (inputHashValue == storedHashValue) {
        return true;
        // 密码正确
    } else {
        return false;
        // 密码错误
    }
}

// 检查唯一字段是否冲突
bool Mysql::checkUniqueField(const QString &fieldName, const QVariant &value, qint64 excludeUserId)
{
    if (value.isNull()) return true;

    QSqlQuery query(m_db);
    query.prepare(
        QString("SELECT COUNT(*) FROM user WHERE %1 = :value AND user_id != :excludeId")
            .arg(fieldName));
    query.bindValue(":value", value);
    query.bindValue(":excludeId", excludeUserId);

    if (!query.exec()) {
        qWarning() << "Check unique field failed:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        return query.value(0).toInt() == 0;
    }

    return false;
}



QList<QJsonObject> Mysql::getUserCollections(qint64 userId) {
    QSqlQuery query(m_db);
    QList<QJsonObject> q_files;
    QJsonObject musicInfo;
    if (!query.exec(QString("SELECT * "
        "FROM music  m "
        "JOIN UserCollections uc ON m.music_id  = uc.music_id  "
        "WHERE uc.user_id  = '%1'"
        "ORDER BY uc.collection_time  DESC"
        ).arg(userId))) {

        std::cout << "查询失败：" << query.lastError().text().toStdString();

        return q_files;
    } else {
        while (query.next()) {
            musicInfo["music_id"] = query.value("music_id").toString();
            musicInfo["music_name"] = query.value("music_name").toString();
            musicInfo["art_name"] = query.value("art_name").toString();
            musicInfo["cover_url"] = query.value("cover_url").toString();
            musicInfo["lyric_url"] = query.value("lyric_url").toString();
            musicInfo["type_name"] = query.value("type_name").toString();
            musicInfo["play_count"] = query.value("play_count").toInt();
            musicInfo["collect_count"] = query.value("collect_count").toInt();
            musicInfo["play_url"] = query.value("play_url").toString();
            musicInfo["album"] = query.value("album").toString();
            musicInfo["duration"] = query.value("duration").toInt();
            q_files.append(musicInfo);
        }
        return q_files;
    }
}

bool Mysql::addUserCollections(qint64 userId, qint64 musicId) {
    QSqlQuery query(m_db);
    if (!query.exec(QString("insert into UserCollections (user_id, music_id) "
        "values ('%1', '%2')"
        ).arg(userId).arg(musicId))) {

        std::cout << "添加失败：" << query.lastError().text().toStdString();

        return false;
    } else {
        return true;
    }
}

bool Mysql::delUserCollections(qint64 userId, qint64 musicId) {
    QSqlQuery query(m_db);
    if (!query.exec(QString("delete from UserCollections "
        "where user_id = '%1' and music_id = '%2'"
        ).arg(userId).arg(musicId))) {

        std::cout << "删除失败：" << query.lastError().text().toStdString();

        return false;
        } else {
            return true;
        }
}

