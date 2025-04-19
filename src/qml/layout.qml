import QtQuick 2.15
import FluentUI 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CustomTypes 1.0
FluWindow {
    id: root
    width: 500
    height: 400
    visible: true
    title: qsTr("登录")
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    modality: Qt.ApplicationModal  // 阻塞整个应用程序的所有窗口[^9^][^10^]

    // Fluent UI 风格颜色定义
    property color primaryColor: "#0078D4"
    property color textColor: "#323130"
    property color subtleColor: "#605E5C"
    property color borderColor: "#EDEBE9"
    property color errorColor: "#A4262C"


    property UserFullInfo is_login
    property bool is_regist: false

    Rectangle {
        anchors.fill:  parent
        color: "#FAF9F8"  // Fluent UI 背景色

        ColumnLayout {
            anchors.centerIn:  parent
            width: parent.width  * 0.8
            spacing: 20

            // 标题
            Text {
                text: qsTr("欢迎登录")
                font.pixelSize:  24
                font.weight:  Font.Medium
                color: textColor
                Layout.alignment:  Qt.AlignHCenter
                Layout.bottomMargin:  30
            }

            // 用户名输入框
            ColumnLayout {
                spacing: 5
                Layout.fillWidth:  true

                Text {
                    text: qsTr("用户名")
                    font.pixelSize:  14
                    color: textColor
                }

                TextField {
                    id: usernameField
                    placeholderText: qsTr("请输入用户名")
                    Layout.fillWidth:  true
                    font.pixelSize:  14
                    color: textColor
                    background: Rectangle {
                        radius: 2
                        border.color:  usernameField.activeFocus  ? primaryColor : borderColor
                        border.width:  1
                    }
                }
            }

            // 密码输入框
            ColumnLayout {
                spacing: 5
                Layout.fillWidth:  true

                Text {
                    text: qsTr("密码")
                    font.pixelSize:  14
                    color: textColor
                }

                TextField {
                    id: passwordField
                    placeholderText: qsTr("请输入密码")
                    Layout.fillWidth:  true
                    echoMode: TextInput.Password
                    font.pixelSize:  14
                    color: textColor
                    background: Rectangle {
                        radius: 2
                        border.color:  passwordField.activeFocus  ? primaryColor : borderColor
                        border.width:  1
                    }
                }
            }

            // 登录按钮
            Button {
                id: loginButton
                text: qsTr("登录")
                Layout.fillWidth:  true
                Layout.topMargin:  20
                contentItem: Text {
                    text: loginButton.text
                    font.pixelSize:  14
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: 2
                    color: primaryColor
                }

                onClicked: {
                    // 这里添加登录逻辑
                    is_login = musicPlayer.login_user(usernameField.text,passwordField.text)
                    if(is_login.nickname) {
                        showSuccess(qsTr("登录成功"))
                        //----等待一秒
                        Window.window.close()
                    }else {
                        showError(qsTr("登录失败"))
                    }



                }
            }

            Button {
                id: registButton
                text: qsTr("注册")
                Layout.fillWidth:  true
                Layout.topMargin:  20
                contentItem: Text {
                    text: registButton.text
                    font.pixelSize:  14
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: 2
                    color: primaryColor
                }

                onClicked: {
                    // 这里添加登录逻辑
                    is_regist = musicPlayer.regist_user(usernameField.text,passwordField.text)
                    if(is_regist) {
                        showSuccess(qsTr("注册成功"))
                        console.log(" 用户名:", usernameField.text)
                        console.log(" 密码:", passwordField.text)
                    }else{
                        showError(qsTr("注册失败"))
                    }


                }
            }

            // 底部提示文本
            Text {
                text: qsTr("Fmusic_Login_Regist")
                anchors.bottom:parent.bottom
                anchors.bottomMargin:10
                font.pixelSize:  12
                color: subtleColor
                Layout.alignment:  Qt.AlignHCenter
                Layout.topMargin:  30
            }
        }
    }
}