import QtQuick 2.15
import Qt5Compat.GraphicalEffects

Item {
    width: 100
    height: 25
    property string text: "" // 文本内容
    property string colors: "black" // 颜色内容
    property int size: 24
    clip:true
    Text {
        id:idtext
        //width: parent.width
        height:parent.height
        text:parent.text
        color:parent.colors
        font.pointSize: size
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
