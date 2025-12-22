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
    m_http->setToken(
        "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjbGFpbXMiOnsicm9sZSI6IlJPTEVfVVNFUiIsInVzZXJJZCI6MTQ4LCJlbWFpbCI6IjIwNjU1OTQ1ODlAcXEuY29tIiwidXNlcm5hbWUiOiJ6aG91anVuamllIn0sImV4cCI6MTc2Njc2MzgwMH0.fCfV6_9HJp-7TkqIgVXNZ5lZV5KwAKm656gBx3bOsy0"
        );
}

// === 用户相关 ===

void ApiClient::login(const QJsonObject &data)
{
    // 调用底层的 Http 封装
    m_http->httpRequestTable("POST", "/user/login", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit loginSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit loginError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit loginError(msg, code);
        }
    );
}

void ApiClient::logout()
{
    m_http->httpRequest("POST", "/user/logout", QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit logoutSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit logoutError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit logoutError(msg, code);
        }
    );
}

void ApiClient::sendEmailCode(const QString &email)
{
    // TS: http<Result>('get', '/user/sendVerificationCode', { params: { email } })
    QMap<QString, QString> params;
    params["email"] = email;

    m_http->httpRequest("GET", "/user/sendVerificationCode", QJsonObject{}, params,
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit sendEmailCodeSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit sendEmailCodeError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit sendEmailCodeError(msg, code);
        }
    );
}

void ApiClient::registerUser(const QJsonObject &data)
{

    m_http->httpRequest("POST", "/user/register", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit registerUserSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit registerUserError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit registerUserError(msg, code);
        }
    );
}

void ApiClient::resetPassword(const QJsonObject &data)
{
    m_http->httpRequest("PATCH", "/user/resetUserPassword", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit resetPasswordSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit resetPasswordError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit resetPasswordError(msg, code);
        }
    );
}

void ApiClient::getUserInfo()
{
    m_http->httpRequest("GET", "/user/getUserInfo", QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getUserInfoSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getUserInfoError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getUserInfoError(msg, code);
        }
    );
}

void ApiClient::updateUserInfo(const QJsonObject &data)
{
    m_http->httpRequest("PUT", "/user/updateUserInfo", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit updateUserInfoSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit updateUserInfoError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit updateUserInfoError(msg, code);
        }
    );
}

void ApiClient::deleteUser()
{
    m_http->httpRequest("DELETE", "/user/deleteAccount", QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit deleteUserSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit deleteUserError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit deleteUserError(msg, code);
        }
    );
}

// === banner / 推荐 ===

void ApiClient::getBanner()
{
    m_http->httpRequest("GET", "/banner/getBannerList", QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getBannerSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getBannerError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getBannerError(msg, code);
        }
    );
}

void ApiClient::getRecommendedPlaylists()
{
    m_http->httpRequest("GET", "/playlist/getRecommendedPlaylists", QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getRecommendedPlaylistsSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getRecommendedPlaylistsError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getRecommendedPlaylistsError(msg, code);
        }
    );
}

void ApiClient::getRecommendedSongs()
{
    m_http->httpRequest("GET", "/song/getRecommendedSongs", QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getRecommendedSongsSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getRecommendedSongsError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getRecommendedSongsError(msg, code);
        }
    );
}

// === 歌曲 ===
void ApiClient::getAllSongs(const QJsonObject &data) {
    // 调用底层的 Http 封装
    m_http->httpRequestTable("POST", "/song/getAllSongs", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getAllSongsSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getAllSongsError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getAllSongsError(msg, code);
        }
    );
}

void ApiClient::getAllVipSongs(const QJsonObject &data)
{
    m_http->httpRequestTable("POST", "/song/getAllVipSongs", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getAllVipSongsSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getAllVipSongsError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getAllVipSongsError(msg, code);
        }
    );
}

void ApiClient::getSongDetail(int id)
{
    const QString url = QString("/song/getSongDetail/%1").arg(id);

    m_http->httpRequestTable("GET", url, QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getSongDetailSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getSongDetailError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getSongDetailError(msg, code);
        }
    );
}

// === 歌手 ===

void ApiClient::getAllArtists(const QJsonObject &data)
{

    m_http->httpRequestTable("POST", "/artist/getAllArtists", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getAllArtistsSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getAllArtistsError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getAllArtistsError(msg, code);
        }
    );
}

void ApiClient::getArtistDetail(int id)
{
    const QString url = QString("/artist/getArtistDetail/%1").arg(id);

    m_http->httpRequest("GET", url, QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getArtistDetailSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getArtistDetailError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getArtistDetailError(msg, code);
        }
    );
}

// === 歌单 ===

void ApiClient::getAllPlaylists(const QJsonObject &data)
{

    m_http->httpRequestTable("POST", "/playlist/getAllPlaylists", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getAllPlaylistsSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getAllPlaylistsError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getAllPlaylistsError(msg, code);
        }
    );
}

void ApiClient::getPlaylistDetail(int id)
{
    const QString url = QString("/playlist/getPlaylistDetail/%1").arg(id);

    m_http->httpRequest("GET", url, QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getPlaylistDetailSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getPlaylistDetailError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getPlaylistDetailError(msg, code);
        }
    );
}

// === 收藏 ===

