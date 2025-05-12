import QtQuick 2.15
import FluentUI 1.0

FluObject{
    property var navigationView

    FluPaneItem{
        id:nav_home
        title: qsTr("Home")
        icon: FluentIcons.Home
        url: "qrc:/navigationPage/home.qml"
        onTap: {
            navigationView.push(url)
        }
        
    }

    FluPaneItem{
    id:nav_recommendation
    title:qsTr("Recommendation")
    icon: FluentIcons.Calendar
    url: "qrc:/navigationPage/recommendation.qml"
    onTap: {
        navigationView.push(url)
    }
    }

    FluPaneItem{
    id:nav_favorite
    title:qsTr("MyCollection")
    icon:FluentIcons.FavoriteStar
    url: "qrc:/navigationPage/favorite.qml"
    onTap: {
        navigationView.push(url)
    }
    }

    FluPaneItem{
    id:nav_local
    title:qsTr("Local")
    icon:FluentIcons.Folder
    url: "qrc:/navigationPage/local.qml"
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
