import QtQuick
import QtQuick.Controls

Item {
    Rectangle {
        id: historyHeader
        width: parent.width; height: 60; color: window.bgTheme; anchors.top: parent.top
        Button {
            anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; anchors.margins: 10
            background: Rectangle { color: "transparent" }
            contentItem: Text { text: window.tr("◄ Back"); color: "#4A90E2"; font.pixelSize: 16; font.bold: true; verticalAlignment: Text.AlignVCenter }
            onClicked: stackView.pop()
        }
        Text { text: window.tr("Training History"); color: window.textTheme; font.pixelSize: 20; font.bold: true; anchors.centerIn: parent }
    }

    ListModel {
        id: historyModel
    }

    StackView.onActivated: {
        historyModel.clear()
        let data = JSON.parse(window.trainingHistoryJson)
        for (let i = 0; i < data.length; i++) {
            historyModel.append(data[i])
        }
    }

    ListView {
        anchors.top: historyHeader.bottom; anchors.bottom: parent.bottom
        anchors.left: parent.left; anchors.right: parent.right
        anchors.margins: 20
        spacing: 15
        model: historyModel
        clip: true

        delegate: Rectangle {
            width: parent.width; height: 80
            color: window.cardTheme; radius: 8; border.color: window.borderTheme

            Row {
                anchors.fill: parent; anchors.margins: 15; spacing: 20

                // Left side: Activity & Date
                Column {
                    width: parent.width * 0.45; spacing: 4
                    Text { text: window.tr(model.activity); color: window.textTheme; font.pixelSize: 18; font.bold: true }
                    Text { text: model.date; color: window.subTextTheme; font.pixelSize: 12 }
                }

                // Right side: Stats
                                Column {
                                    width: parent.width * 0.45; spacing: 4; anchors.verticalCenter: parent.verticalCenter

                                    Text { text: model.duration; color: window.textTheme; font.pixelSize: 16; font.bold: true }

                                    Text {
                                        text: (window.unitSystem === "Metric" ? model.distance : (model.distance * 0.621371).toFixed(2)) + (window.unitSystem === "Metric" ? " km" : " mi")
                                        color: "#FF9800"; font.pixelSize: 14; font.bold: true
                                    }
                                    Text { text: model.calories + " kcal"; color: "#9C27B0"; font.pixelSize: 14; font.bold: true }
                                }
            }
        }

        // If the history is empty
        Text {
            visible: historyModel.count === 0
            text: "No history yet."
            color: window.subTextTheme
            font.pixelSize: 16
            anchors.centerIn: parent
        }
    }
}