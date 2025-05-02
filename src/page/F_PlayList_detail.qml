import QtQuick 2.15
import FluentUI 1.0
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts 1.15
import "../components"

Rectangle {
    id: playList_detail
    anchors.fill: parent
    color: mainWindow.rec_color
    property var play_listContent: {}

    onPlay_listContentChanged: {
        if (play_listContent.playlist_id  !== undefined) {
            mainWindow.music_list  = musicPlayer.Get_PlaylistDetail(play_listContent.playlist_id)
        }
    }


    Row {

        id: header
        width: parent.width
        height:roundImage.height
        RoundImage {
            id: roundImage
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin:20
            anchors.leftMargin:20
            source: play_listContent.cover_url
            radius: 5
            width: 200
            height: 200
        }

        Text {
            id: textItem_1
            anchors.top: roundImage.top
            anchors.left: roundImage.right
            anchors.leftMargin: 20
            text: play_listContent.playlist_name
            font.pixelSize:  16
        }
        Text {
            id: textItem_2
            anchors.top: textItem_1.bottom
            anchors.left: roundImage.right
            anchors.leftMargin: 20
            text: play_listContent.description
            font.pixelSize:  16
        }
        FluFilledButton{
            width: 80
            height: 40
            anchors.left: roundImage.right
            anchors.leftMargin: 20
            anchors.bottom: roundImage.bottom
            FluIconButton{
                anchors.left:parent.left
                anchors.verticalCenter: parent.verticalCenter
                iconSource:FluentIcons.CaretRightSolid8
                onClicked: {

                }
            }
            Text {
                anchors.right:parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                text:"播放"
                color: "white"
                font.pointSize: 13
                horizontalAlignment: Text.AlignRight
            }

            onClicked: {

            }
        }
    }

    Rectangle {
        id: content
        anchors.top: header.bottom
        anchors.topMargin: 30
        width: parent.width
        height: parent.height
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
                            musicPlayer.play(modelData.play_url)
                            musicPlayer.Set_Q_Current_Task(index)
                            albumartManager.updateAlbumArt()
                            lyrics.parseLyric(modelData.lyric_url)
                        }else {
                            // console.log("UserId: ", mainWindow.user.userId)
                            // console.log("music_id: ", modelData.music_id)
                            // showSuccess(qsTr("收藏成功"))
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