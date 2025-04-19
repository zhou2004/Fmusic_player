import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
import QtMultimedia
import Qt.labs.platform 1.1
import "../components"



// 歌单详情页（覆盖层）- 精准修改版
Rectangle {
    id: detailPage
    width: parent.width
    height: parent.height
    visible: parent.visible
    property var music_detail: mainWindow.music_detail
    property var m_row: 0
    // 新增组件索引（最小化修改）
    property var _charMap: ({})
    z: 1


    function resetLyrics() {
        console.log("resetLyrics");
        _charMap = ({});
        m_row = 0;
        // lyricList.positionViewAtIndex(0, ListView.Beginning);
        // for (var i = 0; i < currentLyrics.length; i++) {
        //     for (var j = 0; j < currentLyrics[i].length; j++) {
        //         if (_charMap[i] && _charMap[i][j]) {
        //             _charMap[i][j].color = "black";
        //         }
        //     }
        // }
    }

    // 保持您原有的视图定位函数
    function positonView(pos) {
        lyricList.positionViewAtIndex(pos,  ListView.Center);
    }


    function _registerChar(lineIdx, charIdx, item) {
        if (!_charMap[lineIdx]) _charMap[lineIdx] = {};
        _charMap[lineIdx][charIdx] = item;
    }

    // 新增染色方法（不改变现有结构）
    function updateHighlight(row, col, percent) {
        if (!_charMap[row]?.[col]) return;

        // 清除旧行高亮
        if (m_row != row) {
            for (var i in _charMap[m_row]) {
                _charMap[m_row][i].highlightRatio = 0;
            }
        }

        // 设置当前行高亮
        for (var i = 0; i < col; i++) {
            _charMap[row][i].highlightRatio = 1.0;  // 前序字全亮
        }
        _charMap[row][col].highlightRatio = percent;  // 当前字按比例分色
        m_row = row;
    }

    // 完全保留您原有的可视化层
    Rectangle {
        width: parent.width
        height: parent.height
        visible: parent.visible
        color: mainWindow.rec_color
        z: 1

        // Krc歌词
        ListView {
            id: lyricList
            anchors.fill:  parent
            model: music_detail
            spacing: 10
            clip: true

            delegate: Rectangle {
                id: lineDelegate
                width: parent.width
                height: 40
                color: mainWindow.rec_color
                property int rowIndex: index

                Row {
                    id: innerRow
                    height: parent.height
                    anchors.horizontalCenter:  parent.horizontalCenter

                    Repeater {
                        model: modelData
                        delegate: Rectangle {
                            id: colDelegate
                            width: textContainer.width   // 自动适应文本宽度
                            height: parent.height
                            color: "transparent"
                            property int colIndex: index

                            // 文本容器（用于计算总宽度）
                            Item {
                                id: textContainer
                                width: textItem.implicitWidth
                                height: parent.height
                                anchors.centerIn:  parent

                                // 左半部分（高亮色）
                                Text {
                                    id: leftPart
                                    z:1
                                    text: modelData.content
                                    font: textItem.font
                                    anchors.left:  parent.left
                                    width: parent.width  * highlightRatio  // 动态裁剪宽度
                                    clip: true  // 关键：裁剪超出部分
                                    color: mainWindow.lyric_color
                                }

                                // 右半部分（黑色）
                                Text {
                                    id: rightPart
                                    text: modelData.content
                                    font: textItem.font
                                    anchors.right:  parent.right
                                    width: parent.width  // 剩余宽度
                                    clip: true
                                    color: "black"
                                }
                            }

                            // 对外暴露的Text（仅用于注册和尺寸计算）
                            Text {
                                id: textItem
                                text: modelData.content
                                visible: false  // 隐藏但保留尺寸计算
                                font.pixelSize:  16
                            }

                            property real highlightRatio: 0
                            Component.onCompleted:  detailPage._registerChar(lineDelegate.rowIndex,  colDelegate.colIndex,  this)
                        }
                    }
                }
            }
        }


    }
}
