import QtQuick 2.15
import FluentUI 1.0
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts 1.15
import "../components"
Item {
    id: playListContent
    property color font_color: "black"
    property var headerData: [{name:"全部",type:"0"},
        {name:"华语",type:"7"},
        {name:"欧美",type:"96"},
        {name:"日本",type:"8"},
        {name:"韩国",type:"16"},]
    property double fontSize: 11
    property int headerCuerrent: 0
    property var playlistDatas: musicPlayer.Get_Playlist()  // 音乐列表
    width: parent.width
    height: parent.height

    property color rec_color: "#FF99E6F9"   //浅蓝色（矩形）
    property color tra_color: "#FF1DCCFF" //深蓝色（矩形）



    Row {
        id: header
        spacing:10
        width: parent.width
        height: 20
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left:content.left
        Repeater {
            model: ListModel{}
            delegate: headerDelegate
            Component.onCompleted: {
                model.append(playListContent.headerData)
                update_music_list()
            }
        }
        Component {
            id: headerDelegate
            Text {
                property bool isHoverd: false
                font.bold: isHoverd || index === playListContent.headerCuerrent
                font.pointSize: playListContent.fontSize
                text: name
                color: font_color
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        playListContent.headerCuerrent = index
                        console.log(playListContent.headerCuerrent)
                        update_music_list()
                    }
                    onEntered: {
                        parent.isHoverd = true
                    }
                    onExited: {
                        parent.isHoverd = false
                    }
                }
            }
        }
    }


    // 容器区域（可动态调整大小）
    Rectangle {
        id: container
        anchors.top: header.bottom
        anchors.topMargin: 20
        width: parent.width * 0.98
        height: parent.height
        radius: 5
        anchors.horizontalCenter: parent.horizontalCenter
        color: rec_color


        Flickable {
            id: flickable
            anchors.fill: parent
            contentHeight: flowLayout.childrenRect.height  + 60 // 动态调整内容高度
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            // 使用 Flow 布局自动换行
            Flow {
                id: flowLayout
                anchors.fill:  parent
                anchors.leftMargin: 2
                spacing: 15 // 矩形间无间距

                // 动态创建矩形项
                Repeater {
                    model: playListContent.playlistDatas // 100个矩形

                    delegate: contentDelegate


                }
                Component {
                    id: contentDelegate
                    Rectangle {
                        width: parent.width * 0.17 > 170 ? 150 : parent.width * 0.17// 矩形宽度
                        height: width + 20 // 矩形高度
                        radius: 5
                        RoundImage {
                            id: roundImage
                            anchors.top: parent.top
                            width: parent.width
                            height: parent.width
                            source: modelData.cover_url
                            masksource: FluentIcons.Play
                            colorOverlayColor: "#80000000"
                            radius: 5

                            // 改变图标颜色
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked: {
                                    rightpageLoader.source = "qrc:/page/F_PlayList_detail.qml"
                                    rightpageLoader.item.play_listContent = modelData
                                }
                                // 当鼠标悬停在按钮上时，改变背景颜色以显示阴影
                                onEntered: {
                                    console.log("onEntered")
                                    parent.showColorOverlay = true // 设置阴影颜色
                                }
                                onExited: {
                                    parent.showColorOverlay = false // 恢复原始背景颜色
                                }
                            }
                        }

                        color: mainWindow.rec_color
                        // border { color: "black"; width: 1 }

                        // 显示矩形索引（可选）
                        Text {
                            anchors.bottom:  parent.bottom
                            anchors.horizontalCenter:  parent.horizontalCenter
                            font.pixelSize:  14
                            text: modelData.playlist_name
                        }
                    }
                }
            }
        }



    }





    // Item {
    //     id: header
    //     width: parent.width
    //     height: headerBackground.height
    //     Item {
    //         id: headerBackground
    //         width: parent.width
    //         height: 160
    //         RoundImage {
    //             id: headerBackground_1
    //             anchors.fill: parent
    //             width:40
    //             height:50
    //             source: "qrc:/Images/Splash_Screen_First_Closed_Beta.png"
    //         }
    //
    //         RoundImage {
    //             id: headerBackground_2
    //             anchors.fill: parent
    //             width:40
    //             height:50
    //             source: headerBackground_1.source
    //         }
    //
    //         FastBlur {  //边缘淡发光
    //             anchors.fill: parent
    //             radius: 30
    //             source: headerBackground_1
    //             transparentBorder:true
    //         }
    //
    //         FastBlur {  //模糊背景
    //             anchors.fill: parent
    //             radius: 80
    //             source: headerBackground_2
    //             transparentBorder:true
    //         }
    //     }
    // }
    //
    // Item {
    //     id: content
    // }
    // Text {
    //     text: "Find Music Play List"
    // }
}