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
                // 初始时直接设置 pending，由防抖器在合适时机平滑居中
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

    // 返回是否发生变化
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

        // 使用 ListView 高亮机制平滑移动到中间，保留丝滑体验
        highlightRangeMode: ListView.StrictlyEnforceRange
        preferredHighlightBegin: height / 2 - 40
        preferredHighlightEnd: height / 2
        highlightMoveDuration: 420

        // 适度缓存，避免频繁重建 delegate
        cacheBuffer: 600

        // 始终开启 layer，让 GPU 处理可视变换（视觉缩放不影响布局）
        layer.enabled: true
        layer.smooth: true

        QCScrollBar.vertical: QCScrollBar {
            handleNormalColor: "#"+ lyricItemRoot.scroolBarColor
        }

        delegate: Rectangle {
            id: delegateRoot
            width: listView.width
            // 固定 delegate 高度，缩放只作用在 visual 中
            property int fixedHeight: 72
            height: fixedHeight
            color: "transparent"
            radius: 8
            property bool isCurrent: index === lyricItemRoot.current
            property bool isHoverd: false

            Item {
                id: visual
                anchors.fill: parent
                anchors.margins: 6
                transformOrigin: Item.Center

                // 缩放为纯视觉效果，不改变父项高度
                scale: isCurrent ? 1.08 : (isHoverd ? 1.02 : 1.0)
                // 滑动时缩短动画，静止时较平滑
                Behavior on scale { NumberAnimation { duration: listView.moving ? 100 : 260; easing.type: Easing.OutCubic } }

                // 把 visual 放到自身 layer 中，保证 GPU 合成，提高流畅度
                layer.enabled: true
                layer.smooth: true

                Column {
                    id: contentColumn
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width * 0.9
                    spacing: 6
                    anchors.verticalCenter: parent.verticalCenter

                    /* 文本与荧光层堆栈：
                       - glowText：用于产生发光（仅在 isCurrent 时可见）
                       - glowBlur：对 glowText 进行模糊，作为发光扩散
                       - mainText：实际可交互文本，位于最上层
                    */
                    Item {
                        id: textStack
                        width: parent.width
                        height: Math.max(20, mainText.implicitHeight)
                        anchors.horizontalCenter: parent.horizontalCenter

                        Text {
                            id: glowText
                            text: modelData ? modelData.lyric : ""
                            visible: isCurrent
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pointSize: lyricItemRoot.fontSize
                            font.bold: true
                            color: lyricItemRoot.textColor ? lyricItemRoot.textColor : "#FFFF66" // 发光色，回退为亮黄色
                            opacity: 0.9
                            z: -2
                            // 减小绘制开销：在滑动时不实时更新太大尺寸
                            layer.enabled: true
                        }

                        FastBlur {
                            id: glowBlur
                            source: glowText
                            radius: isCurrent ? (listView.moving ? 6 : 14) : 0
                            transparentBorder: true
                            anchors.fill: glowText
                            visible: isCurrent
                            opacity: 0.85
                            z: -3
                            layer.enabled: true
                        }

                        Text {
                            id: mainText
                            text: modelData ? modelData.lyric : ""
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            width: parent.width
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
                    // 直接设置 currentIndex：触发高亮平滑移动
                    listView.currentIndex = index
                    lyricItemRoot.current = index
                    pendingCenterIndex = index
                    centerDebounce.restart()
                }
            }
        }

        Behavior on contentY { NumberAnimation { duration: 350; easing.type: Easing.OutCubic } }
    }

    // 防抖器：合并快速连续的居中请求；若列表正在滑动则延后重试
    Timer {
        id: centerDebounce
        interval: 220
        repeat: false
        onTriggered: {
            if (pendingCenterIndex >= 0 && pendingCenterIndex !== lastCenteredIndex) {
                if (!listView.moving) {
                    // 使用 currentIndex 触发 ListView 的高亮/滚动动画，从而更丝滑
                    listView.currentIndex = pendingCenterIndex
                    lastCenteredIndex = pendingCenterIndex
                    pendingCenterIndex = -1
                } else {
                    // 延后重试，避开用户手势
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
