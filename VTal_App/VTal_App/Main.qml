import QtQuick
import QtQuick.Controls
import QtCore

ApplicationWindow {
    id: window
    width: 400
    height: 600
    visible: true
    title: "VTal Telemetry"

    //MEmory
    Settings {
        id: appSettings
        property string savedMacAddress: ""
        property bool isDarkMode: true
        property string unitSystem: "Metric"
        property string appLanguage: "English"
        property string trainingHistoryJson: "[]" // Saves workouts as a JSON array
    }

    property alias savedMacAddress: appSettings.savedMacAddress
    property alias isDarkMode: appSettings.isDarkMode
    property alias unitSystem: appSettings.unitSystem
    property alias appLanguage: appSettings.appLanguage
    property alias trainingHistoryJson: appSettings.trainingHistoryJson

    // Training State Variables
    property bool isTrainingActive: false
    property string currentActivity: ""
    property real sessionDistanceStart: 0
    property int trainingDuration: 0 // Tracks total seconds

    // Dynamic Theme Colors
    readonly property color bgTheme: isDarkMode ? "#121212" : "#F5F5F7"
    readonly property color cardTheme: isDarkMode ? "#1E1E1E" : "#FFFFFF"
    readonly property color textTheme: isDarkMode ? "#FFFFFF" : "#1D1D1F"
    readonly property color subTextTheme: isDarkMode ? "#888888" : "#86868B"
    readonly property color borderTheme: isDarkMode ? "#2C2C2C" : "#E5E5EA"
    readonly property color bottomBarTheme: isDarkMode ? "#1A1A1A" : "#FFFFFF"
    readonly property color btnTheme: isDarkMode ? "#2C2C2C" : "#E5E5EA"

    color: bgTheme

    // Aactive timer
    Timer {
        id: trainingTimer
        interval: 1000
        repeat: true
        running: window.isTrainingActive
        onTriggered: window.trainingDuration += 1
    }


    function formatDuration(sec) {
        let h = Math.floor(sec / 3600);
        let m = Math.floor((sec % 3600) / 60);
        let s = sec % 60;
        if (h > 0) return h + ":" + (m < 10 ? "0" : "") + m + ":" + (s < 10 ? "0" : "") + s;
        return (m < 10 ? "0" : "") + m + ":" + (s < 10 ? "0" : "") + s;
    }

    function startTraining(activity) {
        currentActivity = activity
        isTrainingActive = true
        sessionDistanceStart = typeof vtal.distance !== "undefined" ? vtal.distance : 0
        trainingDuration = 0 // Reset the clock
    }

    function stopTraining() {
        isTrainingActive = false
        let endDist = typeof vtal.distance !== "undefined" ? vtal.distance : 0
        let sessionDist = endDist - sessionDistanceStart
        if (sessionDist < 0) sessionDist = 0

        let calories = currentActivity === "Running" ? sessionDist * 65 : sessionDist * 40

        let history = JSON.parse(trainingHistoryJson)
        history.unshift({
            "activity": currentActivity,
            "distance": sessionDist.toFixed(2),
            "calories": calories.toFixed(0),
            "duration": formatDuration(trainingDuration), // Save formatted time
            "date": new Date().toLocaleDateString()
        })
        trainingHistoryJson = JSON.stringify(history)
        currentActivity = ""
    }

    // Translation dictionary
    function tr(text) {
        if (appLanguage === "Polski") {
            if (text === "HEART RATE") return "TĘTNO"
            if (text === "AMBIENT TEMP") return "TEMP. OTOCZENIA"
            if (text === "STEP COUNTER") return "LICZBA KROKÓW"
            if (text === "DISTANCE COVERED") return "POKONANY DYSTANS"
            if (text === "BURNED CALORIES") return "SPALONE KALORIE"
            if (text === "DURATION") return "CZAS TRWANIA"
            if (text === "Overview") return "Przegląd"
            if (text === "Settings") return "Ustawienia"
            if (text === "Disconnect") return "Rozłącz"
            if (text === "New Training") return "Nowy Trening"
            if (text === "Stop Training") return "Zakończ Trening"
            if (text === "Choose Activity") return "Wybierz Aktywność"
            if (text === "Running") return "Bieganie"
            if (text === "Cycling") return "Jazda na Rowerze"
            if (text === "Training History") return "Historia Treningów"
            if (text === "Connect") return "Połącz"
            if (text === "Connecting...") return "Łączenie..."
            if (text === "APPEARANCE") return "WYGLĄD"
            if (text === "Dark Mode") return "Tryb Ciemny"
            if (text === "PREFERENCES") return "PREFERENCJE"
            if (text === "Language") return "Język"
            if (text === "Units") return "Jednostki"
            if (text === "DATA") return "DANE"
            if (text === "Clear Session History") return "Wyczyść Historię Sesji"
            if (text === "CONNECTION") return "POŁĄCZENIE"
            if (text === "Saved MAC Address") return "Zapisany Adres MAC"
            if (text === "Save") return "Zapisz"
            if (text === "◄ Back") return "◄ Wróć"
        } else if (appLanguage === "Deutsch") {
            if (text === "HEART RATE") return "HERZFREQUENZ"
            if (text === "AMBIENT TEMP") return "UMGEBUNGSTEMP."
            if (text === "STEP COUNTER") return "SCHRITTZÄHLER"
            if (text === "DISTANCE COVERED") return "ZURÜCKGELEGTE STRECKE"
            if (text === "BURNED CALORIES") return "VERBRANNTE KALORIEN"
            if (text === "DURATION") return "DAUER"
            if (text === "Overview") return "Übersicht"
            if (text === "Settings") return "Einstellungen"
            if (text === "Disconnect") return "Trennen"
            if (text === "New Training") return "Neues Training"
            if (text === "Stop Training") return "Training Beenden"
            if (text === "Choose Activity") return "Aktivität Wählen"
            if (text === "Running") return "Laufen"
            if (text === "Cycling") return "Radfahren"
            if (text === "Training History") return "Trainingsverlauf"
            if (text === "Connect") return "Verbinden"
            if (text === "Connecting...") return "Verbinde..."
            if (text === "APPEARANCE") return "ERSCHEINUNGSBILD"
            if (text === "Dark Mode") return "Dunkelmodus"
            if (text === "PREFERENCES") return "EINSTELLUNGEN"
            if (text === "Language") return "Sprache"
            if (text === "Units") return "Einheiten"
            if (text === "DATA") return "DATEN"
            if (text === "Clear Session History") return "Sitzungsverlauf löschen"
            if (text === "CONNECTION") return "VERBINDUNG"
            if (text === "Saved MAC Address") return "Gespeicherte MAC-Adresse"
            if (text === "Save") return "Speichern"
            if (text === "◄ Back") return "◄ Zurück"
        }
        return text
    }

    Connections {
        target: vtal
        function onDeviceConnected() { stackView.push("DashboardScreen.qml") }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: "WelcomeScreen.qml"
    }
}