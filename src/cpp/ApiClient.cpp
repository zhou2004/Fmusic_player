/**
* @file ApiClient.cpp
 *
 * @brief API接口定义类。
 * 此类定义了与后端服务器交互的所以API接口方法
 *
 * @author zhou2004
 * @date 2025-12-14
 */

#include "ApiClient.h"

ApiClient::ApiClient(QObject *parent)
    : QObject(parent),
      m_http(Http::instance()) {
    m_http->setBaseUrl(QUrl("http://localhost:8080"));    // 对应 axios baseURL
    m_http->setTimeout(20000);
    m_http->setDefaultHeader("Accept", "application/json, text/plain, */*");
    m_http->setDefaultHeader("Content-Type", "application/json");
    m_http->setWithCredentials(false);
    m_http->setToken("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjbGFpbXMiOnsicm9sZSI6IlJPTEVfVVNFUiIsInVzZXJJZCI6MTQ4LCJlbWFpbCI6IjIwNjU1OTQ1ODlAcXEuY29tIiwidXNlcm5hbWUiOiJ6aG91anVuamllIn0sImV4cCI6MTc2NTczMDExNX0.I-2NyV6-OHSjybmlYOvRY13uwpM6okBJOk-atjH5GcU");
}

// === 用户相关 ===

void ApiClient::login(const QJsonObject &data,
                      const ResultCb &onSuccess,
                      const ErrorCb &onError)
{
    // TS: http<Result>('post', '/user/login', { data })
    m_http->httpRequest("POST",
                        "/user/login",
                        /* body  */ data,
                        /* params*/ {},
                        onSuccess,
                        onError);
}

