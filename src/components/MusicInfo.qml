import QtQuick
import FluentUI 1.0
import "./"

Rectangle {
    property int mIndex: 0
    property url mCover: ""
    property string mTitle: ""
    property string mArtist: ""
    property string mAlbum: ""

    id: musicInfoRectangle
    width: parent ? parent.width : 700
    height: 64
    color: "transparent"

    Rectangle {
        anchors.fill: parent
        id: musicInfoHoverRectangle
        color: "transparent" // 或需要的颜色
        opacity: 0.9
        z: 50 // 保证在最上层

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                musicInfoHoverRectangle.color = FluTheme.itemHoverColor
            }
            onExited: {
                musicInfoHoverRectangle.color = "transparent"
            }

            // TODO: 点击后立即播放，添加到播放列表等功能
            onClicked: {
                // 这里可以添加点击事件的处理逻辑
                console.log("Clicked on music info at index:", mIndex);
            }
        }
    }

    // 左侧：序号、封面、标题
    Row {
        id: musicInfoRow
        spacing: 12
        height: parent.height
        width: parent.width * 0.4

        FluText {
            text: (mIndex + 1).toString()
            id: musicInfoIndex
            font.pixelSize: 20
            width: 32
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
        }
        RoundImage {
            source: mCover
            id: musicInfoCover
            width: parent.height * 0.8
            height: parent.height * 0.8
            sourceSize.width: parent.height * 0.8
            sourceSize.height: parent.height * 0.8
            radius: 10
            smooth: false
            anchors.verticalCenter: parent.verticalCenter
        }
        FluText {
            text: mTitle
            id: musicInfoTitle
            font.bold: true
            font.pixelSize: 18
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // 中间：艺术家和专辑信息

    Rectangle {
        id: musicInfoDetailRectangle
        color: {
            if(Window.active){
                return FluTheme.windowActiveBackgroundColor
            }
            return FluTheme.windowBackgroundColor
        }
        height: parent.height
        width: parent.width * 0.4
        anchors.left: musicInfoRow.right



        Row {
            id: musicInfoDetailRow
            width: parent.width
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter

            Item { width: 15 ; height: parent.height } // 占位符，用于左侧间距

            FluText {
                text: mArtist
                id: musicInfoArtist
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }

            Item { width: 25 ; height: parent.height } // 占位符，用于中间间距

            FluText {
                text: mAlbum
                id: musicInfoAlbum
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }

        }

    }

    // 最右侧按钮
    FluIconButton {
        id: musicInfoMoreButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        iconSource: FluentIcons.More
        z:60

        FluMenu{
            id:musicInfoMoreButtonMenu

            FluMenuItem {
                text:"Play"
                iconSource: FluentIcons.Play
                onClicked: {
                    showInfo("Play")
                }
            }
            FluMenuItem {
                text:"Add to Playlist"
                iconSource: FluentIcons.MusicInfo
                onClicked: {
                    showInfo("Add to Playlist")
                }
            }

            FluMenuItem {
                text:"Favorite"
                iconSource: FluentIcons.NewFolder
                onClicked: {
                    showInfo("Favorite")
                }
            }

            FluMenuItem {
                text:"Delete"
                iconSource: FluentIcons.Delete
                onClicked: {
                    showInfo("Delete")
                }
            }
        }

        onClicked: {
            if (musicInfoMoreButtonMenu.count !== 0) {
                var pos = musicInfoMoreButton.mapToItem(null, 0, 0)
                var containerHeight = musicInfoMoreButtonMenu.count * 36
                var windowHeight = Window.window.height

                if (windowHeight > pos.y + musicInfoMoreButton.height + containerHeight) {
                    musicInfoMoreButtonMenu.y = musicInfoMoreButton.height
                } else if (pos.y > containerHeight) {
                    musicInfoMoreButtonMenu.y = -containerHeight
                } else {
                    musicInfoMoreButtonMenu.y = windowHeight - (pos.y + containerHeight)
                }

                musicInfoMoreButtonMenu.open()
            }
        }

    }

}