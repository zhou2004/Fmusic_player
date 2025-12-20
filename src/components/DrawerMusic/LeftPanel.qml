// language: qmllang
import QtQuick 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0

Item {
    id: root
    property string title: ""
    property string artist: ""
    property string album: ""
    property url cover: ""

    Column {
        anchors.fill: parent
        spacing: 24
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            width: parent.width * 0.9
            height: width
            radius: 16
            color: "#202020"
            clip: true   // 按圆角裁剪内部 Image

            Image {
                anchors.fill: parent
                anchors.margins: 4
                source: root.cover
                fillMode: Image.PreserveAspectFit
                smooth: true
            }
        }

        Column {
            width: parent.width * 0.9
            spacing: 6

            Text {
                text: root.title
                color: "white"
                font.pixelSize: 20
                font.bold: true
                elide: Text.ElideRight
            }

            Text {
                text: root.artist
                color: "#DDDDDD"
                font.pixelSize: 14
                elide: Text.ElideRight
            }

            Text {
                text: root.album
                color: "#AAAAAA"
                font.pixelSize: 13
                elide: Text.ElideRight
            }
        }
    }
}