void ApiClient::logout(const ResultCb &onSuccess,
                       const ErrorCb &onError)
{
    // TS: http<Result>('post', '/user/logout')
    m_http->httpRequest("POST",
                        "/user/logout",
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::sendEmailCode(const QString &email,
                              const ResultCb &onSuccess,
                              const ErrorCb &onError)
{
    // TS: http<Result>('get', '/user/sendVerificationCode', { params: { email } })
    QMap<QString, QString> params;
    params["email"] = email;

    m_http->httpRequest("GET",
                        "/user/sendVerificationCode",
                        QJsonObject{},
                        params,
                        onSuccess,
                        onError);
}

void ApiClient::registerUser(const QJsonObject &data,
                             const ResultCb &onSuccess,
                             const ErrorCb &onError)
{
    // TS: http<Result>('post', '/user/register', { data })
    m_http->httpRequest("POST",
                        "/user/register",
                        data,
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::resetPassword(const QJsonObject &data,
                              const ResultCb &onSuccess,
                              const ErrorCb &onError)
{
    // TS: http<Result>('patch', '/user/resetUserPassword', { data })
    m_http->httpRequest("PATCH",
                        "/user/resetUserPassword",
                        data,
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::getUserInfo(const ResultCb &onSuccess,
                            const ErrorCb &onError)
{
    // TS: http<Result>('get', '/user/getUserInfo')
    m_http->httpRequest("GET",
                        "/user/getUserInfo",
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::updateUserInfo(const QJsonObject &data,
                               const ResultCb &onSuccess,
                               const ErrorCb &onError)
{
    // TS: http<Result>('put', '/user/updateUserInfo', { data })
    m_http->httpRequest("PUT",
                        "/user/updateUserInfo",
                        data,
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::deleteUser(const ResultCb &onSuccess,
                           const ErrorCb &onError)
{
    // TS: http<Result>('delete', '/user/deleteAccount')
    m_http->httpRequest("DELETE",
                        "/user/deleteAccount",
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

// === banner / 推荐 ===

void ApiClient::getBanner(const ResultCb &onSuccess,
                          const ErrorCb &onError)
{
    // TS: http<Result>('get', '/banner/getBannerList')
    m_http->httpRequest("GET",
                        "/banner/getBannerList",
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::getRecommendedPlaylists(const ResultCb &onSuccess,
                                        const ErrorCb &onError)
{
    m_http->httpRequest("GET",
                        "/playlist/getRecommendedPlaylists",
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::getRecommendedSongs(const ResultCb &onSuccess,
                                    const ErrorCb &onError)
{
    m_http->httpRequest("GET",
                        "/song/getRecommendedSongs",
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

// === 歌曲 ===

void ApiClient::getAllSongs(const QJsonObject &data,
                            const ResultCb &onSuccess,
                            const ErrorCb &onError)
{
    // TS: http<ResultTable>('post', '/song/getAllSongs', { data })
    m_http->httpRequestTable("POST",
                             "/song/getAllSongs",
                             data,
                             {},
                             onSuccess,
                             onError);
}

void ApiClient::getAllVipSongs(const QJsonObject &data,
                               const ResultCb &onSuccess,
                               const ErrorCb &onError)
{
    m_http->httpRequestTable("POST",
                             "/song/getAllVipSongs",
                             data,
                             {},
                             onSuccess,
                             onError);
}

void ApiClient::getSongDetail(int id,
                              const ResultCb &onSuccess,
                              const ErrorCb &onError)
{
    // TS: http<ResultTable>('get', `/song/getSongDetail/${id}`)
    const QString url = QString("/song/getSongDetail/%1").arg(id);

    m_http->httpRequestTable("GET",
                             url,
                             QJsonObject{},
                             {},
                             onSuccess,
                             onError);
}

// === 歌手 ===

void ApiClient::getAllArtists(const QJsonObject &data,
                              const ResultCb &onSuccess,
                              const ErrorCb &onError)
{
    m_http->httpRequestTable("POST",
                             "/artist/getAllArtists",
                             data,
                             {},
                             onSuccess,
                             onError);
}

void ApiClient::getArtistDetail(int id,
                                const ResultCb &onSuccess,
                                const ErrorCb &onError)
{
    const QString url = QString("/artist/getArtistDetail/%1").arg(id);
    m_http->httpRequest("GET",
                        url,
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

// === 歌单 ===

void ApiClient::getAllPlaylists(const QJsonObject &data,
                                const ResultCb &onSuccess,
                                const ErrorCb &onError)
{
    m_http->httpRequestTable("POST",
                             "/playlist/getAllPlaylists",
                             data,
                             {},
                             onSuccess,
                             onError);
}

void ApiClient::getPlaylistDetail(int id,
                                  const ResultCb &onSuccess,
                                  const ErrorCb &onError)
{
    const QString url = QString("/playlist/getPlaylistDetail/%1").arg(id);
    m_http->httpRequest("GET",
                        url,
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

// === 收藏 ===

void ApiClient::getFavoriteSongs(const QJsonObject &data,
                                 const ResultCb &onSuccess,
                                 const ErrorCb &onError)
{
    m_http->httpRequest("POST",
                        "/favorite/getFavoriteSongs",
                        data,
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::collectSong(int songId,
                            const ResultCb &onSuccess,
                            const ErrorCb &onError)
{
    QMap<QString, QString> params;
    params["songId"] = QString::number(songId);

    m_http->httpRequest("POST",
                        "/favorite/collectSong",
                        QJsonObject{},
                        params,
                        onSuccess,
                        onError);
}

void ApiClient::cancelCollectSong(int songId,
                                  const ResultCb &onSuccess,
                                  const ErrorCb &onError)
{
    QMap<QString, QString> params;
    params["songId"] = QString::number(songId);

    m_http->httpRequest("DELETE",
                        "/favorite/cancelCollectSong",
                        QJsonObject{},
                        params,
                        onSuccess,
                        onError);
}

void ApiClient::getFavoritePlaylists(const QJsonObject &data,
                                     const ResultCb &onSuccess,
                                     const ErrorCb &onError)
{
    m_http->httpRequest("POST",
                        "/favorite/getFavoritePlaylists",
                        data,
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::collectPlaylist(int playlistId,
                                const ResultCb &onSuccess,
                                const ErrorCb &onError)
{
    QMap<QString, QString> params;
    params["playlistId"] = QString::number(playlistId);

    m_http->httpRequest("POST",
                        "/favorite/collectPlaylist",
                        QJsonObject{},
                        params,
                        onSuccess,
                        onError);
}

void ApiClient::cancelCollectPlaylist(int playlistId,
                                      const ResultCb &onSuccess,
                                      const ErrorCb &onError)
{
    QMap<QString, QString> params;
    params["playlistId"] = QString::number(playlistId);

    m_http->httpRequest("DELETE",
                        "/favorite/cancelCollectPlaylist",
                        QJsonObject{},
                        params,
                        onSuccess,
                        onError);
}

// === 评论 ===

void ApiClient::addSongComment(const QJsonObject &data,
                               const ResultCb &onSuccess,
                               const ErrorCb &onError)
{
    m_http->httpRequest("POST",
                        "/comment/addSongComment",
                        data,
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::addPlaylistComment(const QJsonObject &data,
                                   const ResultCb &onSuccess,
                                   const ErrorCb &onError)
{
    m_http->httpRequest("POST",
                        "/comment/addPlaylistComment",
                        data,
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::likeComment(int commentId,
                            const ResultCb &onSuccess,
                            const ErrorCb &onError)
{
    const QString url = QString("/comment/likeComment/%1").arg(commentId);
    m_http->httpRequest("PATCH",
                        url,
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::cancelLikeComment(int commentId,
                                  const ResultCb &onSuccess,
                                  const ErrorCb &onError)
{
    const QString url = QString("/comment/cancelLikeComment/%1").arg(commentId);
    m_http->httpRequest("PATCH",
                        url,
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::deleteComment(int commentId,
                              const ResultCb &onSuccess,
                              const ErrorCb &onError)
{
    const QString url = QString("/comment/deleteComment/%1").arg(commentId);
    m_http->httpRequest("DELETE",
                        url,
                        QJsonObject{},
                        {},
                        onSuccess,
                        onError);
}

// === 反馈 ===

void ApiClient::addFeedback(const QString &content,
                            const ResultCb &onSuccess,
                            const ErrorCb &onError)
{
    // TS: http<Result>('post', '/feedback/addFeedback', { params: data })
    QMap<QString, QString> params;
    params["content"] = content;

    m_http->httpRequest("POST",
                        "/feedback/addFeedback",
                        QJsonObject{},
                        params,
                        onSuccess,
                        onError);
}

// === 订单 / 支付 ===

void ApiClient::createOrder(const QString &orderNo,
                            int userId,
                            int planId,
                            const ResultCb &onSuccess,
                            const ErrorCb &onError)
{
    QJsonObject body;
    body["orderNo"] = orderNo;
    body["userId"]  = userId;
    body["planId"]  = planId;

    m_http->httpRequest("POST",
                        "/order/createOrder",
                        body,
                        {},
                        onSuccess,
                        onError);
}

void ApiClient::payOrder(const QString &orderNo,
                         const ResultCb &onSuccess,
                         const ErrorCb &onError)
{
    QMap<QString, QString> params;
    params["orderNo"] = orderNo;

    m_http->httpRequest("POST",
                        "/pay/alipay/page",
                        QJsonObject{},
                        params,
                        onSuccess,
                        onError);
}

