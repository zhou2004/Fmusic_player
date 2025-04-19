import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
import QtMultimedia
import Qt.labs.platform 1.1
import CustomTypes 1.0
import "../components"

Rectangle {
    id: aboutPage
    width: parent.width
    height: parent.height
    border.color: "black"
    property var username: "未知名"
    z:2

    Column {
        width: parent.width
        height: parent.height

        Rectangle {
            width:170
            height:90
            anchors.horizontalCenter:parent.horizontalCenter
            anchors.top:parent.top
            anchors.topMargin:30
            radius:8
            Text {
                anchors.centerIn:parent
                text: mainWindow.user.nickname ? mainWindow.user.nickname : username
            }
        }



        Rectangle {
            width:170
            height:90
            anchors.horizontalCenter:parent.horizontalCenter
            anchors.bottom:parent.bottom
            anchors.bottomMargin:30
            radius:8
            FluFilledButton {
                width: parent.width * 0.6
                height: parent.height * 0.3
                anchors.horizontalCenter:parent.horizontalCenter
                anchors.top:parent.top
                text: "登录"
                onClicked: {
                    FluRouter.navigate("/layout")
                }

            }
            FluFilledButton {
                width: parent.width * 0.6
                height: parent.height * 0.3
                anchors.horizontalCenter:parent.horizontalCenter
                anchors.bottom: parent.bottom
                text: "退出"
                onClicked: {
                    mainWindow.user.nickname = ""
                    mainWindow.user.password = ""
                    mainWindow.user.user_avatar_url = ""
                }

            }
            // Text {
            //     anchors.centerIn:parent
            //     text: "Hello world2!"
            // }
        }

        
    }

}

