import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import "../components"
import QtQml.Models 2.15

ColumnLayout {
    id: localPageContent
    spacing: 10
    // 注意：建议这里不要写死 width 为 parent.width * 0.8
    // 如果父级已经有布局管理，Layout.fillWidth: true 会更好
    Layout.fillWidth: true
    Layout.fillHeight: true

    // 在这里设置全局左边距
    property int leftMarginSet: 10

    // 初始化为空数组，用来接收 C++ 的 QVariantList
    property var musicModel: []

    // 分页相关属性
    property int pageSize: 20
    property int currentPage: 0    // 从 0 开始
    property int totalCount: 0
    property int totalPage: 0

    function refreshPage() {
        totalCount = musicPlayer.localTracksCount()
        totalPage = totalCount === 0 ? 0 : Math.ceil(totalCount / pageSize)

        if (currentPage < 0)
            currentPage = 0
        if (currentPage >= totalPage)
            currentPage = Math.max(totalPage - 1, 0)

        var offset = currentPage * pageSize
        musicModel = musicPlayer.localTracksPage(offset, pageSize)
        console.log("刷新本地音乐分页: page", currentPage, "size", pageSize,
            "总数:", totalCount, "总页数:", totalPage)
    }

    // 监听 C++ 信号，实现响应式更新
    Connections {
        target: musicPlayer
        // function onLocalTracksChanged() {
        //     // 当 C++ 扫描完成后，手动更新 QML 属性
        //     localPageContent.musicModel = musicPlayer.localTracks()
        //     console.log("本地音乐已更新，数量:", localPageContent.musicModel.length)
        // }
        function onLocalTracksChanged() {
            currentPage = 0
            refreshPage()
        }
    }

    FluText {
        id: localTitle
        text: qsTr("Local Music")
        font.pixelSize: 24
        font.bold: true
        Layout.fillWidth: true
        Layout.leftMargin: leftMarginSet
        Layout.topMargin: 20 // 增加一点顶部边距更美观
    }

    RowLayout {
        id: localControlButton
        spacing: 10
        Layout.leftMargin: leftMarginSet

        FluButton {
            id: localOpenButton
            text: qsTr("Open")
            onClicked: {
                musicPlayer.openLocalMusicFolder()
            }
        }
        FluButton {
            id: localPlayAllButton
            text: qsTr("PlayAll")
            onClicked: {
                // musicModel = musicPlayer.localTracks()
                // console.log(musicModel[0].title)
                musicPlayer.setTrackList(musicModel)
                musicPlayer.playIndex(0)
            }
        }

        // 上一页
        FluButton {
            text: qsTr("Prev")
            enabled: currentPage > 0
            onClicked: {
                currentPage--
                refreshPage()
            }
        }

        // 下一页
        FluButton {
            text: qsTr("Next")
            enabled: currentPage + 1 < totalPage
            onClicked: {
                currentPage++
                refreshPage()
            }
        }

        FluText {
            text: (totalPage > 0 ? (currentPage + 1) : 0) + "/" + totalPage
        }
    }

    Loader {
        id: demoData
        source: "DemoData.qml"
        asynchronous: true
    }

    FluPivot {
        id: localPivot
        Layout.fillWidth: true
        Layout.fillHeight: true // 确保 Pivot 占满剩余空间
        Layout.leftMargin: leftMarginSet

        // 本地音乐，通过 localOpenButton 打开本地音乐文件夹并加载
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

                    Repeater {
                        // TODO:在这里传入音乐数据，使用C++传入数据时请参考ListView的文档
                        model: musicModel
                        delegate: MusicInfo {
                            mIndex: index
                            mCover: modelData.cover
                            mTitle: modelData.title
                            mArtist: modelData.artist
                            mAlbum: modelData.album
                        }
                    }
                }
            }
        }

        // 本地专辑
        FluPivotItem {
            title: qsTr("Album")
            contentItem: FluScrollablePage {
                id: localAlbumPage
                contentWidth: parent.width

                Column {
                    id: localAlbumContent
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item { width:parent.width ;height: 10 } // 占位符，用于顶部间距

                    Repeater {

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
            title: qsTr("Singer")
            contentItem: FluScrollablePage {
                id: localSingerPage
                contentWidth: parent.width

                Column {
                    id: localSingerContent
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item { width:parent.width ;height: 10 } // 占位符，用于顶部间距

                    Repeater {

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
    }
}