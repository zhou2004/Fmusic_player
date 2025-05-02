import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import QtQuick.Layouts 1.15
import QtMultimedia
import Qt.labs.platform 1.1
import "../components"


// 自定义顶部栏
Rectangle {
    id: titleBar
    width: parent.width
    height: 30
    color: "white"
    z: 100

    property string title: ""
    property string darkText : qsTr("Dark")
    property string lightText : qsTr("Light")
    property string minimizeText : qsTr("Minimize")
    property string restoreText : qsTr("Restore")
    property string maximizeText : qsTr("Maximize")
    property string closeText : qsTr("Close")
    property string stayTopText : qsTr("Sticky on Top")
    property string stayTopCancelText : qsTr("Sticky on Top cancelled")
    property color textColor: FluTheme.fontPrimaryColor
    property color minimizeNormalColor: FluTheme.itemNormalColor
    property color minimizeHoverColor: FluTheme.itemHoverColor
    property color minimizePressColor: FluTheme.itemPressColor
    property color maximizeNormalColor: FluTheme.itemNormalColor
    property color maximizeHoverColor: FluTheme.itemHoverColor
    property color maximizePressColor: FluTheme.itemPressColor
    property color closeNormalColor: Qt.rgba(0,0,0,0)
    property color closeHoverColor: Qt.rgba(251/255,115/255,115/255,1)
    property color closePressColor: Qt.rgba(251/255,115/255,115/255,0.8)
    property bool showDark: false
    property bool showClose: true
    property bool showMinimize: true
    property bool showMaximize: true
    property bool showStayTop: true
    property bool titleVisible: true
    property url icon
    property int iconSize: 20
    property bool isMac: FluTools.isMacos()
    property color borerlessColor : FluTheme.primaryColor
    property var user_avatar_url: "qrc:/Images/头像.jpg"
    // property alias buttonStayTop: btn_stay_top
    property alias buttonMinimize: btn_minimize
    property alias buttonMaximize: btn_maximize
    property alias buttonClose: btn_close
    // property alias buttonDark: btn_dark
    // property alias layoutMacosButtons: layout_macos_buttons
    // property alias layoutStandardbuttons: layout_standard_buttons

    property var minClickListener: function(){
        if(d.win.transientParent != null){
            d.win.transientParent.showMinimized()
        }else{
            d.win.showMinimized()
        }
    }

    property var maxClickListener : function(){
        if(FluTools.isMacos()){
            if (d.win.visibility === Window.FullScreen || d.win.visibility === Window.Maximized)
                d.win.showNormal()
            else
                d.win.showFullScreen()
        }else{
            if (d.win.visibility === Window.Maximized || d.win.visibility === Window.FullScreen)
                d.win.showNormal()
            else
                d.win.showMaximized()
            d.hoverMaxBtn = false
        }
    }

    // 鼠标拖拽区域
    MouseArea {
        id: dragArea
        property var click_pos: Qt.point(0, 0)
        anchors.fill: parent
        onPositionChanged: function (mouse) {
            if(!pressed) return

            if(!mainWindow.startSystemMove()) { //启用系统自带的拖拽
                var offset = Qt.point(mouseX - click_pos.x, mouseY - click_pos.y)
                mainWindow.x += offset.x
                mainWindow.y += offset.y
            }
        }
        onPressedChanged: function(mouse) {
            click_pos = Qt.point(mouseX, mouseY)
        }

    }



    RowLayout {
        anchors.fill: parent
        spacing: 10

        // 添加自定义按钮

        Item{
            id:d
            property var hitTestList: []
            property bool hoverMaxBtn: false
            property var win: Window.window
            property bool stayTop: {
                if(d.win instanceof FluWindow){
                    return d.win.stayTop
                }
                return false
            }
            property bool isRestore: win && (Window.Maximized === win.visibility || Window.FullScreen === win.visibility)
            property bool resizable: win && !(win.height === win.maximumHeight && win.height === win.minimumHeight && win.width === win.maximumWidth && win.width === win.minimumWidth)
            function containsPointToItem(point,item){
                var pos = item.mapToGlobal(0,0)
                var rect = Qt.rect(pos.x,pos.y,item.width,item.height)
                if(point.x>rect.x && point.x<(rect.x+rect.width) && point.y>rect.y && point.y<(rect.y+rect.height)){
                    return true
                }
                return false
            }
        }

        // title: "Fmusic"
        // icon: "qrc:/Images/music163.svg"

        Row{
            anchors{
                verticalCenter: parent.verticalCenter
                left: titleBar.isMac ? undefined : parent.left
                leftMargin: titleBar.isMac ? undefined : 10
                horizontalCenter: titleBar.isMac ? parent.horizontalCenter : undefined
            }
            spacing: 10
            Image{
                width: titleBar.iconSize
                height: titleBar.iconSize
                visible: status === Image.Ready ? true : false
                source: "qrc:/Images/title.png"
                anchors.verticalCenter: parent.verticalCenter
            }
            FluText {
                text: "Fmusic"
                visible: titleBar.titleVisible
                color:titleBar.textColor
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        FluButton {
            anchors.right: btn_minimize.left
            background: Rectangle {
                // color: "#ffffff" // 设置按钮背景颜色    ------------------------------
                // border.color: "#ffffff"              ------------------------------
            }
            contentItem: Row {
                width:parent.width
                height:parent.height
                anchors.verticalCenter: parent.verticalCenter // 垂直居中对齐
                RoundImage {
                    id: user_avatar
                    source: mainWindow.user.user_avatar_url ? mainWindow.user.user_avatar_url : titleBar.user_avatar_url
                    width: 24
                    height: 24
                    radius: 5
                    anchors.verticalCenter: parent.verticalCenter // 图标垂直居中对齐
                    anchors.horizontalCenter: parent.horizontalCenter // 图标水平居中对齐
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            aboutPage.visible = !aboutPage.visible
                            console.log("onClicked")
                        }
                        onEntered: {}

                        onExited: {}

                    }
                }
            }

            // 鼠标区域，用于检测鼠标悬停事件
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    // 在这里处理点击事件
                }

            }

        }

        FluIconButton{
            id:btn_minimize
            anchors.right: btn_maximize.left
            Layout.preferredWidth: 40
            Layout.preferredHeight: 30
            padding: 0
            verticalPadding: 0
            horizontalPadding: 0
            iconSource : FluentIcons.ChromeMinimize
            Layout.alignment: Qt.AlignVCenter
            iconSize: 11
            text:titleBar.minimizeText
            radius: 0
            visible: !titleBar.isMac && titleBar.showMinimize
            iconColor: titleBar.textColor
            color: {
                if(pressed){
                    return titleBar.minimizePressColor
                }
                return hovered ? titleBar.minimizeHoverColor : titleBar.minimizeNormalColor
            }
            onClicked: titleBar.minClickListener()
        }

        FluIconButton{
            id:btn_maximize
            anchors.right: btn_close.left
            property bool hover: btn_maximize.hovered
            Layout.preferredWidth: 40
            Layout.preferredHeight: 30
            padding: 0
            verticalPadding: 0
            horizontalPadding: 0
            iconSource : d.isRestore  ? FluentIcons.ChromeRestore : FluentIcons.ChromeMaximize
            color: {
                if(down){
                    return titleBar.maximizePressColor
                }
                return btn_maximize.hover ? titleBar.maximizeHoverColor : titleBar.maximizeNormalColor
            }
            Layout.alignment: Qt.AlignVCenter
            visible: d.resizable && !titleBar.isMac && titleBar.showMaximize
            radius: 0
            iconColor: titleBar.textColor
            text:d.isRestore?restoreText:maximizeText
            iconSize: 11
            onClicked: titleBar.maxClickListener()
        }

        FluIconButton{
            id:btn_close
            Layout.preferredWidth: 40
            Layout.preferredHeight: 30
            anchors.right: parent.right
            padding: 0
            verticalPadding: 0
            horizontalPadding: 0
            iconSource : FluentIcons.ChromeClose
            Layout.alignment: Qt.AlignVCenter
            text:closeText
            visible: !isMac && showClose
            radius: 0
            iconSize: 10
            iconColor: hovered ? Qt.rgba(1,1,1,1) : titleBar.textColor
            color:{
                if(pressed){
                    return titleBar.closePressColor
                }
                return hovered ? titleBar.closeHoverColor : titleBar.closeNormalColor
            }
            onClicked: Window.window.close()
        }

        // 其他按钮可以类似添加
    }
}