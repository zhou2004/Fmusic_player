// language: qmllang
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects

Item {
    id: root
    property var lyricsModel    // 外部传入 ListModel
    property int currentPosition: 0 // 当前播放位置（毫秒）

    // 当前行索引
    property int currentIndex: -1

    // 根据 currentPosition 计算当前行
    function updateCurrentIndex() {
        if (!lyricsModel || lyricsModel.count === 0)
            return
        var idx = 0
        for (var i = 0; i < lyricsModel.count; ++i) {
            var item = lyricsModel.get(i)
            if (item.time <= currentPosition) {
                idx = i
            } else {
                break
            }
        }
        if (currentIndex !== idx) {
            currentIndex = idx
        }
    }

    onCurrentPositionChanged: updateCurrentIndex()

    // 中央高亮区背景（类似 BetterLyrics 中的中心对齐）
    Rectangle {
        anchors.fill: parent
        color: "transparent"

        // 上下渐隐
        LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(0, height)
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(0,0,0,0.8) }
                GradientStop { position: 0.5; color: Qt.rgba(0,0,0,0.1) }
                GradientStop { position: 1.0; color: Qt.rgba(0,0,0,0.8) }
            }
        }
    }

    ListView {
        id: lyricsView
        anchors.fill: parent
        anchors.margins: 16
        model: lyricsModel

        spacing: 8
        clip: true
        cacheBuffer: 4000
        highlightMoveDuration: 300
        interactive: true

        delegate: Item {
            width: lyricsView.width
            height: 32

            property bool isCurrent: (index === root.currentIndex)

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.9
                text: model.text
                wrapMode: Text.NoWrap
                horizontalAlignment: Text.AlignHCenter
                color: isCurrent ? "#FFFFFF" : "#888888"
                font.pixelSize: isCurrent ? 18 : 14
            }
        }

        // 当前行高亮条
        highlight: Rectangle {
            width: parent.width
            height: 40
            color: "transparent"
            border.color: Qt.rgba(255,255,255,0.16)
        }

        onCurrentIndexChanged: {
            if (currentIndex >= 0) {
                positionViewAtIndex(currentIndex, ListView.Center)
            }
        }
    }

    // 当 root.currentIndex 变化时同步 ListView.currentIndex
    onCurrentIndexChanged: {
        if (currentIndex >= 0 && currentIndex < lyricsModel.count) {
            lyricsView.currentIndex = currentIndex
        }
    }
}