void ApiClient::getFavoriteSongs(const QJsonObject &data)
{

    m_http->httpRequest("POST", "/favorite/getFavoriteSongs", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getFavoriteSongsSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getFavoriteSongsError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getFavoriteSongsError(msg, code);
        }
    );
}

void ApiClient::collectSong(int songId)
{
    QMap<QString, QString> params;
    params["songId"] = QString::number(songId);

    m_http->httpRequest("POST", "/favorite/collectSong", QJsonObject{}, params,
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit collectSongSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit collectSongError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit collectSongError(msg, code);
        }
    );
}

void ApiClient::cancelCollectSong(int songId)
{
    QMap<QString, QString> params;
    params["songId"] = QString::number(songId);

    m_http->httpRequest("DELETE", "/favorite/cancelCollectSong", QJsonObject{}, params,
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit cancelCollectSongSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit cancelCollectSongError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit cancelCollectSongError(msg, code);
        }
    );
}

void ApiClient::getFavoritePlaylists(const QJsonObject &data)
{

    m_http->httpRequest("POST", "/favorite/getFavoritePlaylists", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit getFavoritePlaylistsSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit getFavoritePlaylistsError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit getFavoritePlaylistsError(msg, code);
        }
    );
}

void ApiClient::collectPlaylist(int playlistId)
{
    QMap<QString, QString> params;
    params["playlistId"] = QString::number(playlistId);

    m_http->httpRequest("POST", "/favorite/collectPlaylist", QJsonObject{}, params,
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit collectPlaylistSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit collectPlaylistError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit collectPlaylistError(msg, code);
        }
    );
}

void ApiClient::cancelCollectPlaylist(int playlistId)
{
    QMap<QString, QString> params;
    params["playlistId"] = QString::number(playlistId);

    m_http->httpRequest("DELETE", "/favorite/cancelCollectPlaylist", QJsonObject{}, params,
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit cancelCollectPlaylistSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit cancelCollectPlaylistError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit cancelCollectPlaylistError(msg, code);
        }
    );
}

// === 评论 ===

void ApiClient::addSongComment(const QJsonObject &data)
{

    m_http->httpRequest("POST", "/comment/addSongComment", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit addSongCommentSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit addSongCommentError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit addSongCommentError(msg, code);
        }
    );
}

void ApiClient::addPlaylistComment(const QJsonObject &data)
{

    m_http->httpRequest("POST", "/comment/addPlaylistComment", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit addPlaylistCommentSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit addPlaylistCommentError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit addPlaylistCommentError(msg, code);
        }
    );
}

void ApiClient::likeComment(int commentId)
{
    const QString url = QString("/comment/likeComment/%1").arg(commentId);

    m_http->httpRequest("PATCH", url, QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit likeCommentSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit likeCommentError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit likeCommentError(msg, code);
        }
    );
}

void ApiClient::cancelLikeComment(int commentId)
{
    const QString url = QString("/comment/cancelLikeComment/%1").arg(commentId);

    m_http->httpRequest("PATCH", url, QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit cancelLikeCommentSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit cancelLikeCommentError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit cancelLikeCommentError(msg, code);
        }
    );
}

void ApiClient::deleteComment(int commentId)
{
    const QString url = QString("/comment/deleteComment/%1").arg(commentId);

    m_http->httpRequest("DELETE", url, QJsonObject{}, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit deleteCommentSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit deleteCommentError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit deleteCommentError(msg, code);
        }
    );
}

// === 反馈 ===

void ApiClient::addFeedback(const QString &content)
{
    QMap<QString, QString> params;
    params["content"] = content;

    m_http->httpRequest("POST", "/feedback/addFeedback", QJsonObject{}, params,
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit addFeedbackSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit addFeedbackError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit addFeedbackError(msg, code);
        }
    );
}

// === 订单 / 支付 ===

void ApiClient::createOrder(const QString &orderNo,
                            int userId,
                            int planId)
{
    QJsonObject data;
    data["orderNo"] = orderNo;
    data["userId"]  = userId;
    data["planId"]  = planId;

    m_http->httpRequest("POST", "/order/createOrder", data, {},
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit createOrderSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit createOrderError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit createOrderError(msg, code);
        }
    );
}

void ApiClient::payOrder(const QString &orderNo)
{
    QMap<QString, QString> params;
    params["orderNo"] = orderNo;

    m_http->httpRequest("POST", "/pay/alipay/page", QJsonObject{}, params,
        // 成功回调 -> 发射成功信号
        [this](const Http::Result &res) {
            if (res.code == 0) {
                // 将数据转为 QJsonObject 发送
                QJsonObject Res;
                Res["code"] = 0;

                // 兼容处理：如果是数组则转为数组，否则转为对象
                if (res.data.isArray()) {
                    Res["data"] = res.data.toArray();
                } else {
                    Res["data"] = res.data.toObject();
                }

                emit payOrderSuccess(Res);
            } else {
                // 业务错误 -> 发射失败信号
                emit payOrderError(res.message, res.code);
            }
        },
        // 网络错误回调 -> 发射失败信号
        [this](const QString &msg, int code) {
            emit payOrderError(msg, code);
        }
    );
}

QByteArray ApiClient::sendSyncGet(const QUrl& url) {
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

QString ApiClient::getKrcContent(const QString& title, const QString& artist) {
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