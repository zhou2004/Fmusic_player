import QtQuick
import QtQuick.Window
import QtQuick.Controls
import FluentUI 1.0
import QtQuick.Layouts
import QtMultimedia
import Qt.labs.platform
import "Components/DrawerMusic"

FluWindow {
    id: mainWindow

    // 控制bottom的显隐（声明式绑定：非播放页始终可见；播放页时仅在底部悬停可见）
    property bool bottomBarVisible: (!musicPlayerPageVisible) || (hoverRegion === "bottom")
    property int bottomHeight: 100

    // 当前鼠标所在的悬停区域："top" / "bottom" / ""
    property string hoverRegion: ""
    // 控制全屏播放页的显隐
    property bool musicPlayerPageVisible: false

    fitsAppBarWindows: true
    height: 710
    launchMode: FluWindowType.SingleTask
    minimumHeight: 400
    minimumWidth: 700
    opacity: 1 // 设置窗口整体透明度（可选）

    width: 1010

    // appbar
    appBar: FluAppBar {
        id: appBars

        height: 30
        showDark: true
        showStayTop: false
        visible: !musicPlayerPageVisible   // 普通模式下可见，播放页打开时交给悬停控制
        z: 7
    }

    //window blur effect
    //TODO: set on only on windows platform
    Component.onCompleted: {
        FluTheme.blurBehindWindowEnabled = false;
    }
    onMusicPlayerPageVisibleChanged: {
        if (!musicPlayerPageVisible) {
            hoverRegion = "";
            // appBars.visible = true;
            // bottomBarVisible = true
        } else
        // hoverRegion = ""
        // appBars.visible = false
        // bottomBarVisible = false
        {}
    }

    FluNavigationView {
        id: nav_view

        anchors.top: parent.top
        displayMode: FluNavigationViewType.Auto
        height: parent.height - bottomHeight
        logo: "qrc:/qt/qml/FMusic/Assets/icon_256.ico"
        pageMode: FluNavigationViewType.NoStack
        title: "FMusic"
        width: parent.width
        z: 9

        autoSuggestBox: FluAutoSuggestBox {
            iconSource: FluentIcons.Search
            placeholderText: qsTr("Search")
        }
        footerItems: NavigationItemsFooter {
            navigationView: nav_view
        }
        items: NavigationItemsOriginal {
            navigationView: nav_view
        }

        Component.onCompleted: {
            mainWindow.setHitTestVisible(nav_view.buttonMenu);
            mainWindow.setHitTestVisible(nav_view.buttonBack);
            mainWindow.setHitTestVisible(nav_view.imageLogo);
        }
    }
    // bottom bar
    BottomBar {
        id: bottomBar

        anchors.bottom: parent.bottom
        bottomBarHeight: bottomHeight
        bottomBarWidth: parent.width
        visible: bottomBarVisible
        z: 10000   // 提到最上层
    }

    //=============== 悬停显隐（修正枚举错误） =================

    MouseArea {
        id: bottomHoverArea

        acceptedButtons: Qt.NoButton
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        enabled: musicPlayerPageVisible
        height: bottomHeight
        hoverEnabled: true
        z: 10001

        onEntered: {
            // 鼠标进入底部区域，标记状态并体现显隐规则：
            // 若正在播放页模式，则需要显示 bottomBar；若非播放页，bottomBar 本来就可见
            hoverRegion = "bottom";
            // bottomBarVisible = true
        }
        onExited: {
            // 鼠标离开底部区域时，只有在播放页模式下才隐藏底部栏；
            // 非播放页模式保持可见
            if (hoverRegion === "bottom")
                hoverRegion = "";
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
            visible: musicPlayerPageVisible
            y: musicPlayerPageVisible ? 0 : parent.height

            Behavior on y {
                NumberAnimation {
                    duration: 300
                    easing.type: musicPlayerPageVisible ? Easing.OutCubic : Easing.InCubic
                }
            }

            onRequestClose: {
                mainWindow.musicPlayerPageVisible = false;
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
                    mainWindow.startSystemMove();
                }
            }
            Row {
                id: windowButtonsRow

                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 6
                spacing: 6
                z: 100

                FluIconButton {
                    id: btnMinimize

                    iconSize: 12
                    iconSource: FluentIcons.ChromeMinimize

                    onClicked: mainWindow.showMinimized()
                }
                FluIconButton {
                    id: btnMaximize

                    iconSize: 12
                    iconSource: FluentIcons.ChromeFullScreen

                    onClicked: {
                        if (mainWindow.visibility === Window.Maximized) {
                            mainWindow.showNormal();
                        } else {
                            mainWindow.showMaximized();
                        }
                    }
                }
                FluIconButton {
                    id: btnClose

                    iconSize: 12
                    iconSource: FluentIcons.ChromeClose

                    onClicked: {
                        mainWindow.musicPlayerPageVisible = false;
                        mainWindow.close();
                    }
                }
            }
        }
    }
}
