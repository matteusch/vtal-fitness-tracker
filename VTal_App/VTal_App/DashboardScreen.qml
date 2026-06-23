import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    Rectangle {
        id: headerBar; width: parent.width; height: 60; color: window.bgTheme; anchors.top: parent.top
        Text { text: window.isTrainingActive ? window.tr(window.currentActivity) : window.tr("Overview"); color: window.textTheme; font.pixelSize: 20; font.bold: true; anchors.centerIn: parent }
    }

    Item {
        anchors.top: headerBar.bottom; anchors.bottom: bottomBar.top; anchors.left: parent.left; anchors.right: parent.right
        Column {
            anchors.centerIn: parent; spacing: 12

            // Card 1: Heart Rate (Always visible)
            Rectangle {
                width: window.width * 0.85; height: 90; color: window.cardTheme; radius: 12; border.color: window.borderTheme
                Column {
                    anchors.centerIn: parent; spacing: 2
                    Text { text: window.tr("HEART RATE"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1; anchors.horizontalCenter: parent.horizontalCenter }
                    Row {
                        spacing: 6; anchors.horizontalCenter: parent.horizontalCenter
                        Text { text: typeof vtal.bpm !== "undefined" ? vtal.bpm : "--"; color: window.textTheme; font.pixelSize: 36; font.bold: true }
                        Text { text: "bpm"; color: "#E53935"; font.pixelSize: 14; font.bold: true; anchors.baseline: parent.bottom; anchors.baselineOffset: -6 }
                    }
                }
            }

            // Card 2: Ambient Temp (Hides during training)
            Rectangle {
                visible: !window.isTrainingActive
                width: window.width * 0.85; height: 90; color: window.cardTheme; radius: 12; border.color: window.borderTheme
                Column {
                    anchors.centerIn: parent; spacing: 2
                    Text { text: window.tr("AMBIENT TEMP"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1; anchors.horizontalCenter: parent.horizontalCenter }
                    Row {
                        spacing: 6; anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            text: typeof vtal.temperature === "undefined" ? "--" : (window.unitSystem === "Metric" ? vtal.temperature.toFixed(1) : ((vtal.temperature * 9/5) + 32).toFixed(1))
                            color: window.textTheme; font.pixelSize: 36; font.bold: true
                        }
                        Text { text: window.unitSystem === "Metric" ? "°C" : "°F"; color: "#42A5F5"; font.pixelSize: 14; font.bold: true; anchors.baseline: parent.bottom; anchors.baselineOffset: -6 }
                    }
                }
            }

            // Card 3: Step Counter (Hides during training)
            Rectangle {
                visible: !window.isTrainingActive
                width: window.width * 0.85; height: 90; color: window.cardTheme; radius: 12; border.color: window.borderTheme
                Column {
                    anchors.centerIn: parent; spacing: 2
                    Text { text: window.tr("STEP COUNTER"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1; anchors.horizontalCenter: parent.horizontalCenter }
                    Row {
                        spacing: 6; anchors.horizontalCenter: parent.horizontalCenter
                        Text { text: typeof vtal.steps !== "undefined" ? vtal.steps : "--"; color: window.textTheme; font.pixelSize: 36; font.bold: true }
                        Text { text: window.tr("steps"); color: "#66BB6A"; font.pixelSize: 14; font.bold: true; anchors.baseline: parent.bottom; anchors.baselineOffset: -6 }
                    }
                }
            }

            // Card 4: Distance Covered (Always visible)
            Rectangle {
                width: window.width * 0.85; height: 90; color: window.cardTheme; radius: 12; border.color: window.borderTheme
                Column {
                    anchors.centerIn: parent; spacing: 2
                    Text { text: window.tr("DISTANCE COVERED"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1; anchors.horizontalCenter: parent.horizontalCenter }
                    Row {
                        spacing: 6; anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            text: {
                                if (typeof vtal.distance === "undefined") return "--"
                                let d = window.isTrainingActive ? (vtal.distance - window.sessionDistanceStart) : vtal.distance
                                if (d < 0) d = 0
                                return window.unitSystem === "Metric" ? d.toFixed(2) : (d * 0.621371).toFixed(2)
                            }
                            color: window.textTheme; font.pixelSize: 36; font.bold: true
                        }
                        Text { text: window.unitSystem === "Metric" ? "m" : "mi"; color: "#FF9800"; font.pixelSize: 14; font.bold: true; anchors.baseline: parent.bottom; anchors.baselineOffset: -6 }
                    }
                }
            }

            // Card 5: Burned Calories (Shows ONLY during training)
            Rectangle {
                visible: window.isTrainingActive
                width: window.width * 0.85; height: 90; color: window.cardTheme; radius: 12; border.color: window.borderTheme
                Column {
                    anchors.centerIn: parent; spacing: 2
                    Text { text: window.tr("BURNED CALORIES"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1; anchors.horizontalCenter: parent.horizontalCenter }
                    Row {
                        spacing: 6; anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            text: {
                                if (typeof vtal.distance === "undefined") return "--"
                                let d = vtal.distance - window.sessionDistanceStart
                                if (d < 0) d = 0
                                let cal = window.currentActivity === "Running" ? d * 0.065 : d * 0.04
                                return cal.toFixed(0)
                            }
                            color: window.textTheme; font.pixelSize: 36; font.bold: true
                        }
                        Text { text: "kcal"; color: "#9C27B0"; font.pixelSize: 14; font.bold: true; anchors.baseline: parent.bottom; anchors.baselineOffset: -6 }
                    }
                }
            }

            // Card 6: Duration (Shows ONLY during training)
            Rectangle {
                visible: window.isTrainingActive
                width: window.width * 0.85; height: 90; color: window.cardTheme; radius: 12; border.color: window.borderTheme
                Column {
                    anchors.centerIn: parent; spacing: 2
                    Text { text: window.tr("DURATION"); color: window.subTextTheme; font.pixelSize: 12; font.bold: true; font.letterSpacing: 1; anchors.horizontalCenter: parent.horizontalCenter }
                    Row {
                        spacing: 6; anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            text: window.formatDuration(window.trainingDuration)
                            color: window.textTheme; font.pixelSize: 36; font.bold: true
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: bottomBar; width: parent.width; height: 70; color: window.bottomBarTheme; anchors.bottom: parent.bottom; border.color: window.borderTheme; border.width: 1
        RowLayout {
            anchors.fill: parent; anchors.margins: 10; spacing: 10
            Button {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1
                background: Rectangle { color: window.btnTheme; radius: 6 }
                contentItem: Text { text: window.tr("Disconnect"); color: "#E53935"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: { vtal.disconnectDevice(); stackView.pop() }
            }

            // Dynamic Training Button
            Button {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1
                background: Rectangle { color: window.isTrainingActive ? "#E53935" : "#4CAF50"; radius: 6 }
                contentItem: Text { text: window.isTrainingActive ? window.tr("Stop Training") : window.tr("New Training"); color: "#FFFFFF"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: {
                    if (window.isTrainingActive) {
                        window.stopTraining()
                    } else {
                        stackView.push("TrainingChoiceScreen.qml")
                    }
                }
            }
            Button {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1
                background: Rectangle { color: window.btnTheme; radius: 6 }
                contentItem: Text { text: window.tr("Settings"); color: window.subTextTheme; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: stackView.push("SettingsScreen.qml")
            }
        }
    }
}