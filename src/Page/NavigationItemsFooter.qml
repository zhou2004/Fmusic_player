
import QtQuick 2.15
import FluentUI 1.0

FluObject{

    property var navigationView

    id:footer_items

    FluPaneItem{
      title:qsTr("Login")
      icon: FluentIcons.Contact
      url: "qrc:/qt/qml/FMusic/Page/NavigationPage/UserLogin.qml"
      onTap: {
        navigationView.push(url)
      }
    }

    FluPaneItem{
        title:qsTr("Settings")
        icon:FluentIcons.Settings
        url: "qrc:/qt/qml/FMusic/Page/NavigationPage/Settings.qml"
        onTap: {
            navigationView.push(url)
        }
    }
}
