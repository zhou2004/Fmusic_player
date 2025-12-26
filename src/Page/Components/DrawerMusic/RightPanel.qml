import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects 1.0
// 如果是 Qt5 且没装 Compat，用 import QtGraphicalEffects 1.15

Item {
    id: lyricItemRoot
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    // ============================================
    // 调试开关
    property bool debugMode: true
    // ============================================

    property var lyricData: []
    property int currentTime: 0
    property int currentLineIndex: -1
    property bool isFollow: true

    // --- 样式配置 ---
    property int fontSize: 22
    property int lineSpacing: 30 // 行间距

    // 1. 默认/非焦点颜色 (所有未播放、已播放的行都用这个颜色)
    property color textColor: "#80FFFFFF" // 半透明白

    // 2. 焦点/高亮颜色 (当前行逐字扫过的颜色)
    property color highlightColor: "#fbfbfb" // 金黄色

    // 3. 荧光发光参数 (关键调整)
    property color glowColor: "#1da5ed" // 发光颜色通常和高亮色一致
    property int glowRadius: 1          // 不要太大，太大会糊
    property real glowSpread: 0.2        // 扩散度，越小越柔和

    // 滚动相关
    property int delayFollow: 2000
    property int pendingCenterIndex: -1

    Component.onCompleted: {
        if(debugMode) console.log("[DEBUG] Lyric Component Loaded.");
    }

    Connections {
        target: (typeof musicPlayer !== "undefined") ? musicPlayer : null
        ignoreUnknownSignals: true
        function onPositionChanged(pos) {
            lyricItemRoot.currentTime = pos;
            if (debugMode && pos % 1000 < 50) {
                // console.log("[DEBUG] Recv Position:", pos);
            }
        }
    }

    onLyricDataChanged: updateCurrentLine()
    onCurrentTimeChanged: updateCurrentLine()

    function updateCurrentLine() {
        if (!lyricData || lyricData.length === 0) return;
        var newIndex = -1;
        for (var i = 0; i < lyricData.length; ++i) {
            if (currentTime >= lyricData[i].lineStartTime) {
                newIndex = i;
            } else {
                break;
            }
        }
        if (newIndex !== currentLineIndex) {
            currentLineIndex = newIndex;
            if (isFollow && currentLineIndex >= 0) {
                pendingCenterIndex = currentLineIndex;
                centerDebounce.restart();
            }
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        anchors.margins: 20
        model: lyricData
        clip: true
        spacing: lyricItemRoot.lineSpacing

        // 高亮行保持居中
        highlightRangeMode: ListView.StrictlyEnforceRange
        preferredHighlightBegin: height / 2 - 50
        preferredHighlightEnd: height / 2 + 50
        highlightMoveDuration: 500

        delegate: Item {
            id: lineDelegate
            width: listView.width
            // 【关键修改】高度改为 Column 的高度 + 上下边距(例如20)，保证包裹住原文和翻译
            height: contentColumn.implicitHeight + 24

            property int lineStart: modelData.lineStartTime
            property var charsList: modelData.lyrics
            property bool isCurrentLine: index === lyricItemRoot.currentLineIndex

            // 0. 辅助计算 (保持不变)
            property string fullLineText: {
                var txt = "";
                if (charsList) {
                    for (var i = 0; i < charsList.length; i++) {
                        txt += charsList[i].text;
                    }
                }
                return txt;
            }

            TextMetrics {
                id: tm
                text: lineDelegate.fullLineText
                font.pointSize: lyricItemRoot.fontSize
                font.bold: true
            }

            // 1. 鼠标悬停背景 (现在会完整包裹两行)
            Rectangle {
                anchors.fill: parent
                // 稍微留一点边距，让高亮背景不至于贴着列表边缘
                anchors.margins: 2
                color: isHovered ? "#1AFFFFFF" : "transparent"
                radius: 6
            }
            property bool isHovered: false

            // 【关键修改】使用 Column 垂直排列 原文 和 翻译
            Column {
                id: contentColumn
                width: parent.width
                anchors.centerIn: parent // 让整个内容块在行内垂直居中
                spacing: 10 // 【关键修改】原文和翻译间隔 10

                // 2. 原文歌词排版
                Flow {
                    id: flowLayout
                    // 宽度逻辑保持不变：智能判断居中还是换行
                    width: Math.min(parent.width * 0.95, tm.width + 10)

                    // 在 Column 内部，需要手动指定水平居中
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 0

                    Repeater {
                        model: charsList
                        delegate: Item {
                            id: charDelegate
                            width: bgText.implicitWidth
                            height: bgText.implicitHeight

                            property string charText: modelData.text
                            property int charOffset: modelData.start_time
                            property int charDuration: modelData.duration
                            property int absStart: lineDelegate.lineStart + charOffset
                            property int absEnd: absStart + charDuration

                            property real progress: {
                                if (!lineDelegate.isCurrentLine) return 0.0;
                                var t = lyricItemRoot.currentTime;
                                if (t < absStart) return 0.0;
                                if (t >= absEnd) return 1.0;
                                return (t - absStart) / charDuration;
                            }

                            // A. 底色文字
                            Text {
                                id: bgText
                                text: charDelegate.charText
                                font.pointSize: lyricItemRoot.fontSize
                                font.bold: true
                                color: lyricItemRoot.textColor
                                anchors.centerIn: parent
                                renderType: Text.QtRendering
                            }

                            // B. 高亮遮罩
                            Item {
                                id: maskItem
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                width: parent.width * charDelegate.progress
                                clip: true
                                visible: lineDelegate.isCurrentLine

                                Text {
                                    id: fgText
                                    text: charDelegate.charText
                                    font.pointSize: lyricItemRoot.fontSize
                                    font.bold: true
                                    color: lyricItemRoot.highlightColor
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    renderType: Text.QtRendering
                                }

                                // C. 荧光效果
                                layer.enabled: lineDelegate.isCurrentLine
                                layer.effect: Glow {
                                    samples: 16
                                    radius: lyricItemRoot.glowRadius
                                    color: lyricItemRoot.glowColor
                                    spread: lyricItemRoot.glowSpread
                                    transparentBorder: true
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    if (typeof musicPlayer !== "undefined") {
                                        if(musicPlayer.seek) musicPlayer.seek(charDelegate.absStart)
                                        else musicPlayer.position = charDelegate.absStart
                                    }
                                    lyricItemRoot.isFollow = true
                                    lyricItemRoot.currentLineIndex = lineDelegate.index
                                    listView.currentIndex = lineDelegate.index
                                }
                            }
                        }
                    }
                }

                // 3. 翻译歌词
                Text {
                    id: transText
                    // 在 Column 内部水平居中
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: modelData.tlrc || ""
                    visible: text !== "" // 没有翻译时自动隐藏，不占高度

                    width: parent.width * 0.95 // 限制宽度以支持换行
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap

                    // 样式调整：翻译通常比原文稍小，且颜色淡一点区分层级
                    font.pointSize: lyricItemRoot.fontSize * 0.85
                    font.bold: true
                    color: lineDelegate.isCurrentLine ? "#FFFFFF" : lyricItemRoot.textColor
                    opacity: lineDelegate.isCurrentLine ? 1.0 : 0.7

                    renderType: Text.QtRendering
                }
            } // End Column

            // 行交互
            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                hoverEnabled: true
                onEntered: lineDelegate.isHovered = true
                onExited: lineDelegate.isHovered = false
                onClicked: {
                    if (typeof musicPlayer !== "undefined") {
                        if(musicPlayer.seek) musicPlayer.seek(lineDelegate.lineStart)
                        else musicPlayer.position = lineDelegate.lineStart
                    }
                    lyricItemRoot.isFollow = true
                    listView.currentIndex = index
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onWheel: (wheel) => {
                lyricItemRoot.isFollow = false
                restoreFollowTimer.restart()
                wheel.accepted = false
            }
        }

        Timer {
            id: centerDebounce
            interval: 100
            repeat: false
            onTriggered: {
                if (pendingCenterIndex >= 0) {
                    listView.currentIndex = pendingCenterIndex
                    pendingCenterIndex = -1
                }
            }
        }

        Timer {
            id: restoreFollowTimer
            interval: lyricItemRoot.delayFollow
            repeat: false
            onTriggered: {
                lyricItemRoot.isFollow = true
                if (currentLineIndex >= 0) {
                    pendingCenterIndex = currentLineIndex;
                    centerDebounce.restart();
                }
            }
        }
    }
}