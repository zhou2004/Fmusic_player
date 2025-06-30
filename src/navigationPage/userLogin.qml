import QtQuick 2.15
import FluentUI 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../js/myjs.js" as MyJS
FluWindow {
    id: loginWindow
    width: 300
    height: 340
    visible: true
    title: qsTr("User Login")
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    modality: Qt.ApplicationModal // 阻塞整个应用程序的所有窗口[^9^][^10^]

    property string loginQRCodeLink: "https://example.com/login-qr-code" // 二维码登录的链接

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width * 0.8

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 20 // 占位符，用于顶部标题的布局
        }

        // 顶部标题
        FluText {
            text: qsTr("Login")
            id: loginTitle
            font.pixelSize: 24
            font.weight: Font.Medium
            Layout.alignment: Qt.AlignLeft
            Layout.bottomMargin: 5
        }

        FluPivot {
            Layout.fillWidth: true
            Layout.fillHeight: true
            font.pixelSize: 14
            id: loginPivot

            // 手机号登录界面
            FluPivotItem {
                id: phoneLoginItem
                title: qsTr("Phone")
                contentItem: Column {
                    spacing: 10
                    topPadding: 10

                    // 手机号输入框
                    Row {
                        width: parent.width
                        spacing: 5

                        FluTextBox {
                            id: phoneNumBox
                            placeholderText: qsTr("Phone Number")
                            width: parent.width - phoneVarButton.width - 5
                        }

                        FluFilledButton {
                            id: phoneVarButton
                            text: qsTr("Send")
                            height: phoneNumBox.height
                            width: 60
                            enabled: true // 初始状态为可点击

                            onClicked: {
                                qjson.send_kugou_checkcode(phoneNumBox.text);
                                MyJS.myFunc1();
                                MyJS.myFunc2();

                                // 禁用按钮并启动计时器
                                phoneVarButton.enabled = false;
                                timer.start();
                            }
                        }

                        Timer {
                            id: timer
                            interval: 60000 // 60秒
                            repeat: false
                            onTriggered: {
                                // 60秒后恢复按钮状态
                                phoneVarButton.enabled = true;
                            }
                        }
                    }

                    // 验证码输入框
                    FluPasswordBox {
                        id: passwordBox
                        placeholderText: qsTr("varification Code")
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width
                    }

                    // 登录协议提示
                    FluText {
                        text: qsTr("Logging in means you agree to the privacy policy")
                        font.pixelSize: 10
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 20

                        // 登录按钮
                        FluFilledButton {
                            id: loginButton
                            text: qsTr("Login")
                            onClicked: {
                                console.log(phoneNumBox.text, passwordBox.text);
                                qjson.verify_checkcode(phoneNumBox.text,passwordBox.text);
                                // MyJS.myFunc1();
                                // MyJS.myFunc2();
                            }

                        }

                        // 取消按钮
                        FluButton {
                            id: loginPageCancelButton
                            text: qsTr("Cancel")
                            onClicked: {
                                loginWindow.close()
                            }
                        }
                    }
                }
            }

            // 二维码登录界面
            FluPivotItem {
                id: qrCodeLoginItem
                title: qsTr("QRCode")
                contentItem: Column {
                    spacing: 10
                    topPadding: 10

                    FluQRCode {
                        color: "black"
                        id: loginQRCode
                        text: loginQRCodeLink
                        size: 140
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    FluText {
                        text: qsTr("Scan the QR code to login")
                        font.pixelSize: 13
                        anchors.left: loginQRCode.left
                    }

                    FluButton {
                        text: qsTr("Refresh")
                        anchors.left: loginQRCode.left
                        onClicked: {
                            // 这里可以添加刷新二维码的逻辑
                            console.log("QR Code refreshed")
                        }
                    }
                }
            }
        }
    }
}
