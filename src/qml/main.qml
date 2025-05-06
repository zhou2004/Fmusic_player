import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import QtQuick.Layouts 1.15
import QtMultimedia
import Qt.labs.platform 1.1
import CustomTypes 1.0
import "../components"
import "../page"
import "../Navigation"

FluWindow {
    id: mainWindow
    width: 1010
    height: 710
    minimumWidth: 600
    minimumHeight: 400
    launchMode: FluWindowType.SingleTask
    fitsAppBarWindows: true
    opacity: 1 // 设置窗口整体透明度（可选）
    // title: qsTr("Fmusic")
    // fixSize: false
    // showStayTop: false
    // showDark: true
    // showMinimize:false
    // showMaximize:false
    // showClose:true
    // fitsAppBarWindows: true


    // appbar
    appBar: FluAppBar {
        id: appBars
        height: 30
        showDark: true
        showStayTop: false
        z:7
    }

    flags: {
        if (Qt.platform.os  === "windows") {
            return Qt.Window; // 在 windows 平台下关闭无边框
        } else {
            return Qt.FramelessWindowHint; // 在 其他 平台下开启无边框
        }
    }

    //window blur effect
    //TODO: set on only on windows platform
    Component.onCompleted: {
        FluTheme.blurBehindWindowEnabled = true
        mainWindow.effect = "dwm-blur"
        mainWindow.tintOpacity = 0.6
        mainWindow.blurRadius = 30
    }

    property var dataPool: []
    property var availableItems: []
    readonly property bool isEmpty: availableItems.length  === 0

    // 初始化数据池（0-100）
    function initPool(n) {
        dataPool = Array.from({length:  n}, (_,i) => i)
        reset()
    }

    // 重置可用项
    function reset() {
        availableItems = [...dataPool] // 深拷贝
    }

    // 不放回随机选择
    function selectRandom() {
        if (isEmpty) {
            console.log(" 数据已抽完，正在重置...")
            reset()
        }

        const randomIndex = Math.floor(Math.random()  * availableItems.length)
        const selected = availableItems[randomIndex]
        availableItems.splice(randomIndex,  1) // 移除已选项

        return selected
    }

    onMusic_listChanged: {
        initPool(music_list.length)
    }


    property int request_total: 50 //请求的音乐总数
    property string defaltFolderUrl: ""     // 对话框目录路径
    property string music_folder: ""  //音乐路径
    property int play_styles: 0 //播放方式
    // 使用单例选择器
    property int currentNumber: -1
    property int duration: 0
    onDurationChanged: {
        if(duration == music_info.duration) {
            // next_buttion.clicked()
            if(play_styles == 0) {
                next_buttion.clicked()
            }else if(play_styles == 1) {
                currentNumber = selectRandom()
                console.log("currentNum: ",currentNumber)
                //更新对应的音乐专辑封面
                musicPlayer.play(music_list[currentNumber].play_url)
                albumartManager.updateAlbumArt()
                lyrics.parseLyric(music_list[currentNumber].lyric_url)

            } else {

                musicPlayer.repeat_play(music_list[current_task_index].play_url)

            }

        }
    }

    property UserFullInfo user //用户基本信息


    property var search_keyword: ""

    property int current_task_index : -1

    property var music_list: []   //音乐列表数据

    property var ai_music_list: "" //ai推荐音乐列表

    property var music_detail: ""   //音乐歌词详情/列表

    property var music_info: "0"   //音乐信息

    property var albumart : "qrc:/logo.ico"  //专辑封面


    property int pos: 10 // 示例值，可以动态修改

    property var krc_pos: ""   //歌词位置信息

    property bool isSpectrumVisible: false


    property color rec_color: "#FF99E6F9"   //浅蓝色（矩形）
    property color tra_color: "#FF1DCCFF" //深蓝色（矩形）
    property color ri_color: "#FF97E2F4" //浅浅蓝色（矩形）

    property color lyric_color: "#FFFF3E84" //樱花红


    property int headerCuerrent: 0

    property var lyric_type: ""  //歌词类型

    Connections {
        target: musicPlayer
        onMetaDataChanged: {
            music_info = musicPlayer.getMusicInfo()
            // albumartManager.updateAlbumArt()
            console.log("Duration:", music_info)

        }
        onPositionChanged: {

            duration = musicPlayer.Get_Q_Position()

            //Lrc
            if(lyric_type == "Lrc") {
                pos = lyrics.get_Lyricspos(duration)
                detailPage.item.positonView(pos);
            }else {
                // Krc
                krc_pos = lyrics.get_Lyricspos(duration)
                detailPage.item.positonView(krc_pos.lineIndex)
                detailPage.item.updateHighlight(krc_pos.lineIndex, krc_pos.lyricIndex, krc_pos.percent)
            }

        }

        onMusicfolderChanged: {
            music_list = musicPlayer.get_all_music_path()
            request_total = musicPlayer.Get_Q_request_total()
        }

        onMusicInfoChanged: {
            music_list = musicPlayer.Get_Q_refresh_all_music()
        }

        onPlayingChanged : {
            if (playing) {
                musicPlay_buttion.iconSource = FluentIcons.Pause
            } else {
                musicPlay_buttion.iconSource = FluentIcons.Play
            }
        }

        onUserChanged: {
            user = c_user
            showSuccess(qsTr("登录成功"))
            console.log(" 用户名: ", user.user_avatar_url)
            // titleBar.user_avatar_url= user.user_avatar_url
            // aboutPage.username = user.nickname
        }


    }

    Connections {
        target: albumartManager
        onAlbumArtDataChanged: {
            albumart = "data:image/png;base64," + albumartManager.base64Data
        }
    }

    Connections {
        target: lyrics
        onLyricChanged: {

            music_detail = lyrics.Q_getLyrics()
            console.log("Lyric:更新了")
            // console.log(music_detail[0].content)
        }

        onLyrictypeChanged: {
            // detailPage.item.resetLyrics()
            lyric_type = type
            if(lyric_type == "Lrc") {
                detailPage.source = "qrc:/page/Music_detail_lrc.qml"
            }else {
                // detailPage.item.resetLyrics()
                detailPage.source = "qrc:/page/Music_detail_krc.qml"
            }
        }
    }

    Connections {
        target: qjson
        onRequestTotalChanged: {
            request_total = total
        }
    }


    Connections {
        target: downloader
        onDownloadProgress: {
            progressBar.value = bytesRead / totalBytes * 100
            // statusText.text = "下载进度: " + (bytesRead / totalBytes * 100).toFixed(2) + "%"
        }

        onDownloadFinished: {
            progressBar.visible = false
            showSuccess("下载完成,文件保存在: " + processFilePath(music_folder))
        }

        onDownloadFailed: {
            progressBar.visible = false
            showError("下载失败")
        }
    }

    // 动态背景图
    // AnimatedImage {
    //     id: bg1
    //     anchors.fill: parent  // 填充整个窗口
    //     fillMode: Image.PreserveAspectCrop  // 保持比例并裁剪
    //     source: "qrc:/Images/dynamic.gif"  // 动态图片路径
    //     z: -1  // 确保背景图位于最底层
    // }


    FluNavigationView {
        id: nav_view
        height: parent.height-bottomBar.height
        width: parent.width
        anchors.top: parent.top
        title: "FMusic"
        logo: "qrc:/logo.ico"
        z:9
        pageMode: FluNavigationViewType.NoStack

        displayMode: FluNavigationViewType.Auto



        items: ItemsOriginal {
            navigationView: nav_view
        }

        footerItems: ItemsFooter {
            navigationView: nav_view
        }

        autoSuggestBox:FluAutoSuggestBox{
            iconSource: FluentIcons.Search

            placeholderText: qsTr("Search")

        }
        Component.onCompleted: {
            mainWindow.setHitTestVisible(nav_view.buttonMenu)
            mainWindow.setHitTestVisible(nav_view.buttonBack)
            mainWindow.setHitTestVisible(nav_view.imageLogo)

        }

    }

    // 除底部的所有范围
    // Rectangle {
    //     id: content
    //     width: parent.width
    //     height: parent.height - bottomBar.height - titleBar.height
    //     anchors.top: titleBar.bottom
    //     anchors.left: parent.left
    //     anchors.right: parent.right
    //
    //     Row {
    //         width: parent.width
    //         height: parent.height
    //
    //         //左侧菜单栏的内容
    //         Rectangle {
    //             id: leftBar
    //             width: 270
    //             height: parent.height
    //             color: rec_color  //-------------------------------------------------------
    //
    //
    //
    //
    //
    //             Column {
    //                 width:parent.width
    //                 height:parent.height
    //                 topPadding:5
    //                 bottomPadding:5
    //                 anchors.centerIn: parent // 使 Column 在 Rectangle 中居中
    //                 spacing:5
    //                 // FluAutoSuggestBox{
    //                 //     // color: rec_color
    //                 //     width: parent.width
    //                 //     anchors.horizontalCenter: parent.horizontalCenter // 水平居中对齐到 Column
    //                 //     placeholderText: qsTr("音乐搜索...")
    //                 // }
    //
    //                 TextField {
    //                     id: searchBox
    //                     width: parent.width
    //                     anchors.horizontalCenter: parent.horizontalCenter // 水平居中对齐到 Column
    //                     placeholderText: qsTr("搜索音乐...")
    //                     Layout.fillWidth:  true
    //                     font.pixelSize:  14
    //                     color: "black"
    //                     background: Rectangle {
    //                         radius: 2
    //                         border.color: "#EDEBE9"
    //                         border.width:  1
    //                     }
    //
    //                     Keys.onPressed: {
    //                         if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
    //                             // 触发按下 Enter 键的事件
    //                             console.log(searchBox.text)
    //                             search_keyword = searchBox.text
    //                             music_list = qjson.kugou_search_music(searchBox.text)
    //
    //                             rightpageLoader.source = "qrc:/page/F_search_detail.qml"
    //                             rightpageLoader.item.goToPage(1)
    //                             headerCuerrent = -1
    //                             // performSearch(searchBox.text) // 调用搜索函数
    //                             event.accepted = true // 阻止事件进一步传播
    //                         }
    //                     }
    //                 }
    //
    //                 // 使用 Repeater 来动态生成菜单项
    //                 Repeater {
    //                     id:repeat
    //                     model: ListModel {
    //                         ListElement { name: "发现音乐"; icons: "qrc:/Images/discoverMusic.svg";qml: "qrc:/page/F_findmusic.qml" }
    //                         ListElement { name: "每日推荐"; icons: "qrc:/Images/day_recommend.png";qml: "qrc:/page/F_day_recommend.qml" }
    //                         ListElement {name: "我的收藏"; icons: "qrc:/Images/bookmark.svg";qml: "qrc:/page/F_collect.qml"}
    //                         ListElement {name: "本地音乐"; icons: "qrc:/Images/search.svg";qml: "qrc:/page/F_localmusic.qml"}
    //                         ListElement {name: "AI推荐"; icons: "qrc:/Images/rebot.png";qml: "qrc:/page/F_ai_recommend.qml"}
    //                     }
    //                     delegate: music_menu
    //                 }
    //
    //
    //                 Component {
    //                     id:music_menu
    //                     Rectangle {
    //                         width: parent.width
    //                         height: 40
    //                         radius: 8
    //                         property bool isHoverd: false
    //                         color: isHoverd || index === headerCuerrent ? tra_color : rec_color     // 设置按钮背景颜色
    //                         // background: Rectangle {
    //                         //         ------------------------------
    //                         //     // border.color: "#ffffff"              ------------------------------
    //                         // }
    //                         Row {
    //                             width:parent.width
    //                             height:parent.height
    //                             anchors.verticalCenter: parent.verticalCenter // 垂直居中对齐
    //                             anchors.left: parent.left
    //                             anchors.leftMargin: 10
    //                             spacing: 10
    //                             Image {
    //                                 source: icons
    //                                 width: 24
    //                                 height: 24
    //                                 anchors.verticalCenter: parent.verticalCenter // 图标垂直居中对齐
    //                             }
    //                             Text {
    //                                 text: name
    //                                 color: "#333333" // 设置文本颜色
    //                                 font.pointSize: 14
    //                                 anchors.verticalCenter: parent.verticalCenter // 文本垂直居中对齐
    //                             }
    //                         }
    //
    //                         // 鼠标区域，用于检测鼠标悬停事件
    //                         MouseArea {
    //                             anchors.fill: parent
    //                             hoverEnabled: true
    //
    //                             onClicked: {
    //                                 headerCuerrent = index
    //                                 rightpageLoader.source = qml
    //
    //                                 if(index === 1) {
    //                                     music_list = qjson.kugou_recommend()
    //                                 }
    //                                 else if(index === 2 ) {
    //                                     music_list = musicPlayer.get_user_collections(mainWindow.user.userId,0)
    //                                     // request_total = music_list.length
    //                                 }
    //                                 else if(index === 3 ) {
    //                                     fileDialog.open();
    //                                 }
    //                                 else if(index === 4) {
    //                                     ai_music_list = ""
    //                                     ai_music_list = qjson.kugou_ai_recommend(music_list[current_task_index].album_audio_id)
    //                                 }
    //
    //                                 // 在这里处理点击事件
    //                             }
    //
    //                             // 当鼠标悬停在按钮上时，改变背景颜色以显示阴影
    //                             onEntered: {
    //                                 parent.isHoverd = true
    //                             }
    //                             onExited: {
    //                                 parent.isHoverd = false
    //                             }
    //                         }
    //
    //                     }
    //                 }
    //
    //
    //
    //
    //             }
    //
    //         }
    //
    //         Rectangle {
    //             id: rightContent
    //             width: parent.width - leftBar.width
    //             height: parent.height
    //             color: ri_color   //----------------------------------rightcontent_color
    //             visible: true
    //             Row {
    //                 width: parent.width
    //                 height: parent.height
    //                 // 定义一个 Loader 组件用于动态加载页面
    //                 Loader {
    //                     id: rightpageLoader
    //                     anchors.fill: parent
    //                     active: true
    //                     source: "qrc:/page/F_findmusic.qml"  // 默认加载 Page1.qml
    //                 }
    //             }
    //         }
    //     }
    //
    //     About {
    //         id: aboutPage
    //         width: 200
    //         height: 300
    //         radius: 8
    //         visible: false
    //         color: "#AAAAAAFF"
    //         // border.color: "black"
    //         anchors.top: parent.top
    //         anchors.right: parent.right
    //         anchors.rightMargin: 10
    //         anchors.topMargin: 10
    //     }
    //
    //
    //
    //     Loader {
    //         id: detailPage
    //         anchors.fill: parent
    //         visible: false
    //         active: true
    //         source: "qrc:/page/Music_detail_krc.qml"  // 默认加载 Page1.qml
    //     }
    //
    //
    //
    // }

    // 底部栏
    FluRectangle {
        id: bottomBar
        radius: [25,0,0,25]
        width: parent.width
        height: 80
        anchors.bottom: parent.bottom
        z:10



        // Slider for music playing
        Row {
            width:parent.width
            height:10
            anchors.top:parent.top
            // spacing:5


            FluSlider{
                id: slider
                width: parent.width
                height:10
                anchors.top:parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                orientation: Qt.Horizontal

                property bool isDragging: false // 添加一个标志变量

                from: 0
                to: music_info.duration
                // stepSize: 1


                // value:duration

                // 关键控制逻辑
                Binding on value {
                    id: positionBinding
                    when: !slider.pressed   // 仅在未按下时绑定
                    value: duration
                }

                onPressedChanged: {
                    if (pressed) {
                        slider.isDragging = true // 设置标志为真
                        // musicPlayer.pause()
                    }else {
                        slider.isDragging = false // 设置标志为假
                        musicPlayer.setPosition(position * music_info.duration)
                        musicPlayer.loaded_play()

                    }
                }


            }

            Text {
                anchors.right: parent.right // 将 Text 锚定到 Row 的右边界
                anchors.rightMargin: 5 // 可选：添加一些右边距
                anchors.verticalCenter: slider.verticalCenter // 垂直中心对齐到 FluSlider
                text: music_info.duration
            }
        }

// other elements on the play surface
        Row {
            width:parent.width
            height:70
            anchors.bottom:parent.bottom

            // album picture
            RoundImage {
                id:roundImage
                // anchors.top:parent.top
                anchors.verticalCenter : musicPlay_buttion.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10  // 距离左边 20 个单位
                width: 56
                height: 56

                source: albumart
                radius: 5

                // 改变图标颜色
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        detailPage.visible = !detailPage.visible
                        console.log("onClicked")
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

            // album information
            SrollText {
                id:sroll_text
                width: 150
                height: 25
                anchors.top: parent.top
                anchors.left: roundImage.right
                anchors.leftMargin: 15
                anchors.topMargin: 7
                text: music_info.title
                colors: "#333333"
            }

            // dowload music
            Image {
                id: download_music
                source: "qrc:/Images/download.svg"
                width: 24
                height: 24
                anchors.left: roundImage.right
                anchors.leftMargin: 10
                anchors.bottom:parent.bottom
                anchors.bottomMargin: 5
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var url = musicPlayer.get_music_path()
                        // if(!url) {
                        //     url = music_list[current_task_index].play_url
                        // }
                        var suffix = extractSuffix(url)

                        if(music_folder == "") {
                            musicPlayer.createDirectory("./temp")
                            music_folder = "file:///./temp"
                        }
                        // downloader.downloadFile(url,processFilePath(music_folder) + "/" + music_list[current_task_index].music_name + " - " + music_list[current_task_index].art_name + "." + suffix)
                        downloader.downloadFile(url,processFilePath(music_folder) + "/" + music_info.title + " - " + music_info.artist + "." + suffix)
                        progressBar.visible = true
                    }
                    onEntered: {}

                    onExited: {}

                }

            }


            FluProgressBar {
                id: progressBar
                width: 200
                height: 10
                anchors.left:download_music.right
                anchors.leftMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                indeterminate: false
                visible:false
                value: 0
                from: 0
                to: 100
            }


// play pause next previous
            FluIconButton {
                id: pre_buttion
                // anchors.top:parent.top
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: musicPlay_buttion.left
                iconSource: FluentIcons.BackSolidBold
                iconSize: 25

                onClicked: {
                    // musicPlayer.Pre_play(current_task_index)
                    if (!musicPlayer.play(music_list[current_task_index - 1].play_url)) {
                        musicPlayer.play(qjson.get_kugou_url(music_list[current_task_index - 1].hash))
                    }
                    //更新对应的音乐专辑封面
                    // albumartManager.updateAlbumArt()
                    albumart = music_list[current_task_index - 1].cover_url
                    if (music_list[current_task_index - 1].lyric_url) {
                        lyrics.parseLyric(music_list[current_task_index - 1].lyric_url)
                    }
                    if (music_list[current_task_index - 1].hash) {
                        lyrics.parseLyric(qjson.get_kugou_lyric(music_list[current_task_index - 1].hash))
                    }
                    console.log(current_task_index)
                    if (current_task_index > 0) {
                        current_task_index = current_task_index - 1
                    }

                }
            }

            FluIconButton {
                id: musicPlay_buttion
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                iconSource: FluentIcons.Play
                iconSize: 25

                onClicked: {
                    if (iconSource === FluentIcons.Play) {
                        musicPlayer.loaded_play()
                        iconSource = FluentIcons.Pause // 切换到暂停图标
                    } else {
                        musicPlayer.pause()
                        iconSource = FluentIcons.Play // 切换回播放图标
                    }
                }
            }

            FluIconButton {
                id: next_buttion
                // anchors.horizontalCenter : parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: musicPlay_buttion.right
                iconSource: FluentIcons.ForwardSolidBold
                iconSize: 25

                onClicked: {
                    // musicPlayer.Next_play(current_task_index)
                    if (!musicPlayer.play(music_list[current_task_index + 1].play_url)) {
                        musicPlayer.play(qjson.get_kugou_url(music_list[current_task_index + 1].hash))
                    }

                    //更新对应的音乐专辑封面
                    // albumartManager.updateAlbumArt()
                    albumart = music_list[current_task_index + 1].cover_url
                    if (music_list[current_task_index + 1].lyric_url) {
                        lyrics.parseLyric(music_list[current_task_index + 1].lyric_url)
                    }
                    if (music_list[current_task_index + 1].hash) {
                        lyrics.parseLyric(qjson.get_kugou_lyric(music_list[current_task_index + 1].hash))
                    }
                    if (current_task_index < music_list.length - 1) {
                        current_task_index = current_task_index + 1
                    }
                    console.log(current_task_index)
                }
            }


// right function button
            FluIconButton {
                id: play_style
                anchors.verticalCenter: parent.verticalCenter
                anchors.right:volume.left
                anchors.rightMargin: 10
                iconSource: FluentIcons.Switch  //顺序播放
                iconSize: 25
                // FluentIcons.RepeatAll  //循环播放
                // FluentIcons.Shuffle //随机播放
                // FluentIcons.Switch //顺序播放
                onClicked: {
                    if (iconSource === FluentIcons.Switch){
                        play_styles = 1
                        iconSource = FluentIcons.Shuffle
                    }
                    else if (iconSource === FluentIcons.Shuffle) {
                        play_styles = 2
                        iconSource = FluentIcons.RepeatAll // 切换到暂停图标
                    } else {
                        play_styles = 0
                        iconSource = FluentIcons.Switch // 切换回播放图标
                    }
                }
            }

            FluIconButton {
                id: volume
                anchors.verticalCenter: parent.verticalCenter
                anchors.right:play_list.left
                anchors.rightMargin: 10
                iconSource: FluentIcons.Volume2
                iconSize: 25
                onClicked: {
                    volumeslider.visible = !volumeslider.visible
                }
            }

            FluSlider{
                id: volumeslider
                visible:false
                height:100
                anchors.bottom:volume.top
                anchors.horizontalCenter: volume.horizontalCenter
                orientation: Qt.Vertical

                from: 0
                to: 1000
                value:100


                onValueChanged: {
                    musicPlayer.setVolume(value) // 用户调整进度条时更新播放位置
                }
            }

            FluIconButton {
                id: play_list
                anchors.verticalCenter: parent.verticalCenter
                anchors.right:spec.left
                anchors.rightMargin: 10
                iconSource: FluentIcons.BulletedList
                iconSize: 25
                onClicked: {
                    play_queue.visible = !play_queue.visible
                }

            }

            F_play_queue {
                id: play_queue
                width: mainWindow.width * 0.3
                height: 300
                radius: 8
                visible: false
                color: "white"
                // border.color: "black"
                anchors.bottom: play_list.top
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.bottomMargin: 30
            }

            FluToggleSwitch {
                id: spec
                anchors.right:parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10
                onCheckedChanged: {
                    if (spec.checked) {
                        isSpectrumVisible = true
                    } else {
                        isSpectrumVisible = false
                    }
                }
            }


            // FluIconButton {
            //     id: spec
            //     anchors.right:parent.right
            //     anchors.verticalCenter: parent.verticalCenter
            //     iconSource:FluentIcons.Calculator
            //     iconSize: 25
            //
            //     onClicked: {
            //         // musicPlayer.decoded()
            //         // albumartManager.test()
            //
            //         FluRouter.navigate("/test",  {
            //             mode: FluRouter.Dialog,  // 正确模式枚举
            //             windowModality: Qt.ApplicationModal,  // 正确参数名
            //             parent: ApplicationWindow.window,
            //             onOpened: () => {  // 箭头函数语法
            //                 ApplicationWindow.window.enabled  = false
            //             },
            //             onClosed: () => {
            //                 ApplicationWindow.window.enabled  = true
            //             }
            //         })
            //
            //
            //         // FluRouter.navigate("/test")
            //
            //     }
            // }

        }
    }

    FolderDialog {
        id: fileDialog
        title: "select music directory"
        acceptLabel: "Accept"
        rejectLabel: "Cancel"
        onAccepted: {
            music_folder = folder; // 获取本地文件路径
            console.log("Selected directory:", music_folder)
            musicPlayer.get_Directory(music_folder)
            rightpageLoader.source = "qrc:/page/F_localmusic.qml"

            // music_list = musicPlayer.get_all_music_path()
            // console.log("Files found:", music_list)

        }
    }


    // 提取 URL 后缀的函数
    function extractSuffix(url) {
        var parts = url.split("."); // 按照 "." 分割字符串
        if (parts.length > 1) {
            return parts[parts.length - 1]; // 返回最后一个部分作为后缀
        } else {
            return ""; // 如果没有后缀，返回空字符串
        }
    }

    // 处理文件路径的函数
    function processFilePath(filePath) {
        if (filePath.startsWith("file:///")) {
            return filePath.replace("file:///", "")
        } else {
            return filePath // 如果没有前缀，直接返回原路径
        }
    }

}