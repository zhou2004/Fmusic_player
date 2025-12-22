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
    Q_INVOKABLE void login(const QJsonObject &data);

    // /** 用户登出 */
    Q_INVOKABLE void logout();

    // /** 发送邮箱验证码 */
    Q_INVOKABLE void sendEmailCode(const QString &email);

    // /** 用户注册 */
    Q_INVOKABLE void registerUser(const QJsonObject &data);

    // /** 重置密码 */
    Q_INVOKABLE void resetPassword(const QJsonObject &data);

    // /** 获取用户信息 */
    Q_INVOKABLE void getUserInfo();

    // /** 更新用户信息 */
    Q_INVOKABLE void updateUserInfo(const QJsonObject &data);

    // /** 注销账号 */
    Q_INVOKABLE void deleteUser();

    // === banner / 推荐 ===
    Q_INVOKABLE void getBanner();

    // === 推荐歌单 ===
    Q_INVOKABLE void getRecommendedPlaylists();
    // === 推荐歌曲 ===
    Q_INVOKABLE void getRecommendedSongs();


    // ================== 歌曲相关 ====================

    // === 获取所有歌曲 ===
    Q_INVOKABLE void getAllSongs(const QJsonObject &data);
    // === 获取所有 VIP 歌曲 ===
    Q_INVOKABLE void getAllVipSongs(const QJsonObject &data);
    // === 获取歌曲详情 ===
    Q_INVOKABLE void getSongDetail(int id);

    // ===================== 歌手相关 ======================

    // === 获取所有歌手 ===
    Q_INVOKABLE void getAllArtists(const QJsonObject &data);
    // === 获取歌手详情 ===
    Q_INVOKABLE void getArtistDetail(int id);

    // ===================== 歌单相关 ======================

    // === 获取所有歌单 ===
    Q_INVOKABLE void getAllPlaylists(const QJsonObject &data);
    // === 获取歌单详情 ===
    Q_INVOKABLE void getPlaylistDetail(int id);

    // ===================== 收藏相关 =======================

    // === 获取喜爱歌曲 ===
    Q_INVOKABLE void getFavoriteSongs(const QJsonObject &data);
    // === 收藏歌曲 ===
    Q_INVOKABLE void collectSong(int songId);
    // === 取消收藏歌单 ===
    Q_INVOKABLE void cancelCollectSong(int songId);
    // === 获取喜爱歌单 ===
    Q_INVOKABLE void getFavoritePlaylists(const QJsonObject &data);
    // === 收藏歌单 ===
    Q_INVOKABLE void collectPlaylist(int playlistId);
    // === 取消收藏歌单 ===
    Q_INVOKABLE void cancelCollectPlaylist(int playlistId);

    // ==================== 评论相关 =========================

    // === 添加歌曲评论 ===
    Q_INVOKABLE void addSongComment(const QJsonObject &data);
    // === 添加歌单评论 ===
    Q_INVOKABLE void addPlaylistComment(const QJsonObject &data);
    // === 点赞评论 ===
    Q_INVOKABLE void likeComment(int commentId);
    // === 取消点赞评论 ===
    Q_INVOKABLE void cancelLikeComment(int commentId);
    // === 删除评论 ===
    Q_INVOKABLE void deleteComment(int commentId);

    // ======================= 反馈 ==================

    // === 添加反馈 ===
    Q_INVOKABLE void addFeedback(const QString &content);

    // ================== 订单 / 支付 ==================

    // === 创建订单 ===
    Q_INVOKABLE void createOrder(const QString &orderNo,
                     int userId,
                     int planId);
    // === 支付订单 ===
    Q_INVOKABLE void payOrder(const QString &orderNo);

    // ================== 第三方其他接口 ==================
    // 辅助函数：发送同步 GET 请求（复用你提供的逻辑）
    QByteArray sendSyncGet(const QUrl& url);

    // --- 主函数：获取 KRC 内容 ---
    Q_INVOKABLE QString getKrcContent(const QString& title, const QString& artist);

signals:
    void loginSuccess(const QJsonObject &data);
    void loginError(const QString &message, int code);

    void logoutSuccess(const QJsonObject &data);
    void logoutError(const QString &message, int code);

    void sendEmailCodeSuccess(const QJsonObject &data);
    void sendEmailCodeError(const QString &message, int code);

    void registerUserSuccess(const QJsonObject &data);
    void registerUserError(const QString &message, int code);

    void resetPasswordSuccess(const QJsonObject &data);
    void resetPasswordError(const QString &message, int code);

    void getUserInfoSuccess(const QJsonObject &data);
    void getUserInfoError(const QString &message, int code);

    void updateUserInfoSuccess(const QJsonObject &data);
    void updateUserInfoError(const QString &message, int code);

    void deleteUserSuccess(const QJsonObject &data);
    void deleteUserError(const QString &message, int code);

    void getBannerSuccess(const QJsonObject &data);
    void getBannerError(const QString &message, int code);

    void getRecommendedPlaylistsSuccess(const QJsonObject &data);
    void getRecommendedPlaylistsError(const QString &message, int code);

    void getRecommendedSongsSuccess(const QJsonObject &data);
    void getRecommendedSongsError(const QString &message, int code);

    void getAllSongsSuccess(const QJsonObject &data);
    void getAllSongsError(const QString &message, int code);

    void getAllVipSongsSuccess(const QJsonObject &data);
    void getAllVipSongsError(const QString &message, int code);

    void getSongDetailSuccess(const QJsonObject &data);
    void getSongDetailError(const QString &message, int code);

    void getAllArtistsSuccess(const QJsonObject &data);
    void getAllArtistsError(const QString &message, int code);

    void getArtistDetailSuccess(const QJsonObject &data);
    void getArtistDetailError(const QString &message, int code);

    void getAllPlaylistsSuccess(const QJsonObject &data);
    void getAllPlaylistsError(const QString &message, int code);

    void getPlaylistDetailSuccess(const QJsonObject &data);
    void getPlaylistDetailError(const QString &message, int code);

    void getFavoriteSongsSuccess(const QJsonObject &data);
    void getFavoriteSongsError(const QString &message, int code);

    void collectSongSuccess(const QJsonObject &data);
    void collectSongError(const QString &message, int code);

    void cancelCollectSongSuccess(const QJsonObject &data);
    void cancelCollectSongError(const QString &message, int code);

    void getFavoritePlaylistsSuccess(const QJsonObject &data);
    void getFavoritePlaylistsError(const QString &message, int code);

    void collectPlaylistSuccess(const QJsonObject &data);
    void collectPlaylistError(const QString &message, int code);

    void cancelCollectPlaylistSuccess(const QJsonObject &data);
    void cancelCollectPlaylistError(const QString &message, int code);

    void addSongCommentSuccess(const QJsonObject &data);
    void addSongCommentError(const QString &message, int code);

    void addPlaylistCommentSuccess(const QJsonObject &data);
    void addPlaylistCommentError(const QString &message, int code);

    void likeCommentSuccess(const QJsonObject &data);
    void likeCommentError(const QString &message, int code);

    void cancelLikeCommentSuccess(const QJsonObject &data);
    void cancelLikeCommentError(const QString &message, int code);

    void deleteCommentSuccess(const QJsonObject &data);
    void deleteCommentError(const QString &message, int code);

    void addFeedbackSuccess(const QJsonObject &data);
    void addFeedbackError(const QString &message, int code);

    void createOrderSuccess(const QJsonObject &data);
    void createOrderError(const QString &message, int code);

    void payOrderSuccess(const QJsonObject &data);
    void payOrderError(const QString &message, int code);


private:
    Http *m_http;
};


#endif //APICLIENT_H
