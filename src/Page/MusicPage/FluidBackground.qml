import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects 1.0

Item {
    id: root
    width: parent.width
    height: parent.height

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
                    height: Math.max(root.width, root.height) * 2.0
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

    // 【修复版】游走动画
    /*当 Loader 重新加载（例如切歌导致封面变化）时，旧的 item（光斑图片）会被销毁。但是，依附于它的动画对象（wanderAnim）在销毁的一瞬间，
    可能还会尝试计算最后一次 from 和 to 的绑定值。此时 target（即那个图片 item）已经变成了 null，所以去读取 target.width 就报错了。
    解决方法： 我们需要在计算动画数值时，加一个安全判断。如果 target 是 null，直接返回 0，不再去读它的宽高。*/
    Component {
        id: wanderAnim
        SequentialAnimation {
            id: seqAnim
            loops: Animation.Infinite
            running: root.isRunning && target !== null // 安全检查

            property var target: null
            property string prop
            property real range
            property int duration

            // 安全计算函数
            function getVal(isStart) {
                if (!target) return 0;
                var center = (prop === "x" ? (root.width - target.width)/2 : (root.height - target.height)/2);
                return isStart ? (center - range) : (center + range);
            }

            NumberAnimation {
                target: seqAnim.target
                property: seqAnim.prop
                from: seqAnim.getVal(true)
                to:   seqAnim.getVal(false)
                duration: seqAnim.duration / root.speed
                easing.type: Easing.InOutSine
            }
            NumberAnimation {
                target: seqAnim.target
                property: seqAnim.prop
                from: seqAnim.getVal(false)
                to:   seqAnim.getVal(true)
                duration: seqAnim.duration / root.speed
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