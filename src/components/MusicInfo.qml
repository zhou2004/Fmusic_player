import QtQuick 2.15
import FluentUI 1.0

Rectangle {
    property int mIndex: 0
    property url mCover: ""
    property string mTitle: ""
    property string mArtist: ""
    property string mAlbum: ""

    id: musicInfoRectangle
    width: parent ? parent.width : 600
    height: 64
    color: "transparent" // 设置为透明，背景颜色由鼠标悬停时改变

    // 鼠标悬停时改变背景颜色

    // 19337196548

    // 左侧：序号、封面、标题
    Row {
        id: musicInfoRow
        spacing: 12
        height: parent.height
        width: parent.width * 0.4

        FluText {
            text: (mIndex + 1).toString()
            id: musicInfoIndex
            font.pixelSize: 20
            width: 32
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
        }
        FluImage {
            source: mCover
            id: musicInfoCover
            width: 48
            height: 48
            anchors.verticalCenter: parent.verticalCenter
        }
        FluText {
            text: mTitle
            id: musicInfoTitle
            font.bold: true
            font.pixelSize: 18
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // 中间：艺术家和专辑信息

    Rectangle {
        id: musicInfoDetailRectangle
        color: {
            if(Window.active){
                return FluTheme.windowActiveBackgroundColor
            }
            return FluTheme.windowBackgroundColor
        }
        height: parent.height
        width: parent.width * 0.4
        anchors.left: musicInfoRow.right



        Row {
            id: musicInfoDetailRow
            width: parent.width
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter

            Item { width: 15 ; height: parent.height } // 占位符，用于左侧间距

            FluText {
                text: mArtist
                id: musicInfoArtist
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }

            Item { width: 25 ; height: parent.height } // 占位符，用于中间间距

            FluText {
                text: mAlbum
                id: musicInfoAlbum
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }

        }

    }

    // 最右侧按钮
    FluIconButton {
        id: musicInfoMoreButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        iconSource: FluentIcons.More
    }
}