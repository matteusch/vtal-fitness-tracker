#include "FitnessTrackerWindow.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLocale>
#include <QPainter>
#include <QPixmap>
#include <QIcon>

FitnessTrackerWindow::FitnessTrackerWindow(QWidget *parent) : QWidget(parent), m_isDarkMode(true), m_currentLang(EN) {
    setWindowTitle("Fitness Tracker");
    setFixedSize(640, 650);
    setStyleSheet("background-color: #1c1c1e;");

    m_lastBpm = 0; m_lastSpo2 = 0; m_lastSteps = 0; m_lastDistance = 0.0; m_lastCalories = 0;

    stackedWidget = new QStackedWidget(this);

    // --- PAGE 0: START SCREEN ---
    QWidget *startPage = new QWidget();
    QVBoxLayout *startLayout = new QVBoxLayout(startPage);

    startTitleLabel = new QLabel(startPage);
    startTitleLabel->setStyleSheet("color: white; font-size: 42px; font-weight: bold;");
    startTitleLabel->setAlignment(Qt::AlignCenter);

    startButton = new QPushButton(startPage);
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setFixedSize(350, 80);
    startButton->setStyleSheet(
        "QPushButton { background-color: #34c759; color: white; font-size: 24px; font-weight: bold; border-radius: 40px; }"
        "QPushButton:hover { background-color: #32b353; }"
        "QPushButton:pressed { background-color: #288f42; }"
    );

    startLayout->addStretch();
    startLayout->addWidget(startTitleLabel, 0, Qt::AlignCenter);
    startLayout->addSpacing(40);
    startLayout->addWidget(startButton, 0, Qt::AlignCenter);
    startLayout->addStretch();


    // --- PAGE 1: DASHBOARD SCREEN ---
    QWidget *dashboardPage = new QWidget();

    QString labelStyle = "color: #ffffff; font-size: 32px; font-weight: bold; font-family: Arial;";
    QString subStyle = "color: #8e8e93; font-size: 14px; font-weight: bold; letter-spacing: 2px;";

    // 1. Heart Rate
    heartWidget = new PulsingHeartWidget(dashboardPage);
    bpmLabel = new QLabel(dashboardPage);
    bpmLabel->setStyleSheet(labelStyle);
    bpmLabel->setAlignment(Qt::AlignCenter);
    heartSubtitle = new QLabel(dashboardPage);
    heartSubtitle->setStyleSheet(subStyle);
    heartSubtitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *heartLayout = new QVBoxLayout();
    heartLayout->addWidget(heartSubtitle);
    heartLayout->addSpacing(10);
    heartLayout->addWidget(heartWidget, 0, Qt::AlignCenter);
    heartLayout->addSpacing(35);
    heartLayout->addWidget(bpmLabel);

    // 2. Blood Oxygen
    oxygenWidget = new OxygenCircleWidget(dashboardPage);
    spo2Label = new QLabel(dashboardPage);
    spo2Label->setStyleSheet(labelStyle);
    spo2Label->setAlignment(Qt::AlignCenter);
    oxygenSubtitle = new QLabel(dashboardPage);
    oxygenSubtitle->setStyleSheet(subStyle);
    oxygenSubtitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *oxyLayout = new QVBoxLayout();
    oxyLayout->addWidget(oxygenSubtitle);
    oxyLayout->addSpacing(10);
    oxyLayout->addWidget(oxygenWidget, 0, Qt::AlignCenter);
    oxyLayout->addSpacing(35);
    oxyLayout->addWidget(spo2Label);

    // 3. Activity
    activityWidget = new ActivityRingWidget(dashboardPage);
    stepLabel = new QLabel(dashboardPage);
    stepLabel->setStyleSheet(labelStyle);
    stepLabel->setAlignment(Qt::AlignCenter);
    distanceLabel = new QLabel(dashboardPage);
    distanceLabel->setStyleSheet("color: #ff9500; font-size: 18px; font-weight: bold; font-family: Arial;");
    distanceLabel->setAlignment(Qt::AlignCenter);
    activitySubtitle = new QLabel(dashboardPage);
    activitySubtitle->setStyleSheet(subStyle);
    activitySubtitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *stepLayout = new QVBoxLayout();
    stepLayout->addWidget(activitySubtitle);
    stepLayout->addSpacing(10);
    stepLayout->addWidget(activityWidget, 0, Qt::AlignCenter);

    QVBoxLayout *stepTextLayout = new QVBoxLayout();
    stepTextLayout->setSpacing(0);
    stepTextLayout->addWidget(stepLabel);
    stepTextLayout->addWidget(distanceLabel);

    stepLayout->addSpacing(35);
    stepLayout->addLayout(stepTextLayout);

    // 4. Calories
    calorieWidget = new CalorieRingWidget(dashboardPage);
    calorieLabel = new QLabel(dashboardPage);
    calorieLabel->setStyleSheet(labelStyle);
    calorieLabel->setAlignment(Qt::AlignCenter);
    calorieSubtitle = new QLabel(dashboardPage);
    calorieSubtitle->setStyleSheet(subStyle);
    calorieSubtitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *calLayout = new QVBoxLayout();
    calLayout->addWidget(calorieSubtitle);
    calLayout->addSpacing(10);
    calLayout->addWidget(calorieWidget, 0, Qt::AlignCenter);
    calLayout->addSpacing(35);
    calLayout->addWidget(calorieLabel);
    calLayout->addStretch();

    QGridLayout *dashboardLayout = new QGridLayout(dashboardPage);
    dashboardLayout->setContentsMargins(40, 100, 40, 40);
    dashboardLayout->setVerticalSpacing(40);
    dashboardLayout->setHorizontalSpacing(30);

    dashboardLayout->addLayout(heartLayout, 0, 0);
    dashboardLayout->addLayout(oxyLayout,   0, 1);
    dashboardLayout->addLayout(stepLayout,  1, 0);
    dashboardLayout->addLayout(calLayout,   1, 1);

    stackedWidget->addWidget(startPage);
    stackedWidget->addWidget(dashboardPage);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(stackedWidget);

    sensor = new MockSensor(this);
    connect(sensor, &MockSensor::bpmChanged, this, &FitnessTrackerWindow::onBpmChanged);
    connect(sensor, &MockSensor::spo2Changed, this, &FitnessTrackerWindow::onSpo2Changed);
    connect(sensor, &MockSensor::activityChanged, this, &FitnessTrackerWindow::onActivityChanged);

    connect(startButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentIndex(1);
        sensor->startWorkout();
    });

    // --- THE FLOATING UI CONTROLS ---

    // 1. Theme Button
    themeButton = new QPushButton("☀️", this);
    themeButton->setGeometry(width() - 55, 15, 40, 40);
    themeButton->setCursor(Qt::PointingHandCursor);
    themeButton->setStyleSheet("border-radius: 20px; background-color: #2c2c2e; font-size: 20px;");
    connect(themeButton, &QPushButton::clicked, this, &FitnessTrackerWindow::toggleTheme);

    // --- NEW: DYNAMIC FLAG GENERATION ---
    // 1. UK Flag
    QPixmap ukPix(24, 16);
    ukPix.fill(QColor("#012169"));
    QPainter ukPainter(&ukPix);

    ukPainter.setRenderHint(QPainter::Antialiasing, true);

    ukPainter.setPen(QPen(Qt::white, 4));
    ukPainter.drawLine(0, 0, 24, 16);
    ukPainter.drawLine(0, 16, 24, 0);

    ukPainter.setPen(QPen(QColor("#c8102e"), 2));
    ukPainter.drawLine(0, 0, 24, 16);
    ukPainter.drawLine(0, 16, 24, 0);

    ukPainter.setRenderHint(QPainter::Antialiasing, false);

    ukPainter.fillRect(9, 0, 6, 16, Qt::white);
    ukPainter.fillRect(0, 5, 24, 6, Qt::white);

    ukPainter.fillRect(10, 0, 4, 16, QColor("#c8102e"));
    ukPainter.fillRect(0, 6, 24, 4, QColor("#c8102e"));

    ukPainter.end();
    QIcon ukIcon(ukPix);

    // 2. Polish Flag
    QPixmap plPix(24, 16);
    plPix.fill(Qt::white);
    QPainter plPainter(&plPix);
    plPainter.fillRect(0, 8, 24, 8, QColor("#dc143c"));
    plPainter.end();
    QIcon plIcon(plPix);

    // 3. German Flag
    QPixmap dePix(24, 16);
    QPainter dePainter(&dePix);
    dePainter.fillRect(0, 0, 24, 5, QColor("#000000"));
    dePainter.fillRect(0, 5, 24, 6, QColor("#dd0000"));
    dePainter.fillRect(0, 11, 24, 5, QColor("#ffce00"));
    dePainter.end();
    QIcon deIcon(dePix);

    // 2. Language Dropdown
    langComboBox = new QComboBox(this);
    langComboBox->setGeometry(width() - 150, 15, 85, 40);
    langComboBox->setCursor(Qt::PointingHandCursor);
    langComboBox->setIconSize(QSize(24, 16));

    langComboBox->addItem(ukIcon, " EN");
    langComboBox->addItem(plIcon, " PL");
    langComboBox->addItem(deIcon, " DE");

    langComboBox->setStyleSheet(
        "QComboBox { background-color: #2c2c2e; color: white; border-radius: 20px; padding-left: 10px; font-weight: bold; font-family: Arial; }"
        "QComboBox::drop-down { border: 0px; }"
        "QComboBox QAbstractItemView { background-color: #2c2c2e; color: white; selection-background-color: #0a84ff; }"
    );
    connect(langComboBox, &QComboBox::currentIndexChanged, this, &FitnessTrackerWindow::changeLanguage);

    updateStaticTexts();
    onBpmChanged(75);
    onSpo2Changed(98);
    onActivityChanged(0, 0.0, 0);
}

