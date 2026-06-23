import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    Rectangle {
        id: settingsHeader
        width: parent.width; height: 60; color: window.bgTheme; anchors.top: parent.top
        Button {
            anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; anchors.margins: 10
            background: Rectangle { color: "transparent" }
            contentItem: Text { text: window.tr("◄ Back"); color: "#4A90E2"; font.pixelSize: 16; font.bold: true; verticalAlignment: Text.AlignVCenter }
            onClicked: stackView.pop()
        }
        Text { text: window.tr("Settings"); color: window.textTheme; font.pixelSize: 20; font.bold: true; anchors.centerIn: parent }
    }

    ScrollView {
        anchors.top: settingsHeader.bottom; anchors.bottom: parent.bottom
        anchors.left: parent.left; anchors.right: parent.right
        contentWidth: availableWidth; clip: true

        Column {
            width: parent.width - 40; anchors.horizontalCenter: parent.horizontalCenter
            topPadding: 20; bottomPadding: 40; spacing: 25

            Column {
                spacing: 10; width: parent.width
                Text { text: window.tr("CONNECTION"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1 }

                RowLayout {
                    width: parent.width; spacing: 10
                    TextField {
                        id: macSettingsInput
                        text: window.savedMacAddress // Loads the memory automatically
                        placeholderText: "..."
                        color: window.textTheme
                        font.pixelSize: 16
                        Layout.fillWidth: true
                        background: Rectangle { color: window.btnTheme; radius: 4; border.color: window.borderTheme }
                    }
                    Button {
                        background: Rectangle { color: "#4A90E2"; radius: 4 }
                        contentItem: Text { text: window.tr("Save"); color: "#FFFFFF"; font.bold: true; padding: 10 }
                        onClicked: window.savedMacAddress = macSettingsInput.text // Manually overwrites memory
                    }
                }
            }

            Rectangle { width: parent.width; height: 1; color: window.borderTheme }

            // Section 1: Appearance
            Column {
                spacing: 10; width: parent.width
                Text { text: window.tr("APPEARANCE"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1 }
                RowLayout {
                    width: parent.width
                    Text { text: window.tr("Dark Mode"); color: window.textTheme; font.pixelSize: 18; Layout.fillWidth: true }
                    Switch { checked: window.isDarkMode; onCheckedChanged: window.isDarkMode = checked }
                }
            }

            Rectangle { width: parent.width; height: 1; color: window.borderTheme }

            // Section 2: Preferences
            Column {
                spacing: 10; width: parent.width
                Text { text: window.tr("PREFERENCES"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1 }

                RowLayout {
                    width: parent.width
                    Text { text: window.tr("Language"); color: window.textTheme; font.pixelSize: 18; Layout.fillWidth: true }
                    ComboBox {
                        model: ["English", "Polski", "Deutsch"]
                        currentIndex: model.indexOf(window.appLanguage)
                        background: Rectangle { color: window.btnTheme; radius: 4 }
                        contentItem: Text { text: parent.currentText; color: window.textTheme; verticalAlignment: Text.AlignVCenter; padding: 10 }
                        onActivated: window.appLanguage = currentText
                    }
                }

                RowLayout {
                    width: parent.width
                    Text { text: window.tr("Units"); color: window.textTheme; font.pixelSize: 18; Layout.fillWidth: true }
                    ComboBox {
                        model: ["Metric (m, °C)", "Imperial (mi, °F)"]
                        currentIndex: window.unitSystem === "Metric" ? 0 : 1
                        background: Rectangle { color: window.btnTheme; radius: 4 }
                        contentItem: Text { text: parent.currentText; color: window.textTheme; verticalAlignment: Text.AlignVCenter; padding: 10 }
                        onActivated: window.unitSystem = (index === 0) ? "Metric" : "Imperial"
                    }
                }
            }

            Rectangle { width: parent.width; height: 1; color: window.borderTheme }

            // Section 3: Data Management
            Column {
                spacing: 10; width: parent.width
                Text { text: window.tr("DATA"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1 }

                Button {
                    text: window.tr("Training History")
                    width: parent.width
                    background: Rectangle { color: "#1A4C7D"; radius: 6; border.color: window.borderTheme; border.width: 1 }
                    contentItem: Text { text: parent.text; color: "#FFFFFF"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    onClicked: stackView.push("HistoryScreen.qml")
                }

                Button {
                    text: window.tr("Clear Session History")
                    width: parent.width
                    background: Rectangle { color: window.btnTheme; radius: 6; border.color: window.borderTheme; border.width: 1 }
                    contentItem: Text { text: parent.text; color: "#E53935"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    onClicked: window.trainingHistoryJson = "[]" // Instantly wipes memory
                }
            }
        }
    }
}