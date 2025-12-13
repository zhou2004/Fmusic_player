import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI 1.0
import "../settingsPage"

ColumnLayout {
    id: settingsPageContent
    spacing: 10

    // 在这里设置参数
    property int leftMarginSet: 10 //全局左边距
    property string appVersion: "1.0.0" //版本号
    property string appDeviceID: "c8a30833-8e3d-c158-598c-2961b30048f5" //设备ID
    property color linkColor: "#4A3C1D" // 链接颜色
    property color linkColorDark: "#b5a64c" // 深色主题下的链接颜色
    property url feelzhouAvatar: "qrc:/assets/feelzhou.jpg" // 作者头像
    property url feelliaoAvatar: "qrc:/assets/feelliao.jpg" // 作者头像

    FluText {
        id: settingsTitle
        text: qsTr("Settings")
        font.pixelSize: 24
        Layout.fillWidth: true
        font.bold: true
        Layout.leftMargin: leftMarginSet
    }

    FluPivot {
        id: settingsPivot
        font.pixelSize: 20
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: leftMarginSet

        // 播放设置
        FluPivotItem {
            title: qsTr("Play")
            contentItem: FluScrollablePage {
                id: settingsPlayPage
                contentWidth: parent.width

                Column {
                    id: settingsPlayContent
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item {
                        width: parent.width
                        height: 10
                    } // 占位符，用于顶部间距
                }
            }
        }


        // 显示设置
        FluPivotItem {
            title: qsTr("Display")
            contentItem: FluScrollablePage {
                id: settingsDisplayPage
                contentWidth: parent.width

                DisplayPage {
                    id: displayPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        // 关于页面
        FluPivotItem {
            title: qsTr("About")
            contentItem: FluScrollablePage {
                id: settingsAboutPage
                contentWidth: parent.width

                AboutPage {
                    id: aboutPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
