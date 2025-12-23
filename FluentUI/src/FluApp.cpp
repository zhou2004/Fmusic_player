#include "FluApp.h"

#include <QGuiApplication>
#include <QQuickItem>
#include <QTimer>
#include <QUuid>
#include <QFontDatabase>
#include <QClipboard>
#include <QTranslator>
#include <utility>
#include "FluentIconDef.h"

FluApp::FluApp(QObject *parent) : QObject{parent} {
    _useSystemAppBar = false;
}

FluApp::~FluApp() = default;

void FluApp::init(QObject *launcher, QLocale locale) {
    // 保存传入的 launcher 对象（通常为顶层窗口或启动器），并保存 locale
    this->launcher(launcher);
    _locale = std::move(locale);

    // 获取与 launcher 关联的 QQmlEngine（供后续在 C++ 中访问/触发 QML 侧的重译等）
    _engine = qmlEngine(launcher);

    // 创建并安装翻译器，用于加载模块的本地化文件
    _translator = new QTranslator(this);
    QGuiApplication::installTranslator(_translator);

    // 根据传入的区域设置尝试加载对应的翻译文件，加载成功后触发引擎重译
    const QStringList uiLanguages = _locale.uiLanguages();
    for (const QString &name : uiLanguages) {
        const QString baseName = "fluentui_" + QLocale(name).name();
        if (_translator->load(":/qt/qml/FluentUI/i18n/" + baseName)) {
            // 重新翻译 QML 引擎中的文本
            _engine->retranslate();
            break;
        }
    }
}

[[maybe_unused]] QJsonArray FluApp::iconData(const QString &keyword, bool caseSensitive) {
    // 返回符合 keyword 筛选的图标描述数组，元素为 { name: <枚举名>, icon: <整型值> }
    QJsonArray arr;

    // 通过元对象反射获取 FluentIcons::Type 枚举信息
    QMetaEnum enumType = FluentIcons::staticMetaObject.enumerator(
        FluentIcons::staticMetaObject.indexOfEnumerator("Type"));

    // 遍历枚举值并根据关键字过滤
    for (int i = 0; i <= enumType.keyCount() - 1; ++i) {
        QString name = enumType.key(i);
        int icon = enumType.value(i);
        if (keyword.isEmpty() || name.contains(keyword, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive)) {
            QJsonObject obj;
            obj.insert("name", name);
            obj.insert("icon", icon);
            arr.append(obj);
        }
    }

    return arr;
}
