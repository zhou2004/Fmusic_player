import QtQuick 2.15
import FluentUI 1.0
import "../components"

FluFrame {
    property int bottomBarHeight
    property int bottomBarWidth
    property string songName: "杜鹃"
    property string singerName: "Realzat / 加木"
    property string musicDuration: "04:34"
    property string musicPlayTime: "00:00"
    property int infoTextSize: 13
    property bool isPlaying: false // 用于控制播放状态

    id: bottomBar
    width: bottomBarWidth
    height: bottomBarHeight

    // now playing
    Row {
        id: playInfo
        height: parent.height
        width: parent.width * 0.3
        anchors.left: parent.left
        anchors.leftMargin: 10

        // Album picture
        RoundImage {
            id: albumPicture
            width: parent.height * 0.7
            height: parent.height * 0.7
            sourceSize.width: parent.height * 0.7
            sourceSize.height: parent.height * 0.7
            radius: 15
            smooth: false
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/assets/defaultAlbum.jpg"

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                // TODO: 点击专辑图进入音乐播放界面的逻辑
                onClicked: {
                    console.log("Album onClicked")
                }
                onEntered: parent.showColorOverlay = true
                onExited: parent.showColorOverlay = false
            }
        }

        Item {
            height: parent.height; width: 15
        } // spacer

        // song information
        Column {
            spacing: 10
            anchors.verticalCenter: parent.verticalCenter

            // song name
            SrollText {
                id: songNameText
                color: FluTheme.fontPrimaryColor
                size: infoTextSize
                text: songName
                textWidth: parent.width
            }

            FluText {
                id: singerNameText
                font.pointSize: infoTextSize
                color: FluTheme.dark ? FluColors.Grey50 : FluColors.Grey130
                text: singerName
            }
        }
    }

    //play control
    Column {
        id: playControl
        height: parent.height
        width: parent.width * 0.4
        anchors.horizontalCenter: parent.horizontalCenter

        Item {
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            property int topMargin: 15
            property int betweenMargin: 30

            // TODO:上一曲按钮
            FluIconButton {
                id: pre_buttion
                iconSource: FluentIcons.Previous
                iconSize: 15
                anchors.top: parent.top
                anchors.topMargin: parent.topMargin + 3
                anchors.right: musicPlay_buttion.left
                anchors.rightMargin: parent.betweenMargin
            }

            // TODO:播放/暂停按钮
            FluIconButton {
                id: musicPlay_buttion
                iconSource: isPlaying ? FluentIcons.Pause : FluentIcons.Play
                iconSize: 20
                anchors.top: parent.top
                anchors.topMargin: parent.topMargin
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    isPlaying = !isPlaying // 切换播放状态
                    console.log("Music is now " + (isPlaying ? "playing" : "paused"))
                }
            }

            // TODO:下一曲按钮
            FluIconButton {
                id: next_buttion
                iconSource: FluentIcons.Next
                iconSize: 15
                anchors.top: parent.top
                anchors.topMargin: parent.topMargin + 3
                anchors.left: musicPlay_buttion.right
                anchors.leftMargin: parent.betweenMargin
            }

            // TODO: 随机播放按钮
            FluIconButton {
                id: random_buttion
                iconSource: FluentIcons.ShowBcc
                iconSize: 15
                anchors.top: parent.top
                anchors.topMargin: parent.topMargin + 3
                anchors.right: pre_buttion.left
                anchors.rightMargin: parent.betweenMargin

                onClicked: {
                    console.log("Random button clicked")
                }
            }

            // TODO: 收藏按钮
            FluIconButton {
                id: favorite_buttion
                iconSource: FluentIcons.Heart
                iconSize: 15
                anchors.top: parent.top
                anchors.topMargin: parent.topMargin + 3
                anchors.left: next_buttion.right
                anchors.leftMargin: parent.betweenMargin

                onClicked: {
                    console.log("Favorite button clicked")
                }
            }
        }

        Item {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter

            // TODO: 绑定音乐播放时间到slider的value
            // 音乐播放进度条
            FluSlider {
                id: slider
                width: parent.width - 30
                height: 10
                orientation: Qt.Horizontal
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                property bool isDragging: false
                from: 0
                to: 100
            }

            // TODO: 绑定音乐播放时间
            FluText {
                id: musicPlayTimeText
                text: musicPlayTime
                anchors.right: slider.left
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
            }

            FluText {
                id: musicDurationText
                text: musicDuration
                anchors.left: slider.right
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    // right buttons
    Row {
        spacing: 12
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 14
        layoutDirection: Qt.RightToLeft

        // 更多按钮
        FluIconButton {
            id: moreButton
            iconSource: FluentIcons.More
            iconSize: 20
            onClicked: {
                console.log("More button clicked")
            }
        }

        // 播放列表按钮
        FluIconButton {
            id: playlistButton
            iconSource: FluentIcons.MusicInfo
            iconSize: 20
            onClicked: {
                console.log("Playlist button clicked")
            }
        }

        // 音量控制按钮
        FluIconButton {
            id: volumeButton
            iconSource: FluentIcons.Volume
            iconSize: 20
            onClicked: {
                console.log("Volume button clicked")
            }
        }

        // 歌词按钮
        FluIconButton {
            id: lyricsButton
            iconSource: FluentIcons.SpellingChinese
            iconSize: 20
            onClicked: {
                console.log("Lyrics button clicked")
            }
        }
    }
}

