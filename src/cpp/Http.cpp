/**
* @file Http.cpp
 *
 * @brief 实现Http请求封装类，类似于前端的axios,属于基础类。
 *
 * 该类使用 Qt 网络模块实现 HTTP 请求封装，支持 GET/POST/PUT/DELETE 等常用方法。
 * 实现了请求配置（baseURL、超时时间、默认请求头等）和请求拦截器功能。
 * 实现了简单的 Promise 风格回调接口，方便异步处理响应结果
 * 实现了通用的 Result 结构封装，类似前端接口返回的数据格式。
 * 实现了携带 Cookie 的选项，支持跨域请求。
 *
 * @author zhou2004
 * @date 2025-12-14
 */

#include "Http.h"
#include <QUrlQuery>

Http::Http(QObject *parent)
    : QObject(parent),
      manager(new QNetworkAccessManager(this))
{
    // 绑定请求完成信号
    connect(manager, &QNetworkAccessManager::finished,
            this, &Http::onFinished);
}

Http *Http::instance()
{
    static Http s_instance;
    return &s_instance;
}


void Http::setBaseUrl(const QUrl &url)
{
    m_config.baseUrl = url;
}

void Http::setTimeout(int ms)
{
    m_config.timeoutMs = ms;
}

void Http::setWithCredentials(bool enabled)
{
    m_config.withCredentials = enabled;
}

void Http::setDefaultHeader(const QByteArray &name, const QByteArray &value)
{
    // 替换已存在的同名 header
    for (auto &pair : m_config.defaultHeaders) {
        if (pair.first == name) {
            pair.second = value;
            return;
        }
    }
    // 不存在则添加新 header
    m_config.defaultHeaders.append(qMakePair(name, value));
}

void Http::setToken(const QString &token, const QByteArray &headerName)
{
    m_token = token;
    m_tokenHeaderName = headerName;
}

QNetworkRequest Http::buildRequest(const QString &method,
                                   const QString &url,
                                   const QMap<QString, QString> &query)
{
    // 构建完整 URL，将baseUrl和后缀url拼接
    QUrl fullUrl = m_config.baseUrl.isEmpty()
                    ? QUrl(url)
                    : m_config.baseUrl.resolved(QUrl(url));

    // 拼接 query 参数
    if (!query.isEmpty()) {
        QUrlQuery q;
        for (auto it = query.constBegin(); it != query.constEnd(); ++it) {
            q.addQueryItem(it.key(), it.value());
        }
        // 合并已有的 query 参数
        fullUrl.setQuery(q);
    }
    // 构建请求对象
    QNetworkRequest request(fullUrl);

    // Content-Type
    // 设置请求内容类型
    if (!m_config.contentType.isEmpty()) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, m_config.contentType);
    }

    // 默认 headers
    // 设置默认请求头
    for (const auto &pair : m_config.defaultHeaders) {
        request.setRawHeader(pair.first, pair.second);
    }

    // Token
    // 设置认证 Token
    if (!m_token.isEmpty() && !m_tokenHeaderName.isEmpty()) {
        request.setRawHeader(m_tokenHeaderName, m_token.toUtf8());
    }

    // withCredentials: 这里简单示例，实际可以结合 QNetworkCookieJar
    // 设置是否携带 Cookie，根据具体需求实现，默认不携带
    if (m_config.withCredentials) {
        // 例如：开启跨域 Cookie，具体逻辑根据后端而定
    }

    // @TODO: 触发请求拦截器,这里根据具体需求设计拦截器参数
    // 预留给请求拦截器修改
    QString methodMut = method;
    QJsonObject bodyDummy; // 这里只传空 body，真正 body 在 request() 里处理
    emit beforeRequest(request, methodMut, bodyDummy);

    return request;
}

void Http::applyTimeout(QNetworkReply *reply)
{
    if (!reply || m_config.timeoutMs <= 0)
        return;
    // 创建定时器
    QTimer *timer = new QTimer(reply);
    // 设置为单次触发
    timer->setSingleShot(true);
    // 设置超时时间
    timer->setInterval(m_config.timeoutMs);
    // 超时后中止请求
    QObject::connect(timer, &QTimer::timeout, reply, [reply]() {
        if (reply->isRunning()) {
            reply->abort();
        }
    });
    // 开始计时
    timer->start();
}

QNetworkReply *Http::request(const QString &method,
                             const QString &url,
                             const QJsonObject &data,
                             const QMap<QString, QString> &query)
{
    // 统一将 method 转为大写
    const QString methodUpper = method.toUpper();
    // 构建请求对象
    QNetworkRequest request = buildRequest(methodUpper, url, query);

    // 构建请求体，QByteArray 格式
    QByteArray body;
    if (!data.isEmpty()) {
        QJsonDocument doc(data);
        body = doc.toJson(QJsonDocument::Compact);
    }

    // 实例化响应体
    QNetworkReply *reply = nullptr;

    // @TODO: 定义各种 HTTP 方法的请求逻辑，需要QT的 Network 模块支持
    // 根据不同方法发送请求,支持 GET, POST, PUT, DELETE, PATCH 等方法
    if (methodUpper == "GET") {
        reply = manager->get(request);
    } else if (methodUpper == "POST") {
        reply = manager->post(request, body);
    } else if (methodUpper == "PUT") {
        reply = manager->put(request, body);
    } else if (methodUpper == "DELETE") {
        reply = manager->deleteResource(request);
    } else if (methodUpper == "PATCH") {
        reply = manager->sendCustomRequest(request, "PATCH", body);
    } else {
        // 其他自定义方法，这里可以扩展
        reply = manager->sendCustomRequest(request, methodUpper.toUtf8(), body);
    }

    // 请求超时处理,超时则中止请求，防止长时间挂起
    applyTimeout(reply);
    return reply;
}

