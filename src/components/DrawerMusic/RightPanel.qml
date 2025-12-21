// qmllang
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects 1.0
import "../"

Item {
    id: lyricItemRoot
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    property var lyricData: []
    property int current: -1
    property bool isFollow: true
    property int fontSize: 17
    property int delayFollow: 10000

    Component.onCompleted: {
        console.log("RightPanel initialized. lyricData:", lyricData ? lyricData.length : 0)
        // 试着基于当前播放位置初始化索引（如果有全局 musicPlayer 可用）
        if (lyricData && lyricData.length > 0) {
            var pos = (typeof musicPlayer !== "undefined" && musicPlayer && musicPlayer.position) ? musicPlayer.position : 0
            updateCurrentFromPosition(pos)
            if (current >= 0) listView.positionViewAtIndex(current, ListView.Center)
        }
    }

    onLyricDataChanged: {
        console.log("lyricData changed:", lyricData ? lyricData.length : 0)
        if (lyricData && lyricData.length > 0) {
            var pos = (typeof musicPlayer !== "undefined" && musicPlayer && musicPlayer.position) ? musicPlayer.position : 0
            updateCurrentFromPosition(pos)
            if (current >= 0 && isFollow) listView.positionViewAtIndex(current, ListView.Center)
        } else {
            current = -1
        }
    }

    function updateCurrentFromPosition(position) {
        if (!lyricData || lyricData.length === 0) { current = -1; return }
        var idx = 0
        for (var i = 0; i < lyricData.length; ++i) {
            var item = lyricData[i]
            if (!item) continue
            if (position >= item.tim) {
                idx = i
                // 如果不是最后一条，且下一条的时间仍小于等于 position，则继续循环
            } else {
                break
            }
        }
        if (current !== idx) {
            current = idx
        }
    }

    Connections {
        enabled: typeof musicPlayer !== "undefined" && musicPlayer != null
        target: musicPlayer
        function onPositionChanged(pos) {
            // pos 为毫秒
            updateCurrentFromPosition(pos)
            if (isFollow && current >= 0) {
                listView.positionViewAtIndex(current, ListView.Center)
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onWheel: function(wheel) {
            // 禁止父级阻塞滚动：让 listView 本身可交互（ListView.interactive）
            // 这里仅标记为用户交互，暂不自动跟随
            lyricItemRoot.isFollow = false
            lyricTimer.restart()
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        width: parent.width - 30
        height: parent.height
        model: lyricData
        clip: true
        interactive: true
        spacing: 12
        highlightRangeMode: ListView.StrictlyEnforceRange
        preferredHighlightBegin: height/2 - 40
        preferredHighlightEnd: height/2
        highlightMoveDuration: 400

        QCScrollBar.vertical: QCScrollBar {
            handleNormalColor: "#"+ lyricItemRoot.scroolBarColor
        }

        delegate: Rectangle {
            id: delegateRoot
            width: listView.width
            color: "transparent"
            radius: 8
            property bool isCurrent: index === lyricItemRoot.current
            property bool isHoverd: false
            scale: isCurrent ? 1.12 : (isHoverd ? 1.03 : 1.0)
            transformOrigin: Item.Center
            Behavior on scale { NumberAnimation { duration: 300; easing.type: Easing.OutCubic } }

            height: Math.max(48, contentColumn.implicitHeight + 12)

            Column {
                id: contentColumn
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.75
                spacing: 6
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: mainText
                    text: modelData ? modelData.lyric : ""
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    font.pointSize: lyricItemRoot.fontSize
                    font.bold: isCurrent
                    color: isCurrent ? lyricItemRoot.textColor : "#B0FFFFFF"
                }
                Text {
                    id: transText
                    text: modelData ? modelData.tlrc : ""
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    font.pointSize: Math.max(12, lyricItemRoot.fontSize - 2)
                    color: isCurrent ? lyricItemRoot.textColor : "#80CCCCCC"
                    visible: modelData && modelData.tlrc && modelData.tlrc.length > 0
                }
            }

            Rectangle {
                anchors.fill: parent
                color: isHoverd ? Qt.rgba(1,1,1,0.04) : "transparent"
                z: -1
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: { delegateRoot.isHoverd = true; lyricItemRoot.isFollow = false; lyricTimer.restart() }
                onExited: { delegateRoot.isHoverd = false }
                onClicked: {
                    if (typeof musicPlayer !== "undefined" && musicPlayer && modelData && modelData.tim !== undefined) {
                        // 使用 C\+\+ 对象的 seek 函数（项目中存在时）
                        if (typeof musicPlayer.seek === "function") musicPlayer.seek(modelData.tim)
                        else if (typeof musicPlayer.position !== "undefined") musicPlayer.position = modelData.tim
                    }
                    listView.currentIndex = index
                    lyricItemRoot.current = index
                }
            }
        }

        Behavior on contentY { NumberAnimation { duration: 350; easing.type: Easing.OutCubic } }
    }

    Timer {
        id: lyricTimer
        interval: delayFollow
        repeat: false
        onTriggered: {
            lyricItemRoot.isFollow = true
            if (lyricItemRoot.current >= 0) listView.positionViewAtIndex(lyricItemRoot.current, ListView.Center)
        }
    }
}
