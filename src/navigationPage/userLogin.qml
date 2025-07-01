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

    property string loginQRCodeLink: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAacAAAGnAQMAAAApIQrbAAAABlBMVEUAAAD///+l2Z/dAAAAAnRSTlP//8i138cAAAAJcEhZcwAACxIAAAsSAdLdfvwAAAOiSURBVHic7Zw9kqNADIXlInDIETgKRzNH4ygcwSGBCy399KRuz9Qmk6lKHez04v5IeOjfFv3LkqKSUZvYUt0X1c9D7yu2kZfq+/nBp4ue08WTU1H5qLU97ctEck7t311mKOA+/H62zWtvm36mqHzUcT/uSygJiESkHcYNT3m0+5C6hTQXlZpa9Y03/KXHfMZ9iL9556KyU22jsPOGy2p2/r4W+ikqKUU7jytCt34GPpmd//IFRaWjTAbQBl/8/25sFZWPirX1913fNAV+Qzscq6hsFMy7xItvTpx2Ht78loRbgLbmojJSOGM5Ncx7i9kEV25vrqdpY6FazOMXlY6KgHxvaZe5dW5g3vHOH4Kiip0pKh3V1kltDBFaq5hZLYWp2dOu0AIUlYy6H7cwHpsND5E0CzCZnUfpDMU0ROZFJaPUy6Ew722t7T8X/DvV0mQj1EZRCanmoAfzTgtwGh6lM7ENDL4WlY5CZZsNLA/IzQKYSPAR1KLqdy4qIaVMslwbiNAePIPGJf27N7mKykdJnJn149rAmbYQkP/2DkWlojYWTJBtqR/2KuhsQwi4s4zeoahc1GrPnREazPvhL774Wn7kX0Ulo8QjNKET74ebWixEjytQTlHZKMuytXc0okA6XNm9LioRsxWVifKCSU+yhjYl6+ESZ9ajqJzUId6/mNUD8q6WiYmYq2WcSykqC9ULZd7SQgmFl2Zln9p9AUxBUdkovO9xGAXSnQhKZ96n5nzvEYoqKhGlnPPkwKdy2aw+iypsbXAKRYtKRyEyj9ZzNKMlsi134h8bKBryr6LSUEiuhTOBrgzrTvqLj1g9cPMORaWiXAk6DBh0867KZofN6kdXuqhclM8I4fGPSZZy42MJk7c/tKh01MaA/Gfl5PLWhpg2aAo80isqFWUtSAhg6EpbPbwtdrK8Pe2KKioV5SUUtp6lj4DKMLzNHgc710Vlo9y8C5x4BOQfy7u7wf/9PY6iElH34/ax3gjI7cONA97mCyZrcs1FJaQ2Rt29YrZJrGX4nQr8XamNonJRsdrhIf9Sz8jku1/p+VdRmShXglqzw9y6/17B4sMn0r//zvyrqFQUXmpWTrzZ4R3Mbhx6B7OolNQhXQkxhGCCWfqv0IhfGXrZRSWkZBzrHaf3X6yHu0iKykutfcAAYNtZOVQ4YsS8u6iEVPt7fZXBvU+9Uy24s/g8f1EJKZND9+b2HQ11tdjMibuAPntQVCbqL6uoXNQ/5RArkCMQmXgAAAAASUVORK5CYII=" // 二维码登录的链接
    // property string loginQRCodeLink: "zkk" // 二维码登录的链接
    property var qrmsg: ""

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

                    Image {
                        id: loginQRCode
                        width: 140
                        height: 140
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: qrmsg.qrcode_img // 替换为你的Base64编码的图片数据
                    }

                    // FluQRCode {
                    //     color: "black"
                    //     id: loginQRCode
                    //     text: loginQRCodeLink
                    //     size: 140
                    //     anchors.horizontalCenter: parent.horizontalCenter
                    // }

                    FluText {
                        text: qsTr("Scan the QR code to login")
                        font.pixelSize: 13
                        anchors.left: loginQRCode.left
                    }


                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 20

                        // 登录按钮
                        FluFilledButton {
                            id: loginButton2
                            text: qsTr("Login")
                            onClicked: {
                                qjson.qr_login(qrmsg.qrcode);
                                // MyJS.myFunc1();
                                // MyJS.myFunc2();
                            }

                        }

                        FluButton {
                            text: qsTr("Refresh")
                            anchors.left: loginQRCode.left
                            onClicked: {
                                // 这里可以添加刷新二维码的逻辑
                                console.log("QR Code refreshed")

                                qrmsg = qjson.get_qrcode();
                                console.log(qrmsg.qrcode_img)
                            }
                        }
                    }



                }
            }
        }
    }
}
