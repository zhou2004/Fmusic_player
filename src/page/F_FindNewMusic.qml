import QtQuick 2.15
import FluentUI 1.0
import "../components"
Item {
    id: newMusicContent
    property var musicData: musicPlayer.Get_All_Music()
    property color font_color: "black"
    property var headerData: [{name:"全部",type:"0"},
        {name:"华语",type:"7"},
        {name:"欧美",type:"96"},
        {name:"日本",type:"8"},
        {name:"韩国",type:"16"},]
    property double fontSize: 11
    property int headerCuerrent: 0
    property var music_list: [{}]  // 音乐列表
    width: parent.width
    height: parent.height

    property color rec_color: "#FF99E6F9"   //浅蓝色（矩形）
    property color tra_color: "#FF1DCCFF" //深蓝色（矩形）

    function update_music_list() {
        if(headerCuerrent === 0) {
            newMusicContent.musicData = musicPlayer.Get_All_Music()
            // 获取全部音乐
        }else if(headerCuerrent === 1) {
            newMusicContent.musicData = musicPlayer.Get_Music_Type("华语")
            // 获取华语音乐

        }else if(headerCuerrent === 2) {
            newMusicContent.musicData = musicPlayer.Get_Music_Type("欧美")
            // 获取欧美音乐

        }else if(headerCuerrent === 3) {
            newMusicContent.musicData = musicPlayer.Get_Music_Type("日语")
            // 获取日本音乐
        }else {
            newMusicContent.musicData = musicPlayer.Get_Music_Type("韩语")
            // 获取韩国音乐
        }
    }

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
                model.append(newMusicContent.headerData)
                update_music_list()
            }
        }
        Component {
            id: headerDelegate
            Text {
                property bool isHoverd: false
                font.bold: isHoverd || index === newMusicContent.headerCuerrent
                font.pointSize: newMusicContent.fontSize
                text: name
                color: font_color
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        newMusicContent.headerCuerrent = index
                        console.log(newMusicContent.headerCuerrent)
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

    Rectangle {
        id: content
        anchors.top: header.bottom
        anchors.topMargin: 20
        width: parent.width * 0.9
        height: parent.height
        radius: 5
        anchors.horizontalCenter: parent.horizontalCenter
        color: rec_color

        Flickable {
            id: flickable
            anchors.fill: parent
            contentHeight: column.height + 60 // 动态调整内容高度
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Column {
                id: column
                width: parent.width
                spacing: 10

                Repeater {
                    model: newMusicContent.musicData
                    delegate: contentDelegate
                }
            }
        }

        Component {
            id: contentDelegate

            Rectangle {
                color: "#FFA5DBFF"
                radius: 5
                property bool isHoverd: false
                width: content.width - 20
                height: 60

                anchors.horizontalCenter: parent.horizontalCenter

                Row {
                    width: parent.width - 20
                    height: parent.height - 20
                    anchors.verticalCenter: parent.verticalCenter

                    RoundImage {
                        id: albumImage
                        width: parent.height
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        radius: 5
                        source: modelData.cover_url

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("onClickedpos")
                            }
                            onEntered: {}
                            onExited: {}
                        }
                    }

                    Text {
                        id: music_name
                        width: parent.width / 3
                        anchors.left: albumImage.right
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.weight: 2
                        font.pointSize: newMusicContent.fontSize
                        elide: Text.ElideRight
                        text: modelData.music_name
                        color: font_color
                    }

                    Text {
                        id: artist_name
                        width: parent.width / 5
                        anchors.left: music_name.right
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.weight: 2
                        font.pointSize: newMusicContent.fontSize
                        elide: Text.ElideRight
                        text: modelData.art_name
                        color: font_color
                    }

                    Text {
                        id: album_name
                        width: parent.width / 5
                        anchors.left: artist_name.right
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.weight: 2
                        font.pointSize: newMusicContent.fontSize
                        elide: Text.ElideRight
                        text: modelData.album
                        color: font_color
                    }

                    Text {
                        id: music_duration
                        width: parent.width / 6
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignRight
                        font.weight: 2
                        font.pointSize: newMusicContent.fontSize
                        elide: Text.ElideRight
                        text: modelData.duration
                        color: font_color
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        musicPlayer.play(modelData.play_url)
                        albumartManager.updateAlbumArt()
                        lyrics.parseLyric(modelData.lyric_url)
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


}