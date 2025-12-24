/**
* @file ApiClient.h
 *
 * @brief API接口定义类。
 * 此类定义了与后端服务器交互的所以API接口方法
 *
 * @author zhou2004
 * @date 2025-12-14
 */

#ifndef APICLIENT_H
#define APICLIENT_H

#include <QUrlQuery>

#include "Http.h"
#include "KRCDecryptor.h"


// 你也可以拆成多个类：UserApi, SongApi, PlaylistApi 等
class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    using Result       = Http::Result;
    using ResultTable  = Http::ResultTable;
    using ResultCb     = Http::ResultCallback;
    using ErrorCb      = Http::ErrorCallback;

    // ===================== 用户相关 ===================

    // /** 用户登录 */
    void login(const QJsonObject &data,
               const ResultCb &onSuccess,
               const ErrorCb &onError = nullptr);

    // /** 用户登出 */
    void logout(const ResultCb &onSuccess,
                const ErrorCb &onError = nullptr);

    // /** 发送邮箱验证码 */
    void sendEmailCode(const QString &email,
                       const ResultCb &onSuccess,
                       const ErrorCb &onError = nullptr);

    // /** 用户注册 */
    void registerUser(const QJsonObject &data,
                      const ResultCb &onSuccess,
                      const ErrorCb &onError = nullptr);

    // /** 重置密码 */
    void resetPassword(const QJsonObject &data,
                       const ResultCb &onSuccess,
                       const ErrorCb &onError = nullptr);

    // /** 获取用户信息 */
    void getUserInfo(const ResultCb &onSuccess,
                     const ErrorCb &onError = nullptr);

    // /** 更新用户信息 */
    void updateUserInfo(const QJsonObject &data,
                        const ResultCb &onSuccess,
                        const ErrorCb &onError = nullptr);

    // /** 注销账号 */
    void deleteUser(const ResultCb &onSuccess,
                    const ErrorCb &onError = nullptr);

    // === banner / 推荐 ===
    void getBanner(const ResultCb &onSuccess,
                   const ErrorCb &onError = nullptr);
    // === 推荐歌单 ===
    void getRecommendedPlaylists(const ResultCb &onSuccess,
                                 const ErrorCb &onError = nullptr);
    // === 推荐歌曲 ===
    void getRecommendedSongs(const ResultCb &onSuccess,
                             const ErrorCb &onError = nullptr);


    // ================== 歌曲相关 ====================

    // === 获取所有歌曲 ===
    void getAllSongs(const QJsonObject &data,
                     const ResultCb &onSuccess,
                     const ErrorCb &onError = nullptr);
    // === 获取所有 VIP 歌曲 ===
    void getAllVipSongs(const QJsonObject &data,
                        const ResultCb &onSuccess,
                        const ErrorCb &onError = nullptr);
    // === 获取歌曲详情 ===
    void getSongDetail(int id,
                       const ResultCb &onSuccess,
                       const ErrorCb &onError = nullptr);

    // ===================== 歌手相关 ======================

    // === 获取所有歌手 ===
    void getAllArtists(const QJsonObject &data,
                       const ResultCb &onSuccess,
                       const ErrorCb &onError = nullptr);
    // === 获取歌手详情 ===
    void getArtistDetail(int id,
                         const ResultCb &onSuccess,
                         const ErrorCb &onError = nullptr);

    // ===================== 歌单相关 ======================

    // === 获取所有歌单 ===
    void getAllPlaylists(const QJsonObject &data,
                         const ResultCb &onSuccess,
                         const ErrorCb &onError = nullptr);
    // === 获取歌单详情 ===
    void getPlaylistDetail(int id,
                           const ResultCb &onSuccess,
                           const ErrorCb &onError = nullptr);

    // ===================== 收藏相关 =======================

    // === 获取喜爱歌曲 ===
    void getFavoriteSongs(const QJsonObject &data,
                          const ResultCb &onSuccess,
                          const ErrorCb &onError = nullptr);
    // === 收藏歌曲 ===
    void collectSong(int songId,
                     const ResultCb &onSuccess,
                     const ErrorCb &onError = nullptr);
    // === 取消收藏歌单 ===
    void cancelCollectSong(int songId,
                           const ResultCb &onSuccess,
                           const ErrorCb &onError = nullptr);
    // === 获取喜爱歌单 ===
    void getFavoritePlaylists(const QJsonObject &data,
                              const ResultCb &onSuccess,
                              const ErrorCb &onError = nullptr);
    // === 收藏歌单 ===
    void collectPlaylist(int playlistId,
                         const ResultCb &onSuccess,
                         const ErrorCb &onError = nullptr);
    // === 取消收藏歌单 ===
    void cancelCollectPlaylist(int playlistId,
                               const ResultCb &onSuccess,
                               const ErrorCb &onError = nullptr);

    // ==================== 评论相关 =========================

    // === 添加歌曲评论 ===
    void addSongComment(const QJsonObject &data,
                        const ResultCb &onSuccess,
                        const ErrorCb &onError = nullptr);
    // === 添加歌单评论 ===
    void addPlaylistComment(const QJsonObject &data,
                            const ResultCb &onSuccess,
                            const ErrorCb &onError = nullptr);
    // === 点赞评论 ===
    void likeComment(int commentId,
                     const ResultCb &onSuccess,
                     const ErrorCb &onError = nullptr);
    // === 取消点赞评论 ===
    void cancelLikeComment(int commentId,
                           const ResultCb &onSuccess,
                           const ErrorCb &onError = nullptr);
    // === 删除评论 ===
    void deleteComment(int commentId,
                       const ResultCb &onSuccess,
                       const ErrorCb &onError = nullptr);

    // ======================= 反馈 ==================

    // === 添加反馈 ===
    void addFeedback(const QString &content,
                     const ResultCb &onSuccess,
                     const ErrorCb &onError = nullptr);

    // ================== 订单 / 支付 ==================

    // === 创建订单 ===
    void createOrder(const QString &orderNo,
                     int userId,
                     int planId,
                     const ResultCb &onSuccess,
                     const ErrorCb &onError = nullptr);
    // === 支付订单 ===
    void payOrder(const QString &orderNo,
                  const ResultCb &onSuccess,
                  const ErrorCb &onError = nullptr);

    // ================== 第三方其他接口 ==================
    // 辅助函数：发送同步 GET 请求（复用你提供的逻辑）
    QByteArray sendSyncGet(const QUrl& url) {
        // 伪装成浏览器或常用客户端，防止被拦截
        QNetworkReply *reply = m_http->get(url.toString());
        if (!reply) return QByteArray();

        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec(); // 阻塞等待

        QByteArray data;
        if (reply->error() == QNetworkReply::NoError) {
            data = reply->readAll();
        } else {
            qWarning() << "Network Error:" << reply->errorString() << "URL:" << url.toString();
        }
        reply->deleteLater();
        return data;
    }

    // --- 主函数：获取 KRC 内容 ---
    Q_INVOKABLE QString getKrcContent(const QString& title, const QString& artist) {
        // ==========================================
        // 步骤 1: 搜索获取 Hash
        // ==========================================
        QUrl searchUrl("http://ioscdn.kugou.com/api/v3/search/song");
        QUrlQuery searchQuery;
        searchQuery.addQueryItem("keyword", artist + " " + title); // 组合搜索词
        searchQuery.addQueryItem("page", "1");
        searchQuery.addQueryItem("pagesize", "1");
        searchQuery.addQueryItem("showtype", "10");
        searchQuery.addQueryItem("plat", "2");
        searchQuery.addQueryItem("version", "7910");
        searchQuery.addQueryItem("tag", "1");
        searchQuery.addQueryItem("correct", "1");
        searchQuery.addQueryItem("privilege", "1");
        searchQuery.addQueryItem("sver", "5");
        searchUrl.setQuery(searchQuery);

        QByteArray searchData = sendSyncGet(searchUrl);
        if (searchData.isEmpty()) return QString();

        // 解析 Hash
        QString fileHash;
        QJsonDocument searchDoc = QJsonDocument::fromJson(searchData);
        QJsonObject searchRoot = searchDoc.object();
        if (searchRoot.contains("data") && searchRoot["data"].isObject()) {
            QJsonArray infoArr = searchRoot["data"].toObject()["info"].toArray();
            if (!infoArr.isEmpty()) {
                fileHash = infoArr.first().toObject()["hash"].toString();
            }
        }

        if (fileHash.isEmpty()) {
            qWarning() << "Step 1 Failed: No hash found for" << title;
            return QString();
        }
        // qDebug() << "Step 1 Success. Hash:" << fileHash;

        // ==========================================
        // 步骤 2: 获取 ID 和 AccessKey
        // ==========================================
        QUrl keyUrl("http://krcs.kugou.com/search");
        QUrlQuery keyQuery;
        keyQuery.addQueryItem("ver", "1");
        keyQuery.addQueryItem("man", "no");
        keyQuery.addQueryItem("client", "pc");
        keyQuery.addQueryItem("hash", fileHash);
        keyUrl.setQuery(keyQuery);

        QByteArray keyData = sendSyncGet(keyUrl);
        if (keyData.isEmpty()) return QString();

        // 解析 ID 和 AccessKey
        QString krcId;
        QString accessKey;
        QJsonDocument keyDoc = QJsonDocument::fromJson(keyData);
        QJsonObject keyRoot = keyDoc.object();

        // 注意：krcs.kugou.com 的返回结构通常在 candidates 数组里
        if (keyRoot.contains("candidates") && keyRoot["candidates"].isArray()) {
            QJsonArray candidates = keyRoot["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject bestMatch = candidates.first().toObject();
                krcId = bestMatch["id"].toString();
                accessKey = bestMatch["accesskey"].toString();
            }
        }

        if (krcId.isEmpty() || accessKey.isEmpty()) {
            qWarning() << "Step 2 Failed: ID or AccessKey not found.";
            return QString();
        }
        // qDebug() << "Step 2 Success. ID:" << krcId << "Key:" << accessKey;

        // ==========================================
        // 步骤 3: 下载 KRC Content
        // ==========================================
        QUrl downloadUrl("http://lyrics2.kugou.com/download");
        QUrlQuery downloadQuery;
        downloadQuery.addQueryItem("ver", "1");
        downloadQuery.addQueryItem("client", "pc");
        downloadQuery.addQueryItem("id", krcId);
        downloadQuery.addQueryItem("accesskey", accessKey);
        downloadQuery.addQueryItem("fmt", "krc");
        downloadQuery.addQueryItem("charset", "utf8");
        downloadUrl.setQuery(downloadQuery);

        QByteArray downloadData = sendSyncGet(downloadUrl);
        if (downloadData.isEmpty()) return QString();

        // 解析 Content
        QString contentBase64;
        QJsonDocument downloadDoc = QJsonDocument::fromJson(downloadData);
        QJsonObject downloadRoot = downloadDoc.object();

        if (downloadRoot.contains("content")) {
            contentBase64 = downloadRoot["content"].toString();
        }

        if (contentBase64.isEmpty()) {
            qWarning() << "Step 3 Failed: Content is empty.";
            return QString();
        }

        QString krc_data = KRCDecoder::parseKrcFromBase64(contentBase64);
        std::cout << krc_data.toStdString() << std::endl;
        // qDebug().noquote() << "Step 3 Success. KRC Data :" << krc_data;

        return krc_data;
    }

private:
    Http *m_http;
};


#endif //APICLIENT_H
