import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
import QtMultimedia
import Qt.labs.platform 1.1
import "../components"

Rectangle {
    id:day_recommend
    width: parent.width
    height: parent.height
    color: mainWindow.rec_color


    Row {
        id: header
        width: parent.width
        height: carousel_1.height

        FluCarousel {
            id: carousel_1
            width: parent.width * 0.9
            height: 300
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 20
            anchors.leftMargin: 20



            delegate: Component {
                FluClip {
                    id: fluClip
                    radius: [8, 8, 8, 8]
                    width: carousel_1.width
                    height: carousel_1.height

                    Image {
                        asynchronous: true
                        anchors.fill: parent
                        source: model.url
                        sourceSize: Qt.size(parent.width, parent.height)
                        fillMode: Image.PreserveAspectCrop
                    }


                }
            }

            Component.onCompleted: {
                carousel_1.model = [
                    { url: "qrc:/Images/Splash_Screen_First_Closed_Beta.png" },
                    { url: "qrc:/Images/tianjing.jpeg" },
                    { url: "qrc:/Images/xianhai.jpeg" },
                    { url: "qrc:/Images/go_star.jpeg" },
                    { url: "qrc:/Images/yinhe.png" },
                    { url: "qrc:/Images/yunshang.jpeg" },
                    { url: "qrc:/Images/yunshang2.jpeg" }
                ]
            }
        }
    }

    Rectangle {
        id: content
        anchors.top: header.bottom
        anchors.topMargin: 30
        width: parent.width
        height: parent.height - header.height
        anchors.left:header.left
        radius: 5
        anchors.horizontalCenter: parent.horizontalCenter
        color: parent.color

        Flickable {
            id: flickable
            anchors.fill: parent
            contentHeight: column.height + flickable.height/2  // 动态调整内容高度
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Column {
                id: column
                width: parent.width
                // spacing: 10

                Repeater {
                    model: mainWindow.music_list
                    delegate: contentDelegate
                }
            }
        }

        Component {
            id: contentDelegate

            Rectangle {
                color: "#FFA5DBFF"
                radius: 5
                property bool isHoverd: false
                width: content.width - 20
                height: 60

                anchors.horizontalCenter: parent.horizontalCenter

                Row {
                    width: parent.width - 20
                    height: parent.height - 20
                    anchors.verticalCenter: parent.verticalCenter

                    RoundImage {
                        id: albumImage
                        width: parent.height
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        radius: 5
                        source: modelData.cover_url

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("onClickedpos")
                            }
                            onEntered: {}
                            onExited: {}
                        }
                    }

                    Text {
                        id: music_name
                        width: parent.width / 3
                        anchors.left: albumImage.right
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.weight: 2
                        font.pointSize: newMusicContent.fontSize
                        elide: Text.ElideRight
                        text: modelData.music_name
                        color: font_color
                    }

                    Text {
                        id: artist_name
                        width: parent.width / 5
                        anchors.left: music_name.right
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.weight: 2
                        font.pointSize: newMusicContent.fontSize
                        elide: Text.ElideRight
                        text: modelData.art_name
                        color: font_color
                    }

                    Text {
                        id: album_name
                        width: parent.width / 5
                        anchors.left: artist_name.right
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.weight: 2
                        font.pointSize: newMusicContent.fontSize
                        elide: Text.ElideRight
                        text: modelData.album
                        color: font_color
                    }

                    Image {
                        id: collectImage
                        source: "qrc:/Images/myFavorite.svg"
                        // anchors.left: album_name.right
                        anchors.right: music_duration.left
                        // anchors.leftMargin: 10
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        asynchronous: true
                        width: 25
                        height: width
                        // 使用 PreserveAspectFit 确保在原始比例下不会变形
                        fillMode: Image.PreserveAspectFit
                        clip: true
                        // visible: false // 因为显示的是 OpacityMask 需要 false

                    }

                    Text {
                        id: music_duration
                        width: parent.width / 12
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignRight
                        font.weight: 2
                        font.pointSize: newMusicContent.fontSize
                        elide: Text.ElideRight
                        text: modelData.duration
                        color: font_color
                    }
                }

                MouseArea {
                    id: mainMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        var collectX = collectImage.x
                        var collectY = collectImage.y
                        var collectWidth = collectImage.width
                        var collectHeight = collectImage.height

                        // 判断鼠标点击是否在 collectImage 的范围内
                        if (!(mouse.x >= collectX && mouse.x <= collectX + collectWidth &&
                            mouse.y >= collectY && mouse.y <= collectY + collectHeight)) {
                            mainWindow.current_task_index = index
                            musicPlayer.play(qjson.get_kugou_url(modelData.hash))
                            musicPlayer.Set_Q_Current_Task(index)
                            albumart = modelData.cover_url
                            lyrics.parseLyric(qjson.get_kugou_lyric(modelData.hash))
                        }else {

                            if(musicPlayer.add_user_collections(mainWindow.user.userId, modelData.music_id)) {
                                showSuccess(qsTr("收藏成功"))
                            } else {
                                showError(qsTr("收藏失败"))
                            }
                        }
                    }
                    onEntered: {
                        parent.isHoverd = true
                    }
                    onExited: {
                        parent.isHoverd = false
                    }
                }
            }
        }
    }
}