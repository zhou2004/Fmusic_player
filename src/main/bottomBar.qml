import QtQuick 2.15
import FluentUI 1.0
import "../components"

Rectangle {
    property int bottomBarHeight
    property int bottomBarWidth
    property int infoTextSize: 15

    id: bottomBar
    width: bottomBarWidth
    height: bottomBarHeight
    border.color: FluTheme.dividerColor
    color: {
        if(Window.active){
            return FluTheme.frameActiveColor
        }
        return FluTheme.frameColor
    }
    z:10

    // now playing
    Item {
        id: playInfo
        height: parent.height
        width: parent.width * 0.3
        anchors.left: parent.left

        Row {
            width: parent.width
            height: parent.height

            // Album picture
            RoundImage {
                id: albumPicture
                width: parent.height
                height: parent.height
                source: "qrc:/assets/defaultAlbum.jpg"

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        detailPage.visible = !detailPage.visible
                        console.log("onClicked")
                    }
                    onEntered: parent.showColorOverlay = true
                    onExited: parent.showColorOverlay = false
                }
            }

            // song information
            Column {
                spacing: 4
                anchors.verticalCenter: parent.verticalCenter
                width: 150
                height: implicitHeight

                // song name
                SrollText {
                    id: songName
                    width: parent.width
                    size: infoTextSize
                    height: 20
                    text: "songName"
                }

                Text {
                    id: albumName
                    width: parent.width
                    font.pointSize: infoTextSize
                    height: 20
                    text: "albumName"
                }
            }
        }
    }

    //play control
    Item {
        id: playControl
        height: parent.height
        width: parent.width * 0.4
        anchors.horizontalCenter: parent.horizontalCenter

        Column {
            anchors.centerIn: parent
            width: parent.width
            height: parent.height

            Item {
                height: parent.height * 0.5
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                property int topMargin: Math.ceil(parent.height * 0.25)

                FluIconButton {
                    id: pre_buttion
                    iconSource: FluentIcons.Previous
                    iconSize: 20
                    anchors.top:parent.top
                    anchors.topMargin: parent.topMargin+3
                    anchors.right: musicPlay_buttion.left
                    anchors.rightMargin: 10
                }

                FluIconButton {
                    id: musicPlay_buttion
                    iconSource: FluentIcons.Play
                    iconSize: 25
                    anchors.top:parent.top
                    anchors.topMargin: parent.topMargin
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                FluIconButton {
                    id: next_buttion
                    iconSource: FluentIcons.Next
                    iconSize: 20
                    anchors.top:parent.top
                    anchors.topMargin: parent.topMargin+3
                    anchors.left: musicPlay_buttion.right
                    anchors.leftMargin: 10
                }
            }

            Row {
                height: parent.height * 0.5
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                FluSlider {
                    id: slider
                    width: parent.width - musicDuration.width
                    height: 10
                    orientation: Qt.Horizontal
                    anchors.verticalCenter: parent.verticalCenter

                    property bool isDragging: false
                    from: 0
                    to: 100
                }

                Text {
                    id: musicDuration
                    text: "100"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    Item {
        id: musicFunction
        height: parent.height
        width: parent.width * 0.3
        anchors.right: parent.right
    }
}
