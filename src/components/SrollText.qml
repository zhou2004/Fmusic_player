import QtQuick 2.15
import Qt5Compat.GraphicalEffects

Item {

    property string text: "" // 文本内容
    property string color: "black" // 颜色内容
    property int size: 24
    property int textWidth: 100 // 宽度

    width: textWidth
    height: 25
    clip:true
    Text {
        id:idtext
        //width: parent.width
        height:parent.height
        text:parent.text
        color:parent.color
        font.pointSize: size
        verticalAlignment: Text.AlignVCenter

        SequentialAnimation on x {
            loops: Animation.Infinite
            running: idtext.width > parent.width

            PropertyAnimation {
                from:0
                to: -idtext.width
                duration: 5000
            }
        }
    }
}
