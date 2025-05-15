import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0

FluScrollablePage{
    title: qsTr("Local")
    FluPivot{
        anchors.fill: parent
        FluPivotItem:{
            text: qsTr("Music")
            contentItem: FluText{
            text: qsTr("Music.")
        }
        }
        FluPivotItem:{
            text: qsTr("Album")
            contentItem: FluText{
            text: qsTr("Album.")
        }
        }
        FluPivotItem: {
            text: qsTr("Singer")
            contentItem: FluText {
            text: qsTr("Singer.")
        }
        }
    }



 }
