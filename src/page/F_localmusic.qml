import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
import QtMultimedia
import Qt.labs.platform 1.1
import "../components"
// 使用ListView展示音乐列表


Rectangle {
    id: content
    anchors.top: parent.top
    anchors.topMargin: 20
    width: parent.width * .9
    height: parent.height
    radius: 5
    anchors.horizontalCenter: parent.horizontalCenter
    color: mainWindow.ri_color

    property int itemsPerPage: 10
    property int currentPage: 1
    property int totalItems: mainWindow.request_total
    property int totalPages: Math.ceil(totalItems / itemsPerPage)

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: column.height + 60 // 动态调整内容高度
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: column
            width: parent.width
            anchors.top: parent.top
            anchors.topMargin: 10

            Repeater {
                model: getPaginatedModel()
                delegate: contentDelegate
            }

            FluPagination {

                anchors.horizontalCenter: parent.horizontalCenter
                pageCurrent: currentPage
                itemCount: totalItems
                pageButtonCount: 9
                __itemPerPage: itemsPerPage

                onRequestPage: {
                    if (page >= 1 && page <= totalPages) {
                        currentPage = page;
                        flickable.contentY = 0; // 滚动到顶部
                        musicPlayer.Set_Q_current_page(page)
                        mainWindow.music_list = musicPlayer.get_all_music_path()
                        // musicPlayer.get_Directory(mainWindow.music_folder)
                    }
                }
            }
        }
    }

    Component {
        id: contentDelegate

        Rectangle {
            color: "#FFA5DBFF"
            radius: 8
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
                    source: "data:image/png;base64," + modelData.albumimage

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
                    text: modelData.title
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
                    text: modelData.Author
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
                    musicPlayer.play(modelData.filepath)
                    musicPlayer.Set_Q_Current_Task(index)
                    mainWindow.current_task_index = index
                    albumartManager.updateAlbumArt()
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

    function getPaginatedModel() {
        var start = 0;
        var end = 10;
        return mainWindow.music_list.slice(start, end);
    }
}