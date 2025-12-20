// language: qmllang
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects
import FluentUI 1.0
import "."   // 保证能加载同目录的 LeftPanel / RightPanel


Item {
    id: root

    // trackInfo 不再在定义时调用函数
    property var trackInfo: ({})

    signal requestClose()

    // 初始化时从 C\+\+ 拉一次
    Component.onCompleted: {
        if (musicPlayer && musicPlayer.currentTrackInfo) {
            trackInfo = musicPlayer.currentTrackInfo()
        }
    }

    // 监听 C\+\+ 的信号，更新 trackInfo
    Connections {
        target: musicPlayer

        function onCurrentTrackInfoChanged(info) {
            if (info && typeof info === "object") {
                trackInfo = info
            } else if (musicPlayer.currentTrackInfo) {
                trackInfo = musicPlayer.currentTrackInfo()
            }
            console.log("MusicPlayerPage trackInfo updated:", JSON.stringify(trackInfo))
        }
    }



    Image {
        id: bgImage
        anchors.fill: parent
        source: trackInfo && trackInfo.cover ? trackInfo.cover : "qrc:/assets/defaultAlbum.jpg"
        fillMode: Image.PreserveAspectCrop
        visible: false
    }

    FastBlur {
        id: blur
        anchors.fill: parent
        source: bgImage
        radius: 64
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"

        Rectangle {
            id: glow
            width: parent.width * 0.6
            height: parent.height
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(1,1,1,0.0) }
                GradientStop { position: 0.5; color: Qt.rgba(1,1,1,0.18) }
                GradientStop { position: 1.0; color: Qt.rgba(1,1,1,0.0) }
            }
            rotation: 20
            opacity: 0.7
            x: -width

            NumberAnimation on x {
                from: -glow.width
                to: parent.width
                duration: 6000
                loops: Animation.Infinite
                easing.type: Easing.InOutQuad
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.45)
    }

    Row {
        id: headerRow
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 12
        spacing: 8

        FluIconButton {
            id: pullDownButton
            iconSource: FluentIcons.ChevronDown
            iconSize: 16
            // tooltip: "下拉关闭"
            onClicked: root.requestClose()
        }

        Text {
            text: trackInfo && trackInfo.title ? trackInfo.title : "正在播放"
            color: "white"
            font.pixelSize: 16
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
    }

    Row {
        id: mainRow
        anchors.top: headerRow.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 32
        spacing: 24

        LeftPanel {
            id: leftPanel
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width * 0.4

            title: trackInfo && trackInfo.title ? trackInfo.title : "未知歌曲"
            artist: trackInfo && trackInfo.artist ? trackInfo.artist : "未知歌手"
            album: trackInfo && trackInfo.album ? trackInfo.album : "未知专辑"
            cover: trackInfo && trackInfo.cover ? trackInfo.cover : "qrc:/assets/defaultAlbum.jpg"
        }

        RightPanel {
            id: rightPanel
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width * 0.6

            lyricsModel: musicPlayer.lyricsModel
            currentPosition: musicPlayer.position
        }
    }
}
