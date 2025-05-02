import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects
Flickable {
    id: findMusicFlickable
    anchors.centerIn:parent
    property var headerData: [{headerText:"歌单",qml:"qrc:/page/F_FindMusicPlayList.qml"},
        {headerText:"最新音乐",qml:"qrc:/page/F_FindNewMusic.qml"},
        // {headerText:"个性推荐",qml:""},
        // {headerText:"专属定制",qml:""}
    ]
    property double fontSize: 11

    property var find_map: []

    property int headerCuerrent: 0

    property color rec_color: "#FF99E6F9"   //浅蓝色（矩形）
    property color tra_color: "#FF1DCCFF" //深蓝色（矩形）

    property color font_color: "blue"
    property color origin_color: "black"

    function find_init(index, Item) {
        find_map[index]= Item;

    }

    anchors.fill:parent

    Rectangle {
        id: findMusicHeader
        property int current: 0
        property double topBottomPadding: 25
        property double leftRightPadding: 35
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 25
        radius: 8
        color: rec_color
        Row {
            Repeater {
                model: ListModel{}
                delegate: findMusicHeaderDelegate
                Component.onCompleted: {
                    model.append(findMusicFlickable.headerData)
                }
                onCountChanged: {
                    var w =0
                    var h =0
                    for(var i = 0; i < count; i++) {
                        w += itemAt(i).width
                        if(h < itemAt(i).height) {
                            h = itemAt(i).height
                        }
                        findMusicHeader.width = w
                        findMusicHeader.height = h
                    }
                }
            }
        }
        Component {
            id: findMusicHeaderDelegate
            Rectangle {
                property bool isHovered: false
                width: children[0].contentWidth + findMusicHeader.leftRightPadding
                height:children[0].contentHeight + findMusicHeader.topBottomPadding
                radius:width/2
                color: index === findMusicHeader.current ? tra_color : rec_color

                Text {
                    anchors.centerIn: parent
                    font.pointSize: findMusicFlickable.fontSize
                    text: headerText

                    Component.onCompleted: {
                        if(index === 0) {
                            color = font_color
                        }
                        findMusicFlickable.find_init(index, this)
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            findMusicFlickable.find_map[findMusicHeader.current].color=findMusicFlickable.origin_color
                            findMusicHeader.current = index
                            parent.color = findMusicFlickable.font_color // 设置阴影颜色
                        }

                        // 当鼠标悬停在按钮上时，改变背景颜色以显示阴影
                        onEntered: {
                            parent.color = findMusicFlickable.font_color // 设置阴影颜色
                        }
                        onExited: {
                            if(index!==findMusicHeader.current){
                            parent.color = findMusicFlickable.origin_color // 恢复原始背景颜色
                            }
                        }
                    }
                }
            }
        }
    }

    Loader {
        id: findMusicContent
        width:parent.width
        height:parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.top: findMusicHeader.top
        anchors.topMargin: 25
        source: findMusicFlickable.headerData[findMusicHeader.current].qml
        onStatusChanged: {
            if(status === Loader.Ready) {

            }
        }

    }

}