void FitnessTrackerWindow::changeLanguage(int index) {
    m_currentLang = static_cast<Language>(index);
    updateStaticTexts();

    onBpmChanged(m_lastBpm);
    onSpo2Changed(m_lastSpo2);
    onActivityChanged(m_lastSteps, m_lastDistance, m_lastCalories);
}
//Translations

void FitnessTrackerWindow::updateStaticTexts() {
    switch (m_currentLang) {
        case EN:
            startTitleLabel->setText("Ready to train?");
            startButton->setText("START WORKOUT");
            heartSubtitle->setText("HEART RATE");
            oxygenSubtitle->setText("BLOOD OXYGEN");
            activitySubtitle->setText("DAILY STEPS");
            calorieSubtitle->setText("CALORIES BURNED");
            break;
        case PL:
            startTitleLabel->setText("Gotowy na trening?");
            startButton->setText("ROZPOCZNIJ TRENING");
            heartSubtitle->setText("TĘTNO");
            oxygenSubtitle->setText("NATLENIENIE KRWI");
            activitySubtitle->setText("KROKI DZISIAJ");
            calorieSubtitle->setText("SPALONE KALORIE");
            break;
        case DE:
            startTitleLabel->setText("Bereit fürs Training?");
            startButton->setText("TRAINING STARTEN");
            heartSubtitle->setText("HERZFREQUENZ");
            oxygenSubtitle->setText("BLUTSAUERSTOFF");
            activitySubtitle->setText("TÄGLICHE SCHRITTE");
            calorieSubtitle->setText("VERBRANNTE KALORIEN");
            break;
    }
}

