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

    property int itemsPerPage: mainWindow.music_list.length
    property int currentPage: 1
    property int totalItems: mainWindow.music_list.length
    property int totalPages: Math.ceil(totalItems / itemsPerPage)

    // 模拟一个按钮来触发换页
    function goToPage(page) {

        // 直接设置 currentPage 属性来触发换页
        currentPage = page
        // pagination.page = page
        console.log("Go to page: " + page)

    }

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
                id: pagination

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
                        mainWindow.music_list = musicPlayer.get_user_collections(mainWindow.user.userId,page-1)
                        // mainWindow.request_total = mainWindow.music_list.length
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
                    source: "qrc:/Images/favoriteMode.svg"
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
                        console.log("indexxxxx: ", index)
                        albumartManager.updateAlbumArt()
                        lyrics.parseLyric(modelData.lyric_url)
                    }else {
                        // console.log("UserId: ", mainWindow.user.userId)
                        // console.log("music_id: ", modelData.music_id)
                        // showSuccess(qsTr("收藏成功"))
                        if(musicPlayer.del_user_collections(mainWindow.user.userId, modelData.music_id)) {
                            showSuccess(qsTr("删除成功"))
                            music_list = musicPlayer.get_user_collections(mainWindow.user.userId,0)
                        } else {
                            showError(qsTr("删除失败"))
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

    function getPaginatedModel() {
        var start = 0;
        var end = mainWindow.music_list.length;
        return mainWindow.music_list.slice(start, end);
    }
}