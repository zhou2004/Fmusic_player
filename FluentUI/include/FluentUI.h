#pragma once

#include <QObject>
#include <QQmlEngine>

/**
 * @brief FluentUI
 *
 * 该类提供 FluentUI QML 模块在 C++ 端的安装/初始化辅助函数。
 * 一般在 Qt Quick 应用程序中用于：
 *  - 在 C++ 中将 QML 类型注册到 `QQmlEngine` 或 `qmlRegisterType` / `qmlRegisterSingletonType` 等；
 *  - 对 `QQmlEngine` 做额外的初始化（如添加上下文属性、注册类型、设置插件路径等）；
 *  - 暴露模块的版本信息与模块标识符（URI）。
 *
 * 说明：
 *  - `registerTypes(QQmlEngine*)`：带有导出符号（`Q_DECL_EXPORT`），可在作为插件时被外部调用以注册类型到指定引擎。
 *  - `registerTypes(const char *uri)`：基于模块 URI 注册类型（用于在构建时或静态注册的场景）。
 *  - `initializeEngine(QQmlEngine*, const char*)`：在 `QQmlEngine` 创建后进行必要的初始化；第二个参数为模块 URI，可能未使用。
 *
 * 使用范例：
 *   // 运行时直接注册到引擎
 *   FluentUI::registerTypes(engine);
 *
 *   // 或基于 URI 注册（例如在插件初始化回调中）
 *   FluentUI::registerTypes("FluentUI");
 *
 * 注意：本头文件仅声明接口，具体实现位于对应的源文件中（例如 FluentUI.cpp 或插件实现文件）。
 */
class FluentUI {

public:

   /**
    * 将 FluentUI 模块的 QML 类型和单例注册到指定的 `QQmlEngine`。
    *
    * - 带有 `Q_DECL_EXPORT`，表示该函数会被导出（对动态插件特别有用）。
    * - 参数 `engine`：目标 `QQmlEngine`，不能为 nullptr（调用方负责传入有效引擎）。
    */
   static Q_DECL_EXPORT void registerTypes(QQmlEngine *engine);

   /**
    * 基于模块 URI 注册类型。
    *
    * - 参数 `uri`：QML 模块标识，例如 "FluentUI" 或 "com.example.FluentUI"。
    * - 此函数常用于在模块被静态链接或以不同方式加载时进行类型注册。
    */
   static void registerTypes(const char *uri);

   /**
    * 对 `QQmlEngine` 执行额外初始化（例如注入上下文属性、安装翻译、设置插件路径等）。
    *
    * - 参数 `engine`：要初始化的 `QQmlEngine`。
    * - 参数 `uri`：模块 URI，可标记为 [[maybe_unused]] 表示实现可能不使用该参数。
    */
   static void initializeEngine(QQmlEngine *engine, [[maybe_unused]] const char *uri);

private:
   // 模块的主版本号
   static const int _major = 1;
   // 模块的次版本号
   static const int _minor = 0;
   // 模块的 URI（标识字符串），实现文件负责初始化该指针
   static const char *_uri;
};
