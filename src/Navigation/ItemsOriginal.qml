import QtQuick 2.15
import FluentUI 1.0

FluObject{
    property var navigationView

    FluPaneItem{
        id:nav_home
        title: qsTr("Home")
        icon: FluentIcons.Home
        url: "qrc:/page/F_findmusic.qml"
        onTap: {
            navigationView.push(url)
        }
        
    }

    FluPaneItem{
    id:nav_recommendation
    title:qsTr("Recommendation")
    icon: FluentIcons.Calendar
    url: "qrc:/page/F_day_recommend.qml"
    onTap: {
        navigationView.push(url)
    }
    }

    FluPaneItem{
    id:nav_favorite
    title:qsTr("MyCollection")
    icon:FluentIcons.FavoriteStar
    url: "qrc:/page/F_collect.qml"
    onTap: {
        navigationView.push(url)
    }
    }

    FluPaneItem{
    id:nav_local
    title:qsTr("Local")
    icon:FluentIcons.Folder
    url: "qrc:/page/F_localmusic.qml"
    onTap: {
        navigationView.push(url)
    }
    }

    FluPaneItem{
        id:nav_ai_recom
        title:qsTr("AIRecom")
        icon:FluentIcons.Calories
        url: "qrc:/page/F_ai_recommend.qml"
        onTap: {
            navigationView.push(url)
        }
    }



}
