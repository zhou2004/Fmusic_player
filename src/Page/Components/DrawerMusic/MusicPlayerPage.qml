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
    property int music_pos: 0

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

        function onPositionChanged(pos) {
            // pos 为毫秒
            music_pos = pos
        }

        function onCurrentTrackInfoChanged(info) {
            if (info && typeof info === "object") {
                trackInfo = info
            } else if (musicPlayer.currentTrackInfo) {
                trackInfo = musicPlayer.currentTrackInfo()
            }
            console.log("MusicPlayerPage trackInfo updated:", JSON.stringify(trackInfo))
        }

    }

    Connections {
        target: lyricParser
    }

    Connections {
        target: apiClient
    }

    /* 1) 在 root Item 内（属性区）添加 */
    property real _dragStartY: 0
    property bool _dragging: false
    function startCloseAnimation() {
        closeAnim.from = root.y
        closeAnim.to = root.height
        closeAnim.start()
    }

    function cancelCloseAnimation() {
        closeBackAnim.from = root.y
        closeBackAnim.to = 0
        closeBackAnim.start()
    }

    /* 2) 在 root Item 中定义两个 PropertyAnimation（可放在文件任意位置） */
    PropertyAnimation {
        id: closeAnim
        target: root
        property: "y"
        duration: 360
        easing.type: Easing.InCubic
        onStopped: {
            root.requestClose()
            // root.y = 0
            // // 重置拖拽相关状态（若 MouseArea 尚在拖拽中）
            // if (pullAreaMouse) {
            //     pullAreaMouse._dragActive = false
            //     pullAreaMouse._startY = 0
            // }
            // root._dragging = false
        }
    }

    PropertyAnimation {
        id: closeBackAnim
        target: root
        property: "y"
        duration: 260
        easing.type: Easing.OutCubic
        onStopped: {
            // 回弹完毕，确保状态恢复
            root.y = 0
            if (pullAreaMouse) {
                pullAreaMouse._dragActive = false
                pullAreaMouse._startY = 0
            }
            root._dragging = false
        }
    }


    Image {
        id: bgImage
        anchors.fill: parent
        source: trackInfo && trackInfo.cover ? trackInfo.cover : "qrc:/qt/qml/FMusic/Assets/defaultAlbum.jpg"
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
            onClicked: startCloseAnimation()
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
            cover: trackInfo && trackInfo.cover ? trackInfo.cover : "qrc:/qt/qml/FMusic/Assets/defaultAlbum.jpg"
        }

        RightPanel {
            id: rightPanel
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width * 0.6

            lyricData: trackInfo && trackInfo.lyrics ? lyricParser.parseLyrics(apiClient.getKrcContent(trackInfo.title,trackInfo.artist)) : []

        }
    }

    function parseLyrics(lyricsText) {
        var map = {} // timeInMs -> { tim, lyric, tlrc }
        var lines = (lyricsText || "").split(/\r?\n/)
        var timeTagRegex = /\[(\d{2}):(\d{2})(?:\.(\d{2,3}))?\]/g

        for (var i = 0; i < lines.length; i++) {
            var line = lines[i].trim()
            if (!line) continue

            // 收集该行所有时间标签
            var tags = []
            timeTagRegex.lastIndex = 0
            var m
            while ((m = timeTagRegex.exec(line)) !== null) {
                var minutes = parseInt(m[1])
                var seconds = parseInt(m[2])
                var millis = m[3] ? parseInt(m[3].padEnd(3, '0')) : 0
                var timeInMs = (minutes * 60 + seconds) * 1000 + millis
                tags.push(timeInMs)
            }

            if (tags.length === 0) continue

            // 获取纯文本（去掉所有时间标签）
            var text = line.replace(/\[(\d{2}):(\d{2})(?:\.(\d{2,3}))?\]/g, '').trim()
            if (text === "") continue

            // 将文本分配到每个时间戳：先填 lyric，再填 tlrc（若已有则追加）
            for (var j = 0; j < tags.length; j++) {
                var t = tags[j]
                if (!map[t]) {
                    map[t] = { time: t, lyric: text, tlrc: "" }
                } else {
                    if (!map[t].lyric) {
                        map[t].lyric = text
                    } else if (!map[t].tlrc) {
                        // 第二条视为翻译
                        map[t].tlrc = text
                    } else {
                        // 已有翻译则追加（保留换行）
                        map[t].tlrc += "\n" + text
                    }
                }
            }
        }

        // 转数组并按时间排序
        var result = []
        for (var k in map) {
            if (map.hasOwnProperty(k)) result.push(map[k])
        }
        result.sort(function(a, b) { return a.tim - b.tim })
        return result
    }

}