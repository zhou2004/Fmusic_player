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

        // 设置窗口的图标
        FluApp.windowIcon = "qrc:/assets/logo.ico"


        // 核心控件，路由注册：
        FluRouter.routes = {
            "/":"qrc:/main/main.qml",
        }

        // 跳转路由到 / ，即加载 main.qml
        FluRouter.navigate("/")
    }
}
