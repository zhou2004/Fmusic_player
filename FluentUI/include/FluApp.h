#pragma once

#include <QObject>
#include <QWindow>
#include <QtQml/qqml.h>
#include <QQmlContext>
#include <QJsonObject>
#include <QQmlEngine>
#include <QTranslator>
#include <QQuickWindow>
#include <QJsonArray>
#include "stdafx.h"
#include "singleton.h"

/**
 * @brief FluApp
 *
 * 应用层级的单例辅助类，用于在 QML/引擎中提供全局应用相关状态与服务。
 *
 * 主要职责：
 *  - 保存并提供当前 `QQmlEngine`（用于在运行时访问/设置引擎相关属性）；
 *  - 暴露应用范围的属性（例如 `useSystemAppBar`、`windowIcon`、`locale`、`launcher`）；
 *  - 提供初始化入口 `init()`，用于在应用/引擎启动时传入必要的上下文对象；
 *  - 提供工具型的 Q_INVOKABLE 接口 `iconData()` 给 QML 查询图标数据。
 *
 * QML 暴露：
 *  - 使用 `QML_NAMED_ELEMENT(FluApp)` 将该类型命名为 QML 元素 `FluApp`；
 *  - 使用 `QML_SINGLETON` 标记，让 QML 可以通过 `FluApp` 单例访问（配合 C++ 单例实现）。
 *
 * 生命周期与单例：
 *  - 类通过 `SINGLETON(FluApp)` 宏提供 `FluApp::getInstance()`，返回进程级单例指针；
 *  - 构造函数为私有，防止外部直接创建，推荐通过 QML 的单例工厂或 `create()` 方法访问。
 */
class FluApp : public QObject {
    Q_OBJECT

    // 属性宏：自动生成 Q_PROPERTY、getter、setter 与 change 信号
    Q_PROPERTY_AUTO(bool, useSystemAppBar)
    Q_PROPERTY_AUTO(QString, windowIcon)
    Q_PROPERTY_AUTO(QLocale, locale)
    // 按值传递的对象指针属性（P 表示按值/裸指针版本）
    Q_PROPERTY_AUTO_P(QObject *, launcher)

    // 将此类作为 QML 元素并声明为单例类型（需在模块注册时配套注册）
    QML_NAMED_ELEMENT(FluApp)
    QML_SINGLETON

private:
    // 私有构造/析构：单例模式，禁止外部直接 new/delete
    explicit FluApp(QObject *parent = nullptr);
    ~FluApp() override;

public:
    // 将单例访问器注入类中：提供 static FluApp *getInstance()
    SINGLETON(FluApp)

    /**
     * QML 单例工厂函数。
     *
     * 在 QML 中以单例方式注册时，Qt 会调用此签名的工厂函数来创建/返回实例。
     * 这里直接返回已有单例，保证 QML 与 C++ 共享同一实例。
     */
    static FluApp *create(QQmlEngine *, QJSEngine *) {
        return getInstance();
    }

    /**
     * 初始化方法：
     *
     * - `launcher`：通常为顶层窗口或负责启动的对象，FluApp 会保存该引用以便在需要时访问应用级窗口或上下文。
     * - `locale`：应为当前应用的区域设置，默认为 `QLocale::system()`。
     *
     * 该方法应在 QML 引擎创建并准备好后调用，通常由模块注册或应用入口处触发。
     */
    Q_INVOKABLE void init(QObject *launcher, QLocale locale = QLocale::system());

    /**
     * QML 可调用的静态工具接口：返回符合 `keyword` 的图标数据数组。
     *
     * - `keyword`：筛选关键字，空字符串表示返回全部。
     * - `caseSensitive`：是否区分大小写。
     *
     * 标记为 `[[maybe_unused]]` 以避免在某些构建配置下未使用警告。
     */
    [[maybe_unused]] Q_INVOKABLE static QJsonArray iconData(const QString &keyword = "", bool caseSensitive = true);

private:
    // 保存注册时提供的 QML 引擎指针（非拥有）
    QQmlEngine *_engine{};
    // 翻译器实例（如有需要在 init 时创建并安装）
    QTranslator *_translator = nullptr;
};
