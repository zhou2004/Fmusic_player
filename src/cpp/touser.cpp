
#include <QSqlError>

#include "Sql/mysql.h"
#include "Sql//touser.h"

ToUser::ToUser(QObject *parent) : QObject(parent) {
    // 添加 MySQL 数据库连接
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName(DB_HOST);  // 数据库主机地址
    m_db.setPort(DB_PORT);             // 数据库端口
    m_db.setDatabaseName(DB_NAME);  // 数据库名称
    m_db.setUserName(DB_USER);     // 数据库用户名
    m_db.setPassword(DB_PASS);     // 数据库密码

    // 打开数据库连接
    if (m_db.open()) {
        std::cout << "connect to mysql db1 success" << std::endl;
        qDebug() << "connect to mysql db1 success";
    } else {
        qDebug() << "connect to mysql db1 error" ;
    }
}

// 创建用户（数据库自动生成ID）
qint64 ToUser::createUser(const UserCreateInfo &user) {
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
bool ToUser::genericUpdate(qint64 userId, const QString &field, const QVariant &value)
{
    QSqlQuery query(m_db);
    query.prepare(QString("UPDATE user SET %1 = :value WHERE user_id = :userId").arg(field));
    query.bindValue(":value", value);
    query.bindValue(":userId", userId);
    return query.exec();
}

// 各字段单独更新方法
bool ToUser::updatePassword(qint64 userId, const QString &newPassword)
{
    return genericUpdate(userId, "user_password", encryptPassword(newPassword));
}

bool ToUser::updatePhone(qint64 userId, const QString &newPhone)
{
    if (!checkUniqueField("phone", newPhone, userId)) return false;
    return genericUpdate(userId, "phone", newPhone);
}

bool ToUser::updateEmail(qint64 userId, const QString &newEmail)
{
    if (!checkUniqueField("email", newEmail, userId)) return false;
    return genericUpdate(userId, "email", newEmail);
}

bool ToUser::updateNickname(qint64 userId, const QString &newNickname)
{
    if (!checkUniqueField("nickname", newNickname, userId)) return false;
    return genericUpdate(userId, "nickname", newNickname);
}

bool ToUser::updateGender(qint64 userId, int newGender)
{
    if (newGender != 1 && newGender != -1 && newGender != 0) {
        qWarning() << "Invalid gender value:" << newGender;
        return false;
    }
    return genericUpdate(userId, "gender", newGender);
}

bool ToUser::updateAge(qint64 userId, uint newAge)
{
    if (newAge > 120) {
        qWarning() << "Invalid age value:" << newAge;
        return false;
    }
    return genericUpdate(userId, "age", newAge);
}

// 根据用户ID查询完整信息
UserFullInfo ToUser::getUserById(qint64 userId)
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
        userInfo.gender = query.value("gender").toInt();
        userInfo.age = query.value("age").toUInt();
    } else {
        qDebug() << "未找到用户 (ID:" << userId << ")";
    }

    return userInfo;
}

// 根据手机号查询完整信息
UserFullInfo ToUser::getUserByPhone(const QString &phone)
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
        userInfo.gender = query.value("gender").toInt();
        userInfo.age = query.value("age").toUInt();
    } else {
        qDebug() << "未找到用户 (手机号:" << phone << ")";
    }

    return userInfo;
}

// 密码加密
QString ToUser::encryptPassword(const QString &password)
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

// 检查唯一字段是否冲突
bool ToUser::checkUniqueField(const QString &fieldName, const QVariant &value, qint64 excludeUserId)
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




