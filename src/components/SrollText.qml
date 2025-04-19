import QtQuick 2.15
import Qt5Compat.GraphicalEffects

Item {
    width: 100
    height: 25
    property var text: "" // 文本内容
    property var colors: "" // 颜色内容
    clip:true
    Text {
        id:idtext
        //width: parent.width
        height:parent.height
        text:parent.text
        color:parent.colors
        verticalAlignment: Text.AlignVCenter

        SequentialAnimation on x {
            loops: Animation.Infinite

            PropertyAnimation {
                from:0
                to: -idtext.width
                duration: 5000
            }
        }
    }
}
