#ifndef TOUSER_H
#define TOUSER_H

#include <QObject>
#include <QSqlDatabase>
#include <iostream>
#include <QSqlQuery>
#include <QVariant>
#include <QCryptographicHash>
#include "Sql//mysql.h"




class ToUser : public QObject {
    Q_OBJECT
public:



    explicit ToUser(QObject *parent = nullptr);

    // 用户操作接口
    qint64 createUser(const UserCreateInfo &user);
    bool updatePassword(qint64 userId, const QString &newPassword);
    bool updatePhone(qint64 userId, const QString &newPhone);
    bool updateEmail(qint64 userId, const QString &newEmail);
    bool updateNickname(qint64 userId, const QString &newNickname);
    bool updateGender(qint64 userId, int newGender);
    bool updateAge(qint64 userId, uint newAge);

    // 查询接口
    UserFullInfo getUserById(qint64 userId);
    UserFullInfo getUserByPhone(const QString &phone);

private:
    QSqlDatabase m_db;

    bool checkUniqueField(const QString &fieldName, const QVariant &value, qint64 excludeUserId = 0);
    bool genericUpdate(qint64 userId, const QString &field, const QVariant &value);
    QString encryptPassword(const QString &password);
};

#endif // TOUSER_H