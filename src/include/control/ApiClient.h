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

#include "Http.h"

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

private:
    Http *m_http;
};


#endif //APICLIENT_H
