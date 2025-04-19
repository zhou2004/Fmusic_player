import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import FluentUI 1.0

Item {
    property string source: ""  // 图片资源路径
    property var masksource: FluentIcons.ChevronUp // 遮罩资源路径
    property int radius: 0  // 圆角大小
    property var onButtonClick: null  // 按钮点击事件
    property bool showColorOverlay: false  // 控制 ColorOverlay 是否显示
    property color colorOverlayColor: "#80000000" // 蓝色
    property bool isHovered: true

    width: image.width
    height: image.height

    // 缩略图本体
    Image {
        id: image
        source: parent.source
        asynchronous: true
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        // 使用 PreserveAspectFit 确保在原始比例下不会变形
        fillMode: Image.PreserveAspectFit
        clip: true
        visible: false // 因为显示的是 OpacityMask 需要 false
    }

    // 圆角遮罩 Rectangle
    Rectangle {
        id: maskRec
        anchors.centerIn: parent
        width: image.width
        height: image.height
        color: "transparent"

        Rectangle {
            anchors.centerIn: parent
            width: image.paintedWidth
            height: image.paintedHeight
            color: "black"
            radius: parent.parent.radius
        }
        visible: false // 因为显示的是 OpacityMask 需要 false
    }

    // 遮罩后的图案
    OpacityMask {
        id: mask
        anchors.fill: image
        source: image
        maskSource: maskRec
    }

    // 颜色叠加效果
    ColorOverlay {
        id: change_image_color
        anchors.fill: parent
        color: colorOverlayColor  // 半透明黑色
        source: mask  // 绑定到经过圆角裁剪后的图片
        visible: parent.showColorOverlay  // 根据条件显示
    }

    // 图标按钮
    FluIconButton {
        id: icon_button
        iconSource: masksource
        anchors.centerIn: parent  // 居中显示
        iconSize: 25
        visible: parent.showColorOverlay  // 根据条件显示
    }

    // 改变图标颜色
    MouseArea {
        visible: isHovered
        anchors.fill: parent
        // hoverEnabled: true
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