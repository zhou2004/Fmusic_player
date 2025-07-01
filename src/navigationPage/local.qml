import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import "../components"

FluScrollablePage {
    title: qsTr("Local")
    contentWidth: parent.width
    id: localPage

    ColumnLayout {
        id: localPageContent
        spacing: 10
        width: parent.width * 0.8

        RowLayout {
            id: localControlButton
            spacing: 10

            FluButton {
                id: localOpenButton
                text: qsTr("Open")
                onClicked: {

                }
            }
            FluButton {
                id: localPlayAllButton
                text: qsTr("PlayAll")
                onClicked: {

                }
            }
        }

        FluPivot {
            id: localPivot
            Layout.fillWidth: true
            Layout.fillHeight: true

            FluPivotItem {
                title: qsTr("Music")
                contentItem: ListView {
                    id: musicListView
                    topMargin: 20

                    //在这里传入音乐数据，使用C++传入数据时请参考ListView的文档
                    model: ListModel {
                        ListElement {
                            index: 0
                            cover: "qrc:/assets/defaultAlbum.jpg"
                            title: "A.I.N.Y. 爱你 (G.E.M.重生版)"
                            artist: "G.E.M.邓紫棋"
                            album: "I AM GLORIA"
                        }
                        ListElement {
                            index: 1
                            cover: "qrc:/assets/defaultAlbum.jpg"
                            title: "杜鹃"
                            artist: "Realzat / 加木"
                            album: "杜鹃"
                        }
                    }
                    delegate: MusicInfo {
                        mIndex: index
                        mCover: cover
                        mTitle: title
                        mArtist: artist
                        mAlbum: album
                    }
                }
            }
            FluPivotItem {
                title: qsTr("Album")
                contentItem: FluText {
                    text: qsTr("Unread emails go here.")
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

}
