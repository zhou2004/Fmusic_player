import QtQuick
import QtQuick.Layouts
import FluentUI 1.0
import "../Components"

ColumnLayout {
    id: settingsAboutInfo
    Layout.fillWidth: true
    Layout.fillHeight: true

    RowLayout {
        id: settingsAboutLogo
        Layout.alignment: Qt.AlignHCenter
        Layout.rightMargin: 10
        Layout.topMargin: 10

        Image {
            id: settingsAboutlogoImage
            source: "qrc:/qt/qml/FMusic/Assets/icon.svg"
            sourceSize.width: 80
            sourceSize.height: 80
        }

        FluText {
            id: settingsAboutLogoText
            text: "FMusicPlayer"
            font.pixelSize: 40
            font.bold: true
            Layout.leftMargin: 10
        }
    }

    FluText {
        id: settingsAboutLicense
        text: qsTr("This is an open source-project under GPL-3.0 license.")
        font.pixelSize: 14
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 5
        Layout.rightMargin: 10
    }

    RowLayout {
        id: settingsAboutLinks
        Layout.alignment: Qt.AlignHCenter
        Layout.rightMargin: 10

        FluTextButton {
            id: settingsAboutLinksGitHub
            text: qsTr("GitHub")
            font.pixelSize: 14
            textColor: FluTheme.dark ? linkColorDark:linkColor
            onClicked: {
                Qt.openUrlExternally(
                    "https://github.com/zhou2004/Fmusic_player")
            }
        }

        FluTextButton {
            id: settingsAboutLinksGitee
            text: qsTr("Gitee")
            font.pixelSize: 14
            textColor: FluTheme.dark ? linkColorDark:linkColor
            onClicked: {
                Qt.openUrlExternally(
                    "https://gitee.com/zhou2004jj/fmusic_player")
            }
        }
    }

    FluText {
        id: settingsAboutCopyright
        text: qsTr("Copyright © 2025 FMusicPlayer Team")
        font.pixelSize: 14
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        Layout.alignment: Qt.AlignHCenter
        Layout.rightMargin: 10
    }

    Rectangle {
        id: settingsAboutVersionContainer
        Layout.fillWidth: true
        Layout.rightMargin: 10
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 5
        color: "transparent"
        height: 80

        Item {
            id: settingsAboutVersionPlaceholder
            width: 10
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
        } // 占位符，用于水平居中

        // 版本信息
        FluFrame {
            id: settingsAboutVersionFrame
            width: parent.width * 0.45 > 360 ? 360 : parent.width * 0.45
            height: parent.height
            anchors.right: settingsAboutVersionPlaceholder.left

            Column {
                spacing: 6

                Item {
                    width: parent.width
                    height: 1
                } // 占位符，用于顶部间距

                Row {
                    spacing: 5
                    anchors.left: parent.left
                    anchors.leftMargin: 8

                    FluIcon {
                        anchors.top: parent.top
                        anchors.topMargin: 2
                        iconSource: FluentIcons.Error
                        iconSize: 15
                    }
                    FluText {
                        id: settingsAboutVersionText
                        text: qsTr("Version Information")
                        font.pixelSize: 14
                    }
                }

                FluCopyableText {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    width: settingsAboutVersionFrame.width - 20
                    wrapMode: Text.WrapAnywhere
                    color: FluTheme.dark ? FluColors.Grey50 : FluColors.Grey130
                    text: "Version: " + appVersion
                        + " (PackageID: com.example.fmusicplayer)"
                }
            }
        }

        // 设备ID
        FluFrame {
            id: settingsAboutDeviceFrame
            width: parent.width * 0.45 > 360 ? 360 : parent.width * 0.45
            height: parent.height
            anchors.left: settingsAboutVersionPlaceholder.right

            Column {
                spacing: 6

                Item {
                    width: parent.width
                    height: 1
                } // 占位符，用于顶部间距

                Row {
                    spacing: 5
                    anchors.left: parent.left
                    anchors.leftMargin: 8

                    FluIcon {
                        anchors.top: parent.top
                        anchors.topMargin: 2
                        iconSource: FluentIcons.Connect
                        iconSize: 15
                    }
                    FluText {
                        id: settingsAboutDeviceText
                        text: qsTr("Device ID")
                        font.pixelSize: 14
                    }
                }

                FluCopyableText {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    width: settingsAboutDeviceFrame.width - 20
                    wrapMode: Text.WrapAnywhere
                    color: FluTheme.dark ? FluColors.Grey50 : FluColors.Grey130
                    text: appDeviceID
                }
            }
        }
    }

    // 作者1
    Rectangle {
        id: settingsAboutAuthors1Container
        Layout.fillWidth: true
        Layout.rightMargin: 10
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 5
        color: "transparent"
        height: 60

        FluFrame {
            id: settingsAboutAuthors1Frame
            width: parent.width * 0.9 + 10 > 730 ? 730 : parent.width * 0.9 + 10
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter

            Row {
                spacing: 12
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 8

                RoundImage {
                    source: feelzhouAvatar
                    radius: 100
                    width: 50
                    height: 50
                    isHovered: false
                    sourceSize.width: 50
                    sourceSize.height: 50
                    smooth: false
                    anchors.verticalCenter: parent.verticalCenter
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter

                    FluText {
                        id: settingsAboutAuthors1Name
                        text: qsTr("Feel Zhou")
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Row {
                        id: settingsAboutAuthors1Desc
                        spacing: 5

                        FluText {
                            text: qsTr("Developer")
                            font.pixelSize: 14
                        }

                        FluText {
                            text: qsTr("kawai ( ﹁ ﹁ ) ~→")
                            font.pixelSize: 14
                            color: FluTheme.dark ? FluColors.Grey50 : FluColors.Grey130
                        }

                    }

                }

            }

            Row {
                spacing: 10
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 14
                layoutDirection: Qt.RightToLeft

                FluTextButton {
                    text: qsTr("GitHub")
                    font.pixelSize: 14
                    textColor: FluTheme.dark ? linkColorDark:linkColor
                    onClicked: {Qt.openUrlExternally("https://github.com/zhou2004")}
                }

            }
        }
    }

    // 作者2
    Rectangle {
        id: settingsAboutAuthors2Container
        Layout.fillWidth: true
        Layout.rightMargin: 10
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 5
        color: "transparent"
        height: 60

        FluFrame {
            id: settingsAboutAuthors2Frame
            width: parent.width * 0.9 + 10 > 730 ? 730 : parent.width * 0.9 + 10
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter

            Row {
                spacing: 12
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 8

                RoundImage {
                    source: feelliaoAvatar
                    radius: 100
                    width: 50
                    height: 50
                    isHovered: false
                    sourceSize.width: 50
                    sourceSize.height: 50
                    smooth: false
                    anchors.verticalCenter: parent.verticalCenter
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter

                    FluText {
                        id: settingsAboutAuthors2Name
                        text: qsTr("Feel Liao")
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Row {
                        id: settingsAboutAuthors2Desc
                        spacing: 5

                        FluText {
                            text: qsTr("Designer")
                            font.pixelSize: 14
                        }

                        FluText {
                            text: qsTr("Beauty is everywhere")
                            font.pixelSize: 14
                            color: FluTheme.dark ? FluColors.Grey50 : FluColors.Grey130
                        }

                    }

                }

            }

            Row {
                spacing: 10
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 14
                layoutDirection: Qt.RightToLeft

                FluTextButton {
                    text: "酷安@Solariver2027"
                    font.pixelSize: 14
                    textColor: FluTheme.dark ? linkColorDark:linkColor
                    onClicked: {Qt.openUrlExternally("http://www.coolapk.com/u/644855")}
                }

                FluTextButton {
                    text: qsTr("Blog")
                    font.pixelSize: 14
                    textColor: FluTheme.dark ? linkColorDark:linkColor
                    onClicked: {Qt.openUrlExternally("https://blog.stariverfeel.eu.org/")}
                }

                FluTextButton {
                    text: qsTr("GitHub")
                    font.pixelSize: 14
                    textColor: FluTheme.dark ? linkColorDark:linkColor
                    onClicked: {Qt.openUrlExternally("https://github.com/FeelLiao")}
                }

            }
        }
    }
}