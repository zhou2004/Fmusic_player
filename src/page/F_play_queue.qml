import QtQuick 2.15
import FluentUI 1.0
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts 1.15
import "../components"

Rectangle {
    id: play_queue
    width: parent.width
    height: parent.height
    radius: 5
    // anchors.horizontalCenter: parent.horizontalCenter
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
            color: index === mainWindow.current_task_index ? mainWindow.rec_color : "white"
            radius: 5
            property bool isHoverd: false
            width: play_queue.width
            height: 60

            anchors.horizontalCenter: parent.horizontalCenter

            Row {
                width: parent.width - 20
                height: parent.height - 20
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: music_index
                    width: parent.width / 10
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignLeft
                    font.weight: 2
                    font.pointSize: newMusicContent.fontSize
                    elide: Text.ElideRight
                    text: index + 1
                    color: font_color
                }

                RoundImage {
                    id: albumImage
                    width: parent.height
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: music_index.right
                    // anchors.leftMargin: 10
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

                // Text {
                //     id: album_name
                //     width: parent.width / 5
                //     anchors.left: artist_name.right
                //     anchors.verticalCenter: parent.verticalCenter
                //     horizontalAlignment: Text.AlignHCenter
                //     font.weight: 2
                //     font.pointSize: newMusicContent.fontSize
                //     elide: Text.ElideRight
                //     text: modelData.album
                //     color: font_color
                // }

                Text {
                    id: music_duration
                    width: parent.width / 6
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
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    mainWindow.current_task_index = index
                    musicPlayer.play(modelData.play_url)
                    musicPlayer.Set_Q_Current_Task(index)
                    console.log("indexxxxx: ",index)
                    albumartManager.updateAlbumArt()
                    lyrics.parseLyric(modelData.lyric_url)
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