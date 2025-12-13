import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import FluentUI

Item {
    id: root

    property string text: "" // 文本内容
    property string color: "black" // 颜色内容
    property int size: 24
    // 关键：textWidth 动态绑定组件实际宽度
    property int textWidth: width

    width: parent ? parent.width : 200  // 设置默认宽度
    height: idtext.height
    clip: true

    Text {
        id: idtext
        text: root.text
        color: root.color
        font.pointSize: size
        renderType: FluTheme.nativeText ? Text.NativeRendering : Text.QtRendering

        // 关键：宽度变化时重新启动动画
        onWidthChanged: restartAnimation()

        // 动画定义
        SequentialAnimation {
            id: scrollAnim
            running: idtext.width > root.textWidth
            loops: Animation.Infinite
            PropertyAnimation {
                target: idtext
                property: "x"
                from: 0
                to: -idtext.width
                duration: 5000
            }
        }
    }

    // 关键：当文本宽度或容器宽度改变时，重启动画
    function restartAnimation() {
        scrollAnim.stop()
        idtext.x = 0
        if (idtext.width > root.textWidth)
            scrollAnim.start()
    }

    // 监听外部宽度变化
    onWidthChanged: restartAnimation()
    onTextChanged: restartAnimation()
}

