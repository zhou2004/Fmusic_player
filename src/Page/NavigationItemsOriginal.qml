import QtQuick 2.15
import FluentUI 1.0

FluObject{
    property var navigationView

    FluPaneItem{
        id:nav_home
        title: qsTr("Home")
        icon: FluentIcons.Home
        url: "qrc:/qt/qml/FMusic/Page/NavigationPage/Home.qml"
        onTap: {
            navigationView.push(url)
        }
        
    }

    FluPaneItem{
    id:nav_recommendation
    title:qsTr("Recommendation")
    icon: FluentIcons.Calendar
    url: "qrc:/qt/qml/FMusic/Page/NavigationPage/Recommendation.qml"
    onTap: {
        navigationView.push(url)
    }
    }

    FluPaneItem{
    id:nav_favorite
    title:qsTr("MyCollection")
    icon:FluentIcons.FavoriteStar
    url: "qrc:/qt/qml/FMusic/Page/NavigationPage/Favorite.qml"
    onTap: {
        navigationView.push(url)
    }
    }

    FluPaneItem{
    id:nav_local
    title:qsTr("Local")
    icon:FluentIcons.Folder
    url: "qrc:/qt/qml/FMusic/Page/NavigationPage/Local.qml"
    onTap: {
        navigationView.push(url)
    }
    }

    FluPaneItem{
        id:nav_ai_recom
        title:qsTr("AIRecom")
        icon:FluentIcons.Calories
        url: ""
        onTap: {
            navigationView.push(url)
        }
    }



}
