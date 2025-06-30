import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0

FluScrollablePage{
<<<<<<< HEAD
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
=======
  title: qsTr("Local")
  contentWidth: parent.width
  
  Item {
    anchors.fill: parent
    anchors.margins: 10
    
    FluFrame{
      anchors.fill: parent
      padding: 20
  
      ColumnLayout {
        anchors.fill: parent
        spacing: 10
  
        RowLayout {
          Layout.fillWidth: true
          spacing: 10
  
          FluButton{
            text: qsTr("Open")
            onClicked: {
  
            }
          }
          FluButton{
            text: qsTr("PlayAll")
            onClicked: {
  
            }
          }
        }
  
        FluPivot{
          Layout.fillWidth: true
          Layout.fillHeight: true
          currentIndex: 2
  
          FluPivotItem{
            title: qsTr("Music")
            contentItem: FluText{
              text: qsTr("All emails go here.")
            }
          }
          FluPivotItem{
            title: qsTr("Album")
            contentItem: FluText{
              text: qsTr("Unread emails go here.")
            }
          }
          FluPivotItem{
            title: qsTr("Singer")
            contentItem: FluText{
              text: qsTr("Flagged emails go here.")
            }
          }
        }
      }
    }
  }
}
>>>>>>> 383e005c615dd0769b500fdc84fb9a6b2c140dcf
