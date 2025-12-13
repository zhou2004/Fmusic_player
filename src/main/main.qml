import QtQuick
import QtQuick.Window
import QtQuick.Controls
import FluentUI 1.0
import QtQuick.Layouts
import QtMultimedia
import Qt.labs.platform
import "../navigation"
import "./"

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


    // appbar
    appBar: FluAppBar {
        id: appBars
        height: 30
        showDark: true
        showStayTop: false
        z:7
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
    }
}
