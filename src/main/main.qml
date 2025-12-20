import QtQuick
import QtQuick.Window
import QtQuick.Controls
import FluentUI 1.0
import QtQuick.Layouts
import QtMultimedia
import Qt.labs.platform
import "../navigation"
import "../components/DrawerMusic"

FluWindow {
    id: mainWindow
    width: 1010
    height: 710
    minimumWidth: 700
    minimumHeight: 400
    launchMode: FluWindowType.SingleTask
    fitsAppBarWindows: true
    opacity: 1 // 设置窗口整体透明度（可选）

    property int bottomHeight: 100
    // 控制全屏播放页的显隐
    property bool musicPlayerPageVisible: false

    // 当前鼠标所在的悬停区域："top" / "bottom" / ""
    property string hoverRegion: ""

    // 控制bottom的显隐（声明式绑定：非播放页始终可见；播放页时仅在底部悬停可见）
    property bool bottomBarVisible: (!musicPlayerPageVisible) || (hoverRegion === "bottom")


    // appbar
    appBar: FluAppBar {
        id: appBars
        height: 30
        showDark: true
        showStayTop: false
        z:7
        visible: !musicPlayerPageVisible   // 普通模式下可见，播放页打开时交给悬停控制
    }

    //window blur effect
    //TODO: set on only on windows platform
    Component.onCompleted: {
        FluTheme.blurBehindWindowEnabled = false
    }


    FluNavigationView {

        id: nav_view
        height: parent.height-bottomHeight
        width: parent.width
        anchors.top: parent.top
        title: "FMusic"
        logo: "qrc:/assets/icon_256.ico"
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
    // bottom bar
    BottomBar {
        id: bottomBar
        bottomBarHeight: bottomHeight
        bottomBarWidth: parent.width
        anchors.bottom: parent.bottom
        z: 10000   // 提到最上层
        visible: bottomBarVisible
    }

    //=============== 悬停显隐（修正枚举错误） =================

    MouseArea {
        id: bottomHoverArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: bottomHeight
        hoverEnabled: true
        enabled: musicPlayerPageVisible
        acceptedButtons: Qt.NoButton
        z: 10001

        onEntered: {
            // 鼠标进入底部区域，标记状态并体现显隐规则：
            // 若正在播放页模式，则需要显示 bottomBar；若非播放页，bottomBar 本来就可见
            hoverRegion = "bottom"
            // bottomBarVisible = true
        }
        onExited: {
            // 鼠标离开底部区域时，只有在播放页模式下才隐藏底部栏；
            // 非播放页模式保持可见
            if (hoverRegion === "bottom") hoverRegion = ""
        }
    }

    //=============== 全屏播放页 + 拖拽窗口 =================

    Item {
        id: fullPlayerContainer
        anchors.fill: parent
        z: 9000

        Rectangle {
            anchors.fill: parent
            color: "#000000"
            opacity: musicPlayerPageVisible ? 0.5 : 0.0
            visible: musicPlayerPageVisible
        }

        MusicPlayerPage {
            id: musicPlayerPage
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height
            y: musicPlayerPageVisible ? 0 : parent.height
            visible: musicPlayerPageVisible

            onRequestClose: {
                mainWindow.musicPlayerPageVisible = false
            }

            Behavior on y {
                NumberAnimation {
                    duration: 300
                    easing.type: musicPlayerPageVisible ? Easing.OutCubic : Easing.InCubic
                }
            }

            // 播放页顶部拖拽区域，复用 appBar 的拖动行为
            MouseArea {
                id: dragArea
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 40      // 顶部 40 像素可用来拖动窗口
                hoverEnabled: false

                onPressed: {
                    // 开始系统级窗口移动，和 FluAppBar 行为一致
                    mainWindow.startSystemMove()
                }
            }

            Row {
                id: windowButtonsRow
                spacing: 6
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 6
                anchors.rightMargin: 10
                z: 100

                FluIconButton {
                    id: btnMinimize
                    iconSource: FluentIcons.ChromeMinimize
                    iconSize: 12
                    onClicked: mainWindow.showMinimized()
                }

                FluIconButton {
                    id: btnMaximize
                    iconSource: FluentIcons.ChromeFullScreen
                    iconSize: 12
                    onClicked: {
                        if (mainWindow.visibility === Window.Maximized) {
                            mainWindow.showNormal()
                        } else {
                            mainWindow.showMaximized()
                        }
                    }
                }

                FluIconButton {
                    id: btnClose
                    iconSource: FluentIcons.ChromeClose
                    iconSize: 12
                    onClicked: {
                        mainWindow.musicPlayerPageVisible = false
                        mainWindow.close()
                    }
                }
            }
        }
    }

    onMusicPlayerPageVisibleChanged: {
        if (!musicPlayerPageVisible) {
            hoverRegion = ""
            appBars.visible = true
            // bottomBarVisible = true
        } else {
            // hoverRegion = ""
            // appBars.visible = false
            // bottomBarVisible = false
        }
    }

}
