import QtQuick
import QtQuick.Layouts
import FluentUI 1.0

ColumnLayout {
    id: settingsDisplayInfo
    Layout.fillWidth: true
    Layout.fillHeight: true

    FluExpander {
        headerText: qsTr("Personalization")
        Layout.rightMargin: 10
        Layout.topMargin: 10
        Layout.fillWidth: true
        contentHeight: colorSettingsColumn.implicitHeight

        Column {
            id: colorSettingsColumn
            width: parent.width

            // 黑暗模式设置
            Rectangle {
                width: parent.width
                color: Window.active ? FluTheme.frameActiveColor : FluTheme.frameColor
                border.color: FluTheme.dividerColor
                height: 55

                FluText {
                    text: qsTr("Dark Mode")
                    anchors.left:parent.left
                    anchors.leftMargin: 60
                    anchors.verticalCenter: parent.verticalCenter
                }

                FluComboBox {
                    anchors.right:parent.right
                    anchors.rightMargin: 40
                    anchors.verticalCenter: parent.verticalCenter

                    model: ListModel {
                        id: darkModeModel
                        ListElement { text: qsTr("System") }
                        ListElement { text: qsTr("Light") }
                        ListElement { text: qsTr("Dark") }
                    }

                    onCommit: {
                        if (editText === qsTr("System")) {
                            FluTheme.darkMode = FluThemeType.System
                        } else if (editText === qsTr("Light")) {
                            FluTheme.darkMode = FluThemeType.Light
                        } else if (editText === qsTr("Dark")) {
                            FluTheme.darkMode = FluThemeType.Dark
                        }
                    }
                }
            }

            // 导航栏设置
            Rectangle {
                width: parent.width
                color: Window.active ? FluTheme.frameActiveColor : FluTheme.frameColor
                border.color: FluTheme.dividerColor
                height: 55

                FluText {
                    text: qsTr("Navigation Visibility")
                    anchors.left: parent.left
                    anchors.leftMargin: 60
                    anchors.verticalCenter: parent.verticalCenter
                }

                FluComboBox {
                    anchors.right:parent.right
                    anchors.rightMargin: 40
                    anchors.verticalCenter: parent.verticalCenter

                    model: ListModel {
                        id: navigationViewTypeModel
                        ListElement { text: qsTr("Open") }
                        ListElement { text: qsTr("Compact") }
                        ListElement { text: qsTr("Minimal") }
                        ListElement { text: qsTr("Auto") }
                    }

                    onCommit: {
                        if (editText === qsTr("Open")) {
                                nav_view.displayMode = FluNavigationViewType.Open
                        } else if (editText === qsTr("Compact")) {
                                nav_view.displayMode = FluNavigationViewType.Compact
                        } else if (editText === qsTr("Minimal")) {
                                nav_view.displayMode = FluNavigationViewType.Minimal
                        } else if (editText === qsTr("Auto")) {
                                nav_view.displayMode = FluNavigationViewType.Auto
                        }
                    }
                }
            }
        }
    }
}

