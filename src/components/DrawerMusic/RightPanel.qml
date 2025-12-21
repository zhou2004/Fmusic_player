// src/components/DrawerMusic/RightPanel.qml
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

    // 防抖 / 重试相关
    property int pendingCenterIndex: -1
    property int lastCenteredIndex: -1

    Component.onCompleted: {
        console.log("RightPanel initialized. lyricData:", lyricData ? lyricData.length : 0)
        if (lyricData && lyricData.length > 0) {
            var pos = (typeof musicPlayer !== "undefined" && musicPlayer && musicPlayer.position) ? musicPlayer.position : 0
            updateCurrentFromPosition(pos)
            if (current >= 0) {
                pendingCenterIndex = current
                centerDebounce.restart()
            }
        }
    }

    onLyricDataChanged: {
        console.log("lyricData changed:", lyricData ? lyricData.length : 0)
        if (lyricData && lyricData.length > 0) {
            var pos = (typeof musicPlayer !== "undefined" && musicPlayer && musicPlayer.position) ? musicPlayer.position : 0
            var changed = updateCurrentFromPosition(pos)
            if (current >= 0 && isFollow && changed) {
                pendingCenterIndex = current
                centerDebounce.restart()
            }
        } else {
            current = -1
        }
    }

    function updateCurrentFromPosition(position) {
        if (!lyricData || lyricData.length === 0) {
            if (current !== -1) current = -1
            return false
        }
        var idx = 0
        for (var i = 0; i < lyricData.length; ++i) {
            var item = lyricData[i]
            if (!item) continue
            if (position >= item.tim) {
                idx = i
            } else {
                break
            }
        }
        var changed = (current !== idx)
        current = idx
        return changed
    }

    Connections {
        enabled: typeof musicPlayer !== "undefined" && musicPlayer != null
        target: musicPlayer
        function onPositionChanged(pos) {
            var changed = updateCurrentFromPosition(pos)
            if (isFollow && current >= 0 && changed) {
                pendingCenterIndex = current
                centerDebounce.restart()
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onWheel: function(wheel) {
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
        preferredHighlightBegin: height / 2 - 40
        preferredHighlightEnd: height / 2
        highlightMoveDuration: 420

        cacheBuffer: 600

        layer.enabled: true
        layer.smooth: true

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

            // delegate 高度基于实际内容高度（不随 visual 缩放变化）
            height: Math.max(48, contentColumn.implicitHeight + 12)

            Item {
                id: visual
                anchors.fill: parent
                anchors.margins: 6
                transformOrigin: Item.Center

                // 视觉层缩放（不影响布局高度）
                scale: isCurrent ? 1.08 : (isHoverd ? 1.02 : 1.0)
                Behavior on scale { NumberAnimation { duration: listView.moving ? 100 : 260; easing.type: Easing.OutCubic } }

                layer.enabled: true
                layer.smooth: true

                Column {
                    id: contentColumn
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width * 0.9
                    spacing: 6
                    anchors.verticalCenter: parent.verticalCenter

                    /* 文本与荧光层：放在同一容器，容器高度绑定到 mainText.contentHeight，
                       确保换行与不换行下堆叠位置一致 */
                    Item {
                        id: textWrapper
                        width: parent.width
                        // 使用 contentHeight 获取文本的真实渲染高度（支持换行）
                        height: Math.max(20, mainText.contentHeight)
                        anchors.horizontalCenter: parent.horizontalCenter
                        clip: false

                        // 发光原文（位于主文本下方，用作 FastBlur 的 source）
                        Text {
                            id: glowText
                            text: modelData ? modelData.lyric : ""
                            visible: isCurrent
                            wrapMode: Text.Wrap
                            width: parent.width
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pointSize: lyricItemRoot.fontSize
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            color: lyricItemRoot.textColor ? lyricItemRoot.textColor : "#FFFF66"
                            opacity: 0.95
                            z: -2
                            // 保持在图层中以供模糊使用
                            layer.enabled: true
                        }

                        FastBlur {
                            id: glowBlur
                            source: glowText
                            // 滑动时减小 radius 以降低开销
                            radius: isCurrent ? (listView.moving ? 6 : 18) : 0
                            transparentBorder: true
                            anchors.fill: glowText
                            visible: isCurrent
                            opacity: 0.85
                            z: -3
                            layer.enabled: true
                        }

                        // 最上层主文本，完全覆盖 glowText 的位置
                        Text {
                            id: mainText
                            text: modelData ? modelData.lyric : ""
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            width: parent.width
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pointSize: lyricItemRoot.fontSize
                            font.bold: isCurrent
                            color: isCurrent ? lyricItemRoot.textColor : "#B0FFFFFF"
                            z: 0
                        }
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
                        if (typeof musicPlayer.seek === "function") musicPlayer.seek(modelData.tim)
                        else if (typeof musicPlayer.position !== "undefined") musicPlayer.position = modelData.tim
                    }
                    listView.currentIndex = index
                    lyricItemRoot.current = index
                    pendingCenterIndex = index
                    centerDebounce.restart()
                }
            }
        }

        Behavior on contentY { NumberAnimation { duration: 350; easing.type: Easing.OutCubic } }
    }

    Timer {
        id: centerDebounce
        interval: 220
        repeat: false
        onTriggered: {
            if (pendingCenterIndex >= 0 && pendingCenterIndex !== lastCenteredIndex) {
                if (!listView.moving) {
                    listView.currentIndex = pendingCenterIndex
                    lastCenteredIndex = pendingCenterIndex
                    pendingCenterIndex = -1
                } else {
                    centerDebounce.restart()
                }
            } else {
                pendingCenterIndex = -1
            }
        }
    }

    Timer {
        id: lyricTimer
        interval: delayFollow
        repeat: false
        onTriggered: {
            lyricItemRoot.isFollow = true
            if (lyricItemRoot.current >= 0) {
                pendingCenterIndex = lyricItemRoot.current
                centerDebounce.restart()
            }
        }
    }
}
