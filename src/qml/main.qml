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

FluWindow {
    id: mainWindow
    width: 1010
    height: 710
    minimumWidth: 1010
    minimumHeight: 710
    launchMode: FluWindowType.SingleTask


    flags: {
        if (Qt.platform.os  === "windows") {
            return Qt.Window; // 在 windows 平台下关闭无边框
        } else {
            return Qt.FramelessWindowHint; // 在 其他 平台下开启无边框
        }
    }
    visible: true
    opacity: 1 // 设置窗口整体透明度（可选）
    title: qsTr("Fmusic")
    fixSize: false
    showStayTop: false
    showDark: false
    // showMinimize:false
    // showMaximize:false
    showClose:true
    // fitsAppBarWindows: true

    // 使用空的 FluAppBar
    appBar: FluAppBar {
        id: appBars
        height: 0
        visible: false
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


    property int request_total: 0 //请求的音乐总数
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
                // musicPlayer.setPosition(0)
                musicPlayer.play(music_list[current_task_index].play_url)
            }

        }
    }

    property UserFullInfo user //用户基本信息



    property int current_task_index : -1

    property var music_list: ""   //音乐列表数据

    property var music_detail: ""   //音乐歌词详情/列表

    property var music_info: "0"   //音乐信息

    property var albumart : "qrc:/logo.ico"  //专辑封面


    property int pos: 10 // 示例值，可以动态修改

    property var krc_pos: ""   //歌词位置信息


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

    // 动态背景图
    // AnimatedImage {
    //     id: bg1
    //     anchors.fill: parent  // 填充整个窗口
    //     fillMode: Image.PreserveAspectCrop  // 保持比例并裁剪
    //     source: "qrc:/Images/dynamic.gif"  // 动态图片路径
    //     z: -1  // 确保背景图位于最底层
    // }


    //整个窗口所有范围
    Column {
        anchors.fill: parent





        //标题栏
        TitleBar {
            id: titleBar
            width: parent.width
            height: 30
            color: rec_color
            anchors.top: appBars.bottom
        }




        // 除底部的所有范围
        Rectangle {
            id: content
            width: parent.width
            height: parent.height - bottomBar.height - titleBar.height
            anchors.top: titleBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            Row {
                width: parent.width
                height: parent.height

                //左侧菜单栏的内容
                Rectangle {
                    id: leftBar
                    width: 270
                    height: parent.height
                    color: rec_color  //-------------------------------------------------------





                    Column {
                        width:parent.width
                        height:parent.height
                        topPadding:5
                        bottomPadding:5
                        anchors.centerIn: parent // 使 Column 在 Rectangle 中居中
                        spacing:5
                        FluAutoSuggestBox{
                            // color: rec_color
                            width: parent.width
                            anchors.horizontalCenter: parent.horizontalCenter // 水平居中对齐到 Column
                            placeholderText: qsTr("音乐搜索...")
                        }


                        // 使用 Repeater 来动态生成菜单项
                        Repeater {
                            id:repeat
                            model: ListModel {
                                ListElement { name: "发现音乐"; icons: "qrc:/Images/music163.svg";qml: "qrc:/page/F_findmusic.qml" }
                                ListElement { name: "Form"; icons: "qrc:/logo.ico" }
                                ListElement {name: "My Music"; icons: "qrc:/Images/bookmark.svg"}
                                ListElement {name: "Select File"; icons: "qrc:/Images/search.svg";qml: "qrc:/page/F_localmusic.qml"}
                            }
                            delegate: music_menu
                        }


                        Component {
                            id:music_menu
                            Rectangle {
                                width: parent.width
                                height: 40
                                radius: 8
                                property bool isHoverd: false
                                color: isHoverd || index === headerCuerrent ? tra_color : rec_color     // 设置按钮背景颜色
                                // background: Rectangle {
                                //         ------------------------------
                                //     // border.color: "#ffffff"              ------------------------------
                                // }
                                Row {
                                    width:parent.width
                                    height:parent.height
                                    anchors.verticalCenter: parent.verticalCenter // 垂直居中对齐
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    spacing: 10
                                    Image {
                                        source: icons
                                        width: 24
                                        height: 24
                                        anchors.verticalCenter: parent.verticalCenter // 图标垂直居中对齐
                                    }
                                    Text {
                                        text: name
                                        color: "#333333" // 设置文本颜色
                                        font.pointSize: 14
                                        anchors.verticalCenter: parent.verticalCenter // 文本垂直居中对齐
                                    }
                                }

                                // 鼠标区域，用于检测鼠标悬停事件
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true

                                    onClicked: {
                                        headerCuerrent = index
                                        rightpageLoader.source = qml
                                        if(index === 3 ) {
                                            fileDialog.open();
                                        }

                                        // 在这里处理点击事件
                                    }

                                    // 当鼠标悬停在按钮上时，改变背景颜色以显示阴影
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

                Rectangle {
                    id: rightContent
                    width: parent.width - leftBar.width
                    height: parent.height
                    color: ri_color   //----------------------------------rightcontent_color
                    visible: true

                    Row {
                        width: parent.width
                        height: parent.height


                        // 定义一个 Loader 组件用于动态加载页面
                        Loader {
                            id: rightpageLoader
                            anchors.fill: parent
                            active: true
                            source: "qrc:/page/F_findmusic.qml"  // 默认加载 Page1.qml
                        }


                    }



                }
            }

            About {
                id: aboutPage
                width: 200
                height: 300
                radius: 8
                visible: false
                color: "#AAAAAAFF"
                // border.color: "black"
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.topMargin: 10
            }



            Loader {
                id: detailPage
                anchors.fill: parent
                visible: false
                active: true
                source: "qrc:/page/Music_detail_krc.qml"  // 默认加载 Page1.qml
            }



        }

        // 底部栏
        Rectangle {
            id: bottomBar
            width: parent.width
            height: 80
            color: rec_color //--------------------------------------------------------------------
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

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


            Row {
                width:parent.width
                height:70
                anchors.bottom:parent.bottom


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

                SrollText {
                    width: 150
                    height: 25
                    anchors.top: parent.top
                    anchors.left: roundImage.right
                    anchors.leftMargin: 15
                    anchors.topMargin: 7
                    text: music_info.title
                    colors: "#333333"
                }

                FluIconButton {
                    id: pre_buttion
                    // anchors.top:parent.top
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right:musicPlay_buttion.left
                    iconSource:FluentIcons.BackSolidBold
                    iconSize: 25

                    onClicked: {
                        musicPlayer.Pre_play(current_task_index)
                        //更新对应的音乐专辑封面
                        albumartManager.updateAlbumArt()
                        if(music_list[current_task_index-1].lyric_url) {
                            lyrics.parseLyric(music_list[current_task_index-1].lyric_url)
                        }
                        console.log(current_task_index)
                        if(current_task_index > 0) {
                            current_task_index = current_task_index - 1
                        }

                    }
                }

                FluIconButton {
                    id: musicPlay_buttion
                    anchors.horizontalCenter : parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource:FluentIcons.Play
                    iconSize: 25

                    onClicked : {
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
                    anchors.left:musicPlay_buttion.right
                    iconSource:FluentIcons.ForwardSolidBold
                    iconSize: 25

                    onClicked: {
                        musicPlayer.Next_play(current_task_index)
                        //更新对应的音乐专辑封面
                        albumartManager.updateAlbumArt()
                        if(music_list[current_task_index+1].lyric_url) {
                            lyrics.parseLyric(music_list[current_task_index+1].lyric_url)
                        }
                        // console.log(current_task_index)
                        if(current_task_index < music_list.length - 1) {
                            current_task_index = current_task_index + 1
                        }
                        console.log(current_task_index)
                    }
                }

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


                FluIconButton {
                    id: spec
                    anchors.right:parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource:FluentIcons.Calculator
                    iconSize: 25

                    onClicked: {
                        // musicPlayer.decoded()
                        // albumartManager.test()

                        FluRouter.navigate("/test",  {
                            mode: FluRouter.Dialog,  // 正确模式枚举
                            windowModality: Qt.ApplicationModal,  // 正确参数名
                            parent: ApplicationWindow.window,
                            onOpened: () => {  // 箭头函数语法
                                ApplicationWindow.window.enabled  = false
                            },
                            onClosed: () => {
                                ApplicationWindow.window.enabled  = true
                            }
                        })


                        // FluRouter.navigate("/test")

                    }
                }

            }
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

}