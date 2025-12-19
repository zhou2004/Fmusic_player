/**
* @file Http.h
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

#ifndef Http_H
#define Http_H
#include <iostream>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkCookie>
#include <QTimer>
#include <functional>

class Http : public QObject {
    Q_OBJECT
public:
    /**
     *
     * @brief Http 请求配置结构体
     * 包含请求头、超时时间、baseURL 等配置
     *
     */
    struct RequestConfig {
        QUrl baseUrl;                  // 类似 axios baseURL
        int timeoutMs = 20000;         // 超时时间
        bool withCredentials = false;  // 是否携带 Cookie
        QByteArray contentType = "application/json";
        QList<QPair<QByteArray, QByteArray>> defaultHeaders; // 公共请求头
    };

    explicit Http(QObject *parent = nullptr);

    // 全局单例（可选）
    static Http *instance();

    // 配置后端Url
    void setBaseUrl(const QUrl &url);
    // 设置请求超时时间
    void setTimeout(int ms);
    // 是否携带 Cookie
    void setWithCredentials(bool enabled);
    // 设置默认请求头
    void setDefaultHeader(const QByteArray &name, const QByteArray &value);
    // 设置 Token（自动添加到请求头），这个是公用的 Authorization，对于特殊需求请求头可自行设置
    void setToken(const QString &token, const QByteArray &headerName = "Authorization");

    // 通用 request 接口，此接口不进行封装处理，返回 QNetworkReply*，需要调用者自行处理响应
    QNetworkReply *request(const QString &method,
                           const QString &url,
                           const QJsonObject &data = QJsonObject(),
                           const QMap<QString, QString> &query = {});

    // 语法糖：GET 请求
    QNetworkReply *get(const QString &url,
                       const QMap<QString, QString> &query = {});
    // 语法糖：POST 请求
    QNetworkReply *post(const QString &url,
                        const QJsonObject &data = QJsonObject());
    // 语法糖：PUT 请求
    QNetworkReply *put(const QString &url,
                       const QJsonObject &data = QJsonObject());
    // 语法糖：DEL 请求
    QNetworkReply *del(const QString &url,
                       const QJsonObject &data = QJsonObject());

    // 简单 Promise 风格封装（返回 JSON 数据）
    using JsonCallback = std::function<void(const QJsonObject &json)>;
    using ErrorCallback = std::function<void(const QString &msg, int statusCode)>;

    // GET 请求，返回 JSON 对象
    void getJson(const QString &url,
                 const QMap<QString, QString> &query,
                 const JsonCallback &onSuccess,
                 const ErrorCallback &onError = nullptr);
    // POST 请求，返回 JSON 对象
    void postJson(const QString &url,
                  const QJsonObject &data,
                  const JsonCallback &onSuccess,
                  const ErrorCallback &onError = nullptr);

public:

    //============ Http请求包装结构体 ============

    // 通用 Result 结构，对应 TS 里的 Result
    struct Result {
        int code = 0; // 状态码，0 表示成功，非 0 表示错误
        QString message; // 提示信息
        QJsonValue data;   // data 可以是对象/数组/字符串/数字
    };

    // 通用 ResultTable 结构
    using ResultTable = Result;

    // === axios 风格：返回 Result / ResultTable 的 http 方法 ===
    using ResultCallback = std::function<void(const Result &res)>;

    //============ Http请求通用方法 ============

    /**
     *
     * @brief Http 请求方法通用接口
     *
     * @param method HTTP 方法，如 "GET", "POST""
     * @param url 请求的 URL 地址
     * @param body 请求体数据，作为 JSON 对象发送
     * @param params URL 查询参数，以键值对形式传递
     * @param onSuccess 成功回调，接收 Result 结构
     * @param onError 错误回调，接收错误消息和状态码
     * @return 通过回调处理结果
     */
    void httpRequest(const QString &method,
                     const QString &url,
                     const QJsonObject &body,
                     const QMap<QString, QString> &params,
                     const ResultCallback &onSuccess,
                     const ErrorCallback &onError = nullptr);

    /**
     *
     * @brief Http 请求方法通用接口，返回 ResultTable 结构
     *
     * @param method HTTP 方法，如 "GET", "POST""
     * @param url 请求的 URL 地址
     * @param body 请求体数据，作为 JSON 对象发送
     * @param params URL 查询参数，以键值对形式传递
     * @param onSuccess 成功回调，接收 Result 结构
     * @param onError 错误回调，接收错误消息和状态码
     * @return 通过回调处理结果
     */
    void httpRequestTable(const QString &method,
                          const QString &url,
                          const QJsonObject &body,
                          const QMap<QString, QString> &params,
                          const ResultCallback &onSuccess,
                          const ErrorCallback &onError = nullptr);

    /**
     *
     * @brief requestJson 通用接口,如果你只需要原始 JSON，而不是包装成 Result，可以用这个
     *
     * @param method HTTP 方法，如 "GET", "POST""
     * @param url 请求的 URL 地址
     * @param body 请求体数据，作为 JSON 对象发送
     * @param params URL 查询参数，以键值对形式传递
     * @param onSuccess 成功回调，接收 Result 结构
     * @param onError 错误回调，接收错误消息和状态码
     * @return 通过回调处理结果
     */
    void requestJson(const QString &method,
                     const QString &url,
                     const QJsonObject &body,
                     const QMap<QString, QString> &params,
                     const JsonCallback &onSuccess,
                     const ErrorCallback &onError = nullptr);

signals:
    // TODO: 全局请求/响应信号，可以在这里添加,目前先不实现,留作扩展
    //================== 拦截器接口-类似 axios 拦截器：请求前、响应后 ==================
    // 请求拦截器
    void beforeRequest(QNetworkRequest &request, QString &method, QJsonObject &body);
    // 响应拦截器
    void afterResponse(const QNetworkReply *reply, const QJsonObject &data);
    // 错误拦截器
    void httpError(const QNetworkReply *reply, int statusCode, const QString &message);

private slots:
    // TODO: 核心槽函数，处理请求完成事件，需要处理响应数据和错误，需要完成异步回调和拦截器调用，需要全局同步
    // 核心槽函数，使用QT的Network模块的信号槽机制处理请求完成事件，这个是Http异步处理的关键
    void onFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    RequestConfig m_config;
    QString m_token;
    QByteArray m_tokenHeaderName;

    /**
     *
     * @brief 内部构建请求对象，是比较重要的函数，负责构建完整的 QNetworkRequest
     *
     * @param method HTTP 方法
     * @param url 请求 URL
     * @param query 查询参数
     * @return 网络请求体。
     */
    QNetworkRequest buildRequest(const QString &method, const QString &url,
                                 const QMap<QString, QString> &query);
    /**
     *
     * @brief 请求超时处理函数,如果请求超时则中止请求
     *
     * @param reply 网络回复对象
     * @return 无
     */
    void applyTimeout(QNetworkReply *reply);
};


#endif //Http_H
