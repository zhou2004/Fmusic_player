 //
 // Created by Q2305 on 2025/4/1.
 //

 #ifndef MYSQL_H
 #define MYSQL_H
#include <QObject>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QCryptographicHash>
#include <QPasswordDigestor>
 // ------------ 数据库配置声明 ------------
 extern const char* DB_HOST;
 extern const char* DB_USER;
 extern const char* DB_PASS;
 extern const char* DB_NAME;
 extern const unsigned int DB_PORT;
 extern const char* USER_TABLE_SQL;

 // ------------ 函数声明 ------------
struct UserCreateInfo {
 Q_GADGET  // 关键！使QML能识别结构体属性
public:
 QString password;
 QString phone;
 QString email;
 QString nickname;
};

struct UserFullInfo {
 Q_GADGET  // 关键！使QML能识别结构体属性
 Q_PROPERTY(qint64 userId MEMBER userId)
 Q_PROPERTY(QString password MEMBER password)
 Q_PROPERTY(QString nickname MEMBER nickname)
 Q_PROPERTY(QString phone MEMBER phone)
 Q_PROPERTY(QString email MEMBER email)
 Q_PROPERTY(int gender MEMBER gender)
 Q_PROPERTY(uint age MEMBER age)
 Q_PROPERTY(QString user_avatar_url MEMBER user_avatar_url)

public:
 qint64 userId;
 QString password;
 QString phone;
 QString email;
 QString nickname;
 int gender = 0;    // 0-未设置
 uint age = 0;
 QString user_avatar_url;
};

class Mysql : public QObject {
  Q_OBJECT
public:

   bool init_db_connection();

 // -----------------------音乐驱动接口
   void queryartTable();

   QList<QJsonObject> query_musicTable();

   QList<QJsonObject> query_musictype(QString typeId);


// -------------------------歌单驱动接口
   QList<QJsonObject> query_Playlist();

   QList<QJsonObject> query_PlaylistDetail(qint64 playlist_id);



 // ------------------------用户驱动接口
 qint64 createUser(const UserCreateInfo &user);
 bool updatePassword(qint64 userId, const QString &newPassword);
 bool updatePhone(qint64 userId, const QString &newPhone);
 bool updateEmail(qint64 userId, const QString &newEmail);
 bool updateNickname(qint64 userId, const QString &newNickname);
 bool updateGender(qint64 userId, int newGender);
 bool updateAge(qint64 userId, uint newAge);

 // 用户信息查询接口
 UserFullInfo getUserById(qint64 userId);
 UserFullInfo getUserByname(QString nickname);
 UserFullInfo getUserByPhone(const QString &phone);

    // ------------------------收藏表


   bool addUserCollections(qint64 userId, qint64 musicId);
   bool delUserCollections(qint64 userId, qint64 musicId);
   QList<QJsonObject> getUserCollections(qint64 userId);


 bool verifyPassword(const QString& inputPassword, const QString& storedHashValue);

   explicit Mysql(QObject *parent = nullptr);
public slots:

private:

 QSqlDatabase m_db;
 // ------------------------用户驱动接口
 bool checkUniqueField(const QString &fieldName, const QVariant &value, qint64 excludeUserId = 0);
 bool genericUpdate(qint64 userId, const QString &field, const QVariant &value);
 QString encryptPassword(const QString &password);







};

 #endif //MYSQL_H
