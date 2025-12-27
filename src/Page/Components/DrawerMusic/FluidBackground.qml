import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects 1.0

Item {
    id: root

    property string rawSource: ""
    property string fluidSource: ""

    visible: fluidSource !== ""
    property bool isRunning: visible && Qt.application.state === Qt.ApplicationActive

    onRawSourceChanged: {
        if (rawSource && audioProcessor) {
            audioProcessor.processFluidCover(rawSource)
        }
    }

    Connections {
        target: audioProcessor
        function onFluidCoverReady(base64Data) {
            root.fluidSource = base64Data
        }
    }

    // 【核心 1】根节点裁剪。
    // 我们会在内部把内容渲染得比屏幕大，然后切掉边缘，
    // 这样就永远看不到模糊导致的黑边了。
    clip: true

    // 黑色底板 (作为最后的防线，虽然理论上会被覆盖)
    Rectangle { anchors.fill: parent; color: "#1a1a1a" }

    // =========================================================
    // 渲染容器 (比屏幕大一圈，用于消除边缘黑影)
    // =========================================================
    Item {
        id: overscanContainer
        // 让容器向四周各延伸 100px
        anchors.fill: parent
        anchors.margins: -100

        // -----------------------------------------------------
        // 【核心 2】安全底图 (Base Layer)
        // 这是一个静止的、铺满的层。
        // 它的作用是：无论上面的流体怎么飘，缝隙里透出来的永远是
        // 专辑的主色调，而不是黑色。
        // -----------------------------------------------------
        Image {
            anchors.fill: parent
            source: root.fluidSource
            smooth: true
            asynchronous: false
            opacity: 1.0 // 完全不透明
            sourceSize: Qt.size(16, 16) // 强制拉伸
        }

        // -----------------------------------------------------
        // 动态流体层 (作为 FastBlur 的源)
        // -----------------------------------------------------
        Item {
            id: blobContainer
            anchors.fill: parent
            visible: false // 隐藏原始层

            // 定义通用的“游走光斑”
            Component {
                id: wanderingBlob
                Image {
                    source: root.fluidSource
                    smooth: true
                    // 巨大化：光斑尺寸是屏幕的 2 倍
                    width: Math.max(root.width, root.height) * 2.0
                    height: width
                    // 居中起始
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    opacity: 0.6
                }
            }

            // --- 光斑 1：主色调 (游走 A) ---
            Loader {
                sourceComponent: wanderingBlob
                // 撑满父容器
                width: parent.width; height: parent.height
                onLoaded: {
                    // 1. 极慢旋转 (仅为了打破纹理)
                    var rot = rotAnim.createObject(item, {target: item, duration: 150000})
                    rot.running = Qt.binding(() => root.isRunning)

                    // 2. 【核心 3】X轴 / Y轴 独立游走 (Lissajous 运动)
                    // 让 X 和 Y 的运动周期互质 (比如 23s 和 37s)，
                    // 这样永远不会重复轨迹，看起来完全随机。
                    var wanderX = wanderAnim.createObject(item, {target: item, prop: "x", range: 150, duration: 23000})
                    wanderX.running = Qt.binding(() => root.isRunning)

                    var wanderY = wanderAnim.createObject(item, {target: item, prop: "y", range: 150, duration: 37000})
                    wanderY.running = Qt.binding(() => root.isRunning)
                }
            }

            // --- 光斑 2：对比色 (游走 B - 镜像) ---
            Loader {
                sourceComponent: wanderingBlob
                width: parent.width; height: parent.height
                onLoaded: {
                    // 镜像翻转，获取不同颜色
                    item.transform = [Qt.createQmlObject('import QtQuick 2.0; Scale { xScale: -1; origin.x: width/2 }', item)]
                    item.opacity = 0.5

                    // 反向旋转
                    var rot = rotAnim.createObject(item, {target: item, from: 360, to: 0, duration: 120000})
                    rot.running = Qt.binding(() => root.isRunning)

                    // 不同的游走周期
                    var wanderX = wanderAnim.createObject(item, {target: item, prop: "x", range: 200, duration: 31000})
                    wanderX.running = Qt.binding(() => root.isRunning)

                    var wanderY = wanderAnim.createObject(item, {target: item, prop: "y", range: 200, duration: 29000})
                    wanderY.running = Qt.binding(() => root.isRunning)

                    // 呼吸效果
                    var pulse = pulseAnim.createObject(item, {target: item})
                    pulse.running = Qt.binding(() => root.isRunning)
                }
            }

            // --- 光斑 3：亮部 (游走 C - 倒置) ---
            Loader {
                sourceComponent: wanderingBlob
                width: parent.width; height: parent.height
                onLoaded: {
                    // 上下颠倒
                    item.transform = [Qt.createQmlObject('import QtQuick 2.0; Scale { yScale: -1; origin.y: height/2 }', item)]
                    item.opacity = 0.5

                    var rot = rotAnim.createObject(item, {target: item, duration: 180000})
                    rot.running = Qt.binding(() => root.isRunning)

                    var wanderX = wanderAnim.createObject(item, {target: item, prop: "x", range: 180, duration: 41000})
                    wanderX.running = Qt.binding(() => root.isRunning)

                    var wanderY = wanderAnim.createObject(item, {target: item, prop: "y", range: 180, duration: 33000})
                    wanderY.running = Qt.binding(() => root.isRunning)
                }
            }
        }

        // =========================================================
        // 最终融合：大半径模糊
        // =========================================================
        FastBlur {
            anchors.fill: blobContainer
            source: blobContainer
            // 增加半径，彻底抹平任何“旋转”的痕迹
            radius: 84
            // 设为 false 以避免边缘黑边 (虽然我们已经用了 overscan，双重保险)
            transparentBorder: false
        }
    }

    // =========================================================
    // 动画逻辑库 (Physics)
    // =========================================================

    Component {
        id: rotAnim
        RotationAnimation {
            loops: Animation.Infinite; from: 0; to: 360
        }
    }

    Component {
        id: wanderAnim
        SequentialAnimation {
            loops: Animation.Infinite
            property var target
            property string prop // "x" or "y"
            property real range  // 游走范围
            property int duration

            // 这里的逻辑是：在原始位置的基础上，增加/减少 range
            // 注意：因为 Image 的 x/y 是左上角坐标，我们需要基于当前值偏移
            // 这里的 from/to 必须动态计算比较麻烦，我们用简单的 PathAnimation 或者相对值
            // 简化方案：使用 Sine 曲线控制 offset

            // 下面这个是错的写法，修正为 NumberAnimation 控制 anchors 偏移或者 transform Translate
            // 但因为我们用了 Image 的 x,y 属性，最简单的方法是用 Translate

            // --- 修正后的游走逻辑 ---
            // 我们不直接改 x/y，而是让它做一个往复运动
            // 这里的 from/to 是相对于初始布局的偏移
            NumberAnimation {
                target: target
                property: prop
                // 稍微偏移一点，模拟漂浮
                from: (prop === "x" ? (root.width - target.width)/2 - range : (root.height - target.height)/2 - range)
                to:   (prop === "x" ? (root.width - target.width)/2 + range : (root.height - target.height)/2 + range)
                duration: duration
                easing.type: Easing.InOutSine // 正弦波让运动像水流一样丝滑
            }
            NumberAnimation {
                target: target
                property: prop
                from: (prop === "x" ? (root.width - target.width)/2 + range : (root.height - target.height)/2 + range)
                to:   (prop === "x" ? (root.width - target.width)/2 - range : (root.height - target.height)/2 - range)
                duration: duration
                easing.type: Easing.InOutSine
            }
        }
    }

    Component {
        id: pulseAnim
        SequentialAnimation {
            loops: Animation.Infinite
            property var target
            NumberAnimation { target: target; property: "scale"; from: 1.0; to: 1.15; duration: 15000; easing.type: Easing.InOutQuad }
            NumberAnimation { target: target; property: "scale"; from: 1.15; to: 1.0; duration: 15000; easing.type: Easing.InOutQuad }
        }
    }

    // 装饰：极度轻微的暗角 (可选，为了不让角落太亮抢戏)
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#40000000" }
            GradientStop { position: 0.2; color: "transparent" }
            GradientStop { position: 0.8; color: "transparent" }
            GradientStop { position: 1.0; color: "#60000000" }
        }
    }

    // 全局统一色调压暗
    Rectangle { anchors.fill: parent; color: "black"; opacity: 0.25 }
}