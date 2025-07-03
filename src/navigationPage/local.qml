import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import "../components"

ColumnLayout {
    id: localPageContent
    spacing: 10
    width: parent.width * 0.8

    FluText {
        id: localTitle
        text: qsTr("Local Music")
        font.pixelSize: 24
        Layout.fillWidth: true
    }

    RowLayout {
        id: localControlButton
        spacing: 10

        FluButton {
            id: localOpenButton
            text: qsTr("Open")
            onClicked: {
                //TODO: 打开本地音乐文件夹
            }
        }
        FluButton {
            id: localPlayAllButton
            text: qsTr("PlayAll")
            onClicked: {
                //TODO: 播放本地全部音乐
            }
        }
    }

    FluPivot {
        id: localPivot
        Layout.fillWidth: true
        Layout.fillHeight: true

        FluPivotItem {
            title: qsTr("Music")
            contentItem: FluScrollablePage {
                id: localMusicPage
                contentWidth: parent.width

                Column {
                    id: localMusicContent
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    
                    Item { width:parent.width ;height: 10 } // 占位符，用于顶部间距

                    Loader {
                        id: demoData
                        source: "DemoData.qml"
                        asynchronous: true
                    }

                    Repeater {
                        // TODO:在这里传入音乐数据，使用C++传入数据时请参考ListView的文档
                        model: demoData.item ? demoData.item.demoMusicData : null
                        delegate: MusicInfo {
                            mIndex: index
                            mCover: cover
                            mTitle: title
                            mArtist: artist
                            mAlbum: album
                        }
                    }


                }

            }
        }


        FluPivotItem {
            title: qsTr("Album")
            contentItem: FluScrollablePage {
                contentWidth: parent.width

                Rectangle {
                    width: parent.width
                    height: parent.height
                    color: "red"

                }
        }
    }
        FluPivotItem {
            title: qsTr("Singer")
            contentItem: FluText {
                text: qsTr("Flagged emails go here.")
            }
        }
    }
}
