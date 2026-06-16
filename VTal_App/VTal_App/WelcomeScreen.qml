import QtQuick
import QtQuick.Controls

Item {
    StackView.onActivated: {
        // Autofill from memory
        macInput.text = window.savedMacAddress
        connectBtn.text = window.tr("Connect")
    }

    Column {
        anchors.centerIn: parent
        spacing: 25
        Text { text: "VTal Link"; color: window.textTheme; font.pixelSize: 28; font.bold: true; anchors.horizontalCenter: parent.horizontalCenter }

        Column {
            spacing: 8
            anchors.horizontalCenter: parent.horizontalCenter
            Text { text: "HC-05 MAC Address"; color: "#E53935"; font.pixelSize: 14; font.bold: true }
            TextField {
                id: macInput
                width: 300
                color: window.textTheme
                background: Rectangle { color: window.btnTheme; radius: 4; border.color: window.borderTheme }
                font.pixelSize: 18
            }
        }

        Button {
            id: connectBtn
            text: window.tr("Connect")
            width: 200
            anchors.horizontalCenter: parent.horizontalCenter
            background: Rectangle { color: "#1A4C7D"; radius: 4 }
            contentItem: Text { text: parent.text; color: "#FFFFFF"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.bold: true }
            onClicked: {
                if (macInput.text !== "") {
                    // Autosave the MAC address
                    window.savedMacAddress = macInput.text
                    connectBtn.text = window.tr("Connecting...")
                    vtal.connectToHC05(macInput.text)
                }
            }
        }
    }
}