//Toggle dark/light theme

void FitnessTrackerWindow::toggleTheme() {
    m_isDarkMode = !m_isDarkMode;

    QString bgColor = m_isDarkMode ? "#1c1c1e" : "#f2f2f7";
    QString textColor = m_isDarkMode ? "#ffffff" : "#000000";
    QString btnBgColor = m_isDarkMode ? "#2c2c2e" : "#d1d1d6";

    setStyleSheet("background-color: " + bgColor + ";");

    themeButton->setText(m_isDarkMode ? "☀️" : "🌙");
    themeButton->setStyleSheet("border-radius: 20px; background-color: " + btnBgColor + "; font-size: 20px;");

    langComboBox->setStyleSheet(
        "QComboBox { background-color: " + btnBgColor + "; color: " + textColor + "; border-radius: 20px; padding-left: 10px; font-weight: bold; font-family: Arial; }"
        "QComboBox::drop-down { border: 0px; }"
        "QComboBox QAbstractItemView { background-color: " + btnBgColor + "; color: " + textColor + "; selection-background-color: #0a84ff; }"
    );

    QString labelStyle = "color: " + textColor + "; font-size: 32px; font-weight: bold; font-family: Arial;";
    bpmLabel->setStyleSheet(labelStyle);
    spo2Label->setStyleSheet(labelStyle);
    stepLabel->setStyleSheet(labelStyle);
    calorieLabel->setStyleSheet(labelStyle);

    startTitleLabel->setStyleSheet("color: " + textColor + "; font-size: 42px; font-weight: bold;");

    heartWidget->setDarkMode(m_isDarkMode);
    oxygenWidget->setDarkMode(m_isDarkMode);
    activityWidget->setDarkMode(m_isDarkMode);
    calorieWidget->setDarkMode(m_isDarkMode);
}

void FitnessTrackerWindow::onBpmChanged(int newBpm) {
    m_lastBpm = newBpm;
    if (newBpm == 0) bpmLabel->setText(m_currentLang == EN ? "Waiting..." : (m_currentLang == PL ? "Oczekiwanie..." : "Warten..."));
    else bpmLabel->setText(QString::number(newBpm) + " BPM");
    heartWidget->updateBpm(newBpm);
}

void FitnessTrackerWindow::onSpo2Changed(int newSpo2) {
    m_lastSpo2 = newSpo2;
    if (newSpo2 == 0) spo2Label->setText(m_currentLang == EN ? "Waiting..." : (m_currentLang == PL ? "Oczekiwanie..." : "Warten..."));
    else spo2Label->setText(QString::number(newSpo2) + " %");
    oxygenWidget->updateSpo2(newSpo2);
}

void FitnessTrackerWindow::onActivityChanged(int newSteps, double newDistance, int newCalories) {
    m_lastSteps = newSteps;
    m_lastDistance = newDistance;
    m_lastCalories = newCalories;

    QString stepsStr = (m_currentLang == EN) ? " STEPS" : ((m_currentLang == PL) ? " KROKI" : " SCHRITTE");

    stepLabel->setText(QLocale().toString(newSteps) + stepsStr);
    distanceLabel->setText(QString::number(newDistance, 'f', 2) + " KM");
    calorieLabel->setText(QLocale().toString(newCalories) + " KCAL");

    activityWidget->updateValue(newSteps);
    calorieWidget->updateValue(newCalories);
}