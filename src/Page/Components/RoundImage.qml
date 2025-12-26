import QtQuick 2.15
import QtQuick.Window 2.15 // 用于获取 Screen.devicePixelRatio
import Qt5Compat.GraphicalEffects
import FluentUI 1.0

Item {
    id: root
    property alias source: image.source
    property var masksource: FluentIcons.ChevronUp
    property int radius: 0
    property var onButtonClick: null
    property bool showColorOverlay: false
    property color colorOverlayColor: "#80000000"
    property bool isHovered: false
    // 注意：开启 mipmap 后 smooth 属性通常会被自动处理，但为了兼容性可以保留
    property alias smooth: image.smooth

    // 移除 alias sourceSize，改为内部自动计算以优化性能，
    // 如果外部强制指定，可以覆盖此逻辑
    property size requestedSourceSize: Qt.size(0, 0)

    property var fillMode: Image.PreserveAspectFit

    // 缩略图本体
    Image {
        id: image
        asynchronous: true
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        fillMode: root.fillMode

        // --- 优化点 1: 开启 Mipmap ---
        // 这对缩略图的清晰度至关重要
        mipmap: true

        // --- 优化点 2: 智能设置 sourceSize ---
        // 只有当 sourceSize 未被外部显式指定时，才自动计算
        // 乘以 devicePixelRatio 确保在高分屏上清晰
        sourceSize: root.requestedSourceSize.width > 0 ? root.requestedSourceSize : Qt.size(width * Screen.devicePixelRatio, height * Screen.devicePixelRatio)

        clip: true
        visible: false
    }

    // 圆角遮罩 Rectangle
    Item {
        id: maskRec
        anchors.fill: image
        visible: false

        Rectangle {
            anchors.centerIn: parent
            // 确保遮罩的大小与图片实际绘制的大小完全一致
            width: image.paintedWidth
            height: image.paintedHeight
            color: "black"
            radius: root.radius

            // --- 优化点 3: 开启抗锯齿 ---
            // 让圆角更平滑
            antialiasing: true

            // 解决某些特定渲染后端下的边缘黑边问题
            smooth: true
        }
    }

    // 遮罩后的图案
    OpacityMask {
        id: mask
        anchors.fill: image
        source: image
        maskSource: maskRec
        // 确保遮罩过程也是抗锯齿的（部分版本支持）
        antialiasing: true
    }

    // 颜色叠加效果
    ColorOverlay {
        id: change_image_color
        anchors.fill: mask // 确保覆盖在 mask 区域上
        color: colorOverlayColor
        source: mask
        visible: root.showColorOverlay

        // 优化 ColorOverlay 的边缘
        cached: true
    }

    // 图标按钮
    FluIconButton {
        id: icon_button
        iconSource: masksource
        anchors.centerIn: parent
        iconSize: 25
        visible: root.showColorOverlay
        onClicked: {
            if(root.onButtonClick){
                root.onButtonClick()
            }
        }
    }

    // 改变图标颜色
    MouseArea {
        // 只有当显示 overlay 或者 hover 开启时才启用
        enabled: root.isHovered
        anchors.fill: parent
        hoverEnabled: true // 必须显式开启，否则 onEntered 不生效

        onEntered: {
            // console.log("onEntered")
            root.showColorOverlay = true
        }
        onExited: {
            root.showColorOverlay = false
        }
        // 如果点击图片本身也需要触发事件，可以在这里加 onClicked
    }
}