pragma Singleton
import QtQuick 2.15

QtObject {
    id: root

    property var dataPool: []
    property var availableItems: []
    readonly property bool isEmpty: availableItems.length  === 0

    // 初始化数据池（0-100）
    function initPool() {
        dataPool = Array.from({length:  101}, (_,i) => i)
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

    Component.onCompleted:  initPool()
}