import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

// 主入口 Fluentui启动器
FluLauncher {
    id: app

    // 组件加载完成后执行，所有组件在此处注册，路由也在此处注册
    Component.onCompleted: {
        // 组件初始化
        FluApp.init(app)

        FluApp.useSystemAppBar = false

        // 核心控件，路由注册：
        FluRouter.routes = {
            "/": "qrc:/qt/qml/FMusic/Page/Main.qml",
        };

        // 跳转路由到 / ，即加载 main.qml
        FluRouter.navigate("/")
    }
}
