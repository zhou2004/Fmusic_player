// language: qmllang
import QtQuick 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import "../"

Item {
    id: root
    width: parent.width
    height: parent.height
    property string title: ""
    property string artist: ""
    property string album: ""
    property url cover: ""

    Column {
        // anchors.fill: parent
        spacing: 24
        anchors.centerIn: parent

        RoundImage {
            source: root.cover
            width: root.width * 0.7
            height: width
            radius: 16
        }

        // Rectangle {
        //     width: root.width * 0.7
        //     height: width
        //     radius: 16
        //     color: "#202020"
        //     clip: true   // 按圆角裁剪内部 Image
        //
        //     Image {
        //         anchors.fill: parent
        //         anchors.margins: 4
        //         source: root.cover
        //         fillMode: Image.PreserveAspectFit
        //         smooth: true
        //     }
        // }

        Column {
            width: root.width * 0.7
            spacing: 6

            Text {
                text: root.title
                color: "white"
                font.pixelSize: root.width * 0.06
                font.bold: true
                elide: Text.ElideRight
                wrapMode: Text.WrapAnywhere
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                text: root.artist
                color: "#DDDDDD"
                font.pixelSize: root.width * 0.05
                elide: Text.ElideRight
                wrapMode: Text.WrapAnywhere
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                text: root.album
                color: "#AAAAAA"
                font.pixelSize: root.width * 0.05
                elide: Text.ElideRight
                wrapMode: Text.WrapAnywhere
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
