import QtQuick
import QtQuick.Controls

Item {
    Rectangle {
        id: header
        width: parent.width; height: 60; color: window.bgTheme; anchors.top: parent.top
        Button {
            anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; anchors.margins: 10
            background: Rectangle { color: "transparent" }
            contentItem: Text { text: window.tr("◄ Back"); color: "#4A90E2"; font.pixelSize: 16; font.bold: true; verticalAlignment: Text.AlignVCenter }
            onClicked: stackView.pop()
        }
        Text { text: window.tr("Choose Activity"); color: window.textTheme; font.pixelSize: 20; font.bold: true; anchors.centerIn: parent }
    }

    Column {
        anchors.centerIn: parent
        spacing: 20

        Button {
            width: window.width * 0.8; height: 120
            background: Rectangle { color: "#1A4C7D"; radius: 12 }
            contentItem: Text { text: window.tr("Running"); color: "#FFFFFF"; font.pixelSize: 24; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            onClicked: {
                window.startTraining("Running")
                stackView.pop()
            }
        }

        Button {
            width: window.width * 0.8; height: 120
            background: Rectangle { color: "#4A90E2"; radius: 12 }
            contentItem: Text { text: window.tr("Cycling"); color: "#FFFFFF"; font.pixelSize: 24; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            onClicked: {
                window.startTraining("Cycling")
                stackView.pop()
            }
        }
    }
}