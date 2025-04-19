import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import QtQuick.Layouts 1.15
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
    z: 1

    // 保持您原有的视图定位函数
    function positonView(pos) {
        lyricList.positionViewAtIndex(pos,  ListView.Center);
    }

    // 新增组件索引（最小化修改）
    property var _charMap: ({})
    function _registerChar(lineIdx, charIdx, item) {
        if (!_charMap[lineIdx]) _charMap[lineIdx] = {};
        _charMap[lineIdx][charIdx] = item;
    }

    // 新增染色方法（不改变现有结构）
    function updateHighlight(row, col, percent) {
        if (!_charMap[row]?.[col]) console.log("not found!");
        if (m_row != row) {
            for(var i in _charMap[m_row]) _charMap[m_row][i].color = "black";
        }
        for (var i = 0; i <= col; i++) {
            _charMap[row][i].color  = "red";
            m_row = row;
        }
        // _charMap[row][col].opacity = percent;
    }

    // 完全保留您原有的可视化层
    Rectangle {
        width: parent.width
        height: parent.height
        visible: parent.visible
        color: mainWindow.rec_color
        z: 1


        // LRC歌词

        ListView {
            id: lyricList
            anchors.fill:  parent
            model: music_detail
            spacing: 10
            clip: true



            // 将视图定位到第 n 行
            function scrollToIndex(n) {
                if (n >= 0 && n < model.count)  {
                    positionViewAtIndex(n, ListView.Center);
                }
            }

            delegate: Rectangle {
                width: parent.width
                height: 40
                color: mainWindow.rec_color

                Text {
                    id: lyricText
                    text: modelData.content
                    anchors.centerIn:  parent
                    wrapMode: Text.Wrap
                    font.pixelSize:  16
                    color: index === pos ? mainWindow.lyric_color : "black" // 如果当前行是第 n 行，则设置为红色
                }
            }
        }
    }
}
// LRC歌词

// ListView {
//     id: lyricList
//     anchors.fill:  parent
//     model: music_detail
//     spacing: 10
//     clip: true
//
//     // 垂直滚动条
//     ScrollBar.vertical:  ScrollBar {
//         active: true
//         policy: ScrollBar.AsNeeded
//     }
//
//     // 将视图定位到第 n 行
//     function scrollToIndex(n) {
//         if (n >= 0 && n < model.count)  {
//             positionViewAtIndex(n, ListView.Center);
//         }
//     }
//
//     delegate: Rectangle {
//         width: parent.width
//         height: 40
//
//         Text {
//             id: lyricText
//             text: modelData.content
//             anchors.centerIn:  parent
//             wrapMode: Text.Wrap
//             font.pixelSize:  16
//             color: index === pos ? "red" : "black" // 如果当前行是第 n 行，则设置为红色
//         }
//     }
// }