QNetworkReply *Http::get(const QString &url, const QMap<QString, QString> &query)
{
    return request("GET", url, QJsonObject(), query);
}

QNetworkReply *Http::post(const QString &url, const QJsonObject &data)
{
    return request("POST", url, data, {});
}

QNetworkReply *Http::put(const QString &url, const QJsonObject &data)
{
    return request("PUT", url, data, {});
}

QNetworkReply *Http::del(const QString &url, const QJsonObject &data)
{
    return request("DELETE", url, data, {});
}

void Http::getJson(const QString &url,
                   const QMap<QString, QString> &query,
                   const JsonCallback &onSuccess,
                   const ErrorCallback &onError)
{
    // 进行get请求
    QNetworkReply *reply = get(url, query);

    QObject::connect(reply, &QNetworkReply::finished, reply, [reply, onSuccess, onError]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            if (onError) {
                onError(reply->errorString(), reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
            }
            return;
        }
        // 读取返回的数据字节流
        const QByteArray data = reply->readAll();
        QJsonParseError parseError;
        // 解析 JSON 数据
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            // 错误请求抛出异常
            if (onError) {
                onError(QStringLiteral("JSON parse error: ") + parseError.errorString(),
                        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
            }
            return;
        }
        // 成功回调返回 JSON 对象，调用 onSuccess
        onSuccess(doc.object());
    });
}

void Http::postJson(const QString &url,
                    const QJsonObject &data,
                    const JsonCallback &onSuccess,
                    const ErrorCallback &onError)
{
    // 进行post请求，此过程和 getJson 类似

    QNetworkReply *reply = post(url, data);

    QObject::connect(reply, &QNetworkReply::finished, reply, [reply, onSuccess, onError]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            if (onError) {
                onError(reply->errorString(), reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
            }
            return;
        }

        const QByteArray data = reply->readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            if (onError) {
                onError(QStringLiteral("JSON parse error: ") + parseError.errorString(),
                        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
            }
            return;
        }

        onSuccess(doc.object());
    });
}

void Http::requestJson(const QString &method,
                       const QString &url,
                       const QJsonObject &body,
                       const QMap<QString, QString> &params,
                       const JsonCallback &onSuccess,
                       const ErrorCallback &onError)
{
    QNetworkReply *reply = request(method, url, body, params);

    QObject::connect(reply, &QNetworkReply::finished, reply, [reply, onSuccess, onError]() {
        reply->deleteLater();
        // 获取 HTTP 状态码
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (reply->error() != QNetworkReply::NoError) {
            if (onError) {
                onError(reply->errorString(), statusCode);
            }
            return;
        }

        const QByteArray raw = reply->readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(raw, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            if (onError) {
                onError(QStringLiteral("JSON parse error: ") + parseError.errorString(), statusCode);
            }
            return;
        }

        QJsonObject obj;
        if (doc.isObject()) {
            obj = doc.object();
        } else {
            // 返回的不是对象，包在 data 里返回
            QJsonObject tmp;
            tmp.insert("data", doc.isArray() ? QJsonValue(doc.array()) : QJsonValue());
            obj = tmp;
        }

        if (onSuccess) {
            onSuccess(obj);
        }
    });
}

void Http::httpRequest(const QString &method,
                       const QString &url,
                       const QJsonObject &body,
                       const QMap<QString, QString> &params,
                       const ResultCallback &onSuccess,
                       const ErrorCallback &onError)
{
    requestJson(method, url, body, params,
                [onSuccess](const QJsonObject &obj) {
                    Result res;
                    res.code = obj.value("code").toInt();
                    res.message = obj.value("message").toString();
                    res.data = obj.value("data");

                    if (onSuccess) {
                        onSuccess(res);
                    }
                },
                onError);
}

void Http::httpRequestTable(const QString &method,
                            const QString &url,
                            const QJsonObject &body,
                            const QMap<QString, QString> &params,
                            const ResultCallback &onSuccess,
                            const ErrorCallback &onError)
{
    // 目前和 httpRequest 相同，只是给语义区分开，方便以后如果分页格式不一样再单独处理
    httpRequest(method, url, body, params, onSuccess, onError);
}

void Http::onFinished(QNetworkReply *reply)
{
    if (!reply)
        return;

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    // 注意：这里不要再 readAll 了，避免把数据读光
    // QByteArray data = reply->readAll(); // 删掉

    QJsonObject obj; // 暂时不解析 body，由 requestJson 系列负责解析

    if (reply->error() == QNetworkReply::NoError) {
        emit afterResponse(reply, obj);
    } else {
        emit httpError(reply, statusCode, reply->errorString());
    }

    // 不在这里 deleteLater，让 requestJson 里的 lambda 删
    // 如果你有通过 request() / get() 直接用 reply 的场景，
    // 可以由调用者自己决定何时 deleteLater
}


/* 使用示例：用户登录
auto api = new ApiClient(this);

QJsonObject body;
body["email"] = email;
body["password"] = password;

api->login(body,
           // onSuccess
           [this](const Http::Result &res) {
               if (res.code == 0) {
                   // res.data 里就是后端返回的数据（QJsonValue）
                   QJsonObject dataObj = res.data.toObject();
                   QString token = dataObj.value("token").toString();
                   // 存储 token
                   Http::instance()->setToken("Bearer " + token);
                   // 业务逻辑...
               } else {
                   // 业务错误提示
                   qWarning() << "登录失败:" << res.message;
               }
           },
           // onError
           [this](const QString &msg, int status) {
               qWarning() << "网络错误" << status << msg;
           });

*/