#include "FitnessTrackerWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLocale>
#include <QPropertyAnimation>
#include <QPainter>
#include <QPixmap>
#include <QIcon>

FitnessTrackerWindow::FitnessTrackerWindow(QWidget *parent) : QWidget(parent), m_isDarkMode(true), m_currentLang(EN), m_isWorkoutPaused(false) {
    setWindowTitle("Fitness Tracker");
    setFixedSize(640, 700);
    setStyleSheet("background-color: #1c1c1e;");

    stackedWidget = new QStackedWidget(this);
    opacityEffect = new QGraphicsOpacityEffect(this);
    stackedWidget->setGraphicsEffect(opacityEffect);

    sensor = new MockSensor(this);

    // --- NEW: Master Stylesheets with tactile "Press" animations and "Disabled" states ---
    QString titleStyle = "color: white; font-size: 36px; font-weight: bold;";
    QString labelStyle = "color: #ffffff; font-size: 32px; font-weight: bold; font-family: Arial;";
    QString subStyle = "color: #8e8e93; font-size: 14px; font-weight: bold; letter-spacing: 2px;";

    QString btnStyle =
        "QPushButton { background-color: #34c759; color: white; font-size: 20px; font-weight: bold; border-radius: 30px; }"
        "QPushButton:hover { background-color: #32b353; }"
        "QPushButton:pressed { background-color: #288f42; padding-top: 3px; }" // The physical "push" animation!
        "QPushButton:disabled { background-color: #3a3a3c; color: #8e8e93; }"; // The grayed-out state!

    QString secondaryBtnStyle =
        "QPushButton { background-color: #48484a; color: white; font-size: 16px; font-weight: bold; border-radius: 20px; }"
        "QPushButton:hover { background-color: #5c5c5e; }"
        "QPushButton:pressed { background-color: #3a3a3c; padding-top: 3px; }"
        "QPushButton:disabled { background-color: #2c2c2e; color: #636366; }";

    // ==========================================
    // PAGE 0: HOME SCREEN
    // ==========================================
    QWidget *homePage = new QWidget();
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);
    homeLayout->setContentsMargins(40, 80, 40, 40);

    homeTitle = new QLabel();
    homeTitle->setStyleSheet(titleStyle);
    homeTitle->setAlignment(Qt::AlignCenter);

    QHBoxLayout *dailyWidgetsLayout = new QHBoxLayout();

    QVBoxLayout *hHeart = new QVBoxLayout();
    homeHeartSub = new QLabel(); homeHeartSub->setStyleSheet(subStyle); homeHeartSub->setAlignment(Qt::AlignCenter);
    homeHeartWidget = new PulsingHeartWidget();
    homeBpmLabel = new QLabel("-- BPM"); homeBpmLabel->setStyleSheet(labelStyle); homeBpmLabel->setAlignment(Qt::AlignCenter);
    hHeart->addWidget(homeHeartSub); hHeart->addWidget(homeHeartWidget, 0, Qt::AlignCenter); hHeart->addWidget(homeBpmLabel);

    QVBoxLayout *hSteps = new QVBoxLayout();
    homeStepsSub = new QLabel(); homeStepsSub->setStyleSheet(subStyle); homeStepsSub->setAlignment(Qt::AlignCenter);
    homeStepsWidget = new ActivityRingWidget();
    homeStepsLabel = new QLabel("--"); homeStepsLabel->setStyleSheet(labelStyle); homeStepsLabel->setAlignment(Qt::AlignCenter);
    hSteps->addWidget(homeStepsSub); hSteps->addWidget(homeStepsWidget, 0, Qt::AlignCenter); hSteps->addWidget(homeStepsLabel);

    dailyWidgetsLayout->addLayout(hHeart);
    dailyWidgetsLayout->addLayout(hSteps);

    startTrainingBtn = new QPushButton();
    startTrainingBtn->setFixedSize(300, 60);
    startTrainingBtn->setStyleSheet(btnStyle);
    startTrainingBtn->setCursor(Qt::PointingHandCursor);

    connect(startTrainingBtn, &QPushButton::clicked, this, [this](){
        if (sensor->isWorkoutActive()) fadeToPage(2);
        else fadeToPage(1);
    });

    homeLayout->addWidget(homeTitle);
    homeLayout->addStretch();
    homeLayout->addLayout(dailyWidgetsLayout);
    homeLayout->addStretch();
    homeLayout->addWidget(startTrainingBtn, 0, Qt::AlignCenter);

    // ==========================================
    // PAGE 1: ACTIVITY SELECTION
    // ==========================================
    QWidget *selectPage = new QWidget();
    QVBoxLayout *selectLayout = new QVBoxLayout(selectPage);
    selectLayout->setContentsMargins(40, 100, 40, 40);

    selectTitle = new QLabel();
    selectTitle->setStyleSheet(titleStyle);
    selectTitle->setAlignment(Qt::AlignCenter);

    runBtn = new QPushButton("🏃 Running"); runBtn->setFixedSize(300, 60); runBtn->setStyleSheet(btnStyle); runBtn->setCursor(Qt::PointingHandCursor);
    cycleBtn = new QPushButton("🚴 Cycling"); cycleBtn->setFixedSize(300, 60); cycleBtn->setStyleSheet(btnStyle); cycleBtn->setCursor(Qt::PointingHandCursor);
    swimBtn = new QPushButton("🏊 Swimming"); swimBtn->setFixedSize(300, 60); swimBtn->setStyleSheet(btnStyle); swimBtn->setCursor(Qt::PointingHandCursor);
    cancelSelectionBtn = new QPushButton(); cancelSelectionBtn->setFixedSize(300, 60); cancelSelectionBtn->setStyleSheet(secondaryBtnStyle); cancelSelectionBtn->setCursor(Qt::PointingHandCursor);

    auto startSpecificWorkout = [this](MockSensor::WorkoutType type) {
        sensor->startWorkout(type);
        m_isWorkoutPaused = false;

        pauseBtn->setEnabled(true);
        endBtn->setEnabled(true);

        onWorkoutUpdated(0, 75, 98, 0.0, 0);
        updateStaticTexts();
        fadeToPage(2);
    };

    connect(runBtn, &QPushButton::clicked, this, [=](){ startSpecificWorkout(MockSensor::Running); });
    connect(cycleBtn, &QPushButton::clicked, this, [=](){ startSpecificWorkout(MockSensor::Cycling); });
    connect(swimBtn, &QPushButton::clicked, this, [=](){ startSpecificWorkout(MockSensor::Swimming); });
    connect(cancelSelectionBtn, &QPushButton::clicked, this, [=](){ fadeToPage(0); });

    selectLayout->addWidget(selectTitle);
    selectLayout->addSpacing(40);
    selectLayout->addWidget(runBtn, 0, Qt::AlignCenter);
    selectLayout->addWidget(cycleBtn, 0, Qt::AlignCenter);
    selectLayout->addWidget(swimBtn, 0, Qt::AlignCenter);
    selectLayout->addStretch();
    selectLayout->addWidget(cancelSelectionBtn, 0, Qt::AlignCenter);

    // ==========================================
    // PAGE 2: ACTIVE TRAINING
    // ==========================================
    QWidget *workoutPage = new QWidget();
    QVBoxLayout *workoutLayout = new QVBoxLayout(workoutPage);
    workoutLayout->setContentsMargins(20, 80, 20, 20);

    workTimeSub = new QLabel(); workTimeSub->setStyleSheet(subStyle); workTimeSub->setAlignment(Qt::AlignCenter);
    workoutTimeLabel = new QLabel("00:00");
    workoutTimeLabel->setStyleSheet("color: #ff9500; font-size: 48px; font-weight: bold; font-family: monospace;");
    workoutTimeLabel->setAlignment(Qt::AlignCenter);

    QGridLayout *metricsGrid = new QGridLayout();

    workoutHeartWidget = new PulsingHeartWidget();
    workoutBpmLabel = new QLabel("--"); workoutBpmLabel->setStyleSheet(labelStyle); workoutBpmLabel->setAlignment(Qt::AlignCenter);
    workHeartSub = new QLabel(); workHeartSub->setStyleSheet(subStyle); workHeartSub->setAlignment(Qt::AlignCenter);
    QVBoxLayout *wHeart = new QVBoxLayout(); wHeart->addWidget(workHeartSub); wHeart->addWidget(workoutHeartWidget, 0, Qt::AlignCenter); wHeart->addWidget(workoutBpmLabel);

    workoutSpo2Widget = new OxygenCircleWidget();
    workoutSpo2Label = new QLabel("--"); workoutSpo2Label->setStyleSheet(labelStyle); workoutSpo2Label->setAlignment(Qt::AlignCenter);
    workSpo2Sub = new QLabel(); workSpo2Sub->setStyleSheet(subStyle); workSpo2Sub->setAlignment(Qt::AlignCenter);
    QVBoxLayout *wSpo2 = new QVBoxLayout(); wSpo2->addWidget(workSpo2Sub); wSpo2->addWidget(workoutSpo2Widget, 0, Qt::AlignCenter); wSpo2->addWidget(workoutSpo2Label);

    workDistSub = new QLabel(); workDistSub->setStyleSheet(subStyle); workDistSub->setAlignment(Qt::AlignCenter);
    workoutDistanceLabel = new QLabel("0.00 KM"); workoutDistanceLabel->setStyleSheet(labelStyle); workoutDistanceLabel->setAlignment(Qt::AlignCenter);
    QVBoxLayout *wDist = new QVBoxLayout(); wDist->addWidget(workDistSub); wDist->addWidget(workoutDistanceLabel);

    workCalSub = new QLabel(); workCalSub->setStyleSheet(subStyle); workCalSub->setAlignment(Qt::AlignCenter);
    workoutCalorieLabel = new QLabel("0 KCAL"); workoutCalorieLabel->setStyleSheet(labelStyle); workoutCalorieLabel->setAlignment(Qt::AlignCenter);
    QVBoxLayout *wCal = new QVBoxLayout(); wCal->addWidget(workCalSub); wCal->addWidget(workoutCalorieLabel);

    metricsGrid->addLayout(wHeart, 0, 0); metricsGrid->addLayout(wSpo2, 0, 1);
    metricsGrid->addLayout(wDist, 1, 0);  metricsGrid->addLayout(wCal, 1, 1);

    // --- Control Buttons with Animated Stylesheets ---
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    QString controlBtnBase = "font-size: 16px; font-weight: bold; border-radius: 25px; font-family: Arial;";

    pauseBtn = new QPushButton();
    pauseBtn->setFixedSize(115, 50);
    pauseBtn->setCursor(Qt::PointingHandCursor);
    pauseBtn->setStyleSheet(
        "QPushButton { background-color: #ffcc00; color: black; " + controlBtnBase + "}"
        "QPushButton:hover { background-color: #ffd633; }"
        "QPushButton:pressed { background-color: #e6b800; padding-top: 3px; }"
        "QPushButton:disabled { background-color: #3a3a3c; color: #8e8e93; }" // Turns gray when ended!
    );

    endBtn = new QPushButton();
    endBtn->setFixedSize(115, 50);
    endBtn->setCursor(Qt::PointingHandCursor);
    endBtn->setStyleSheet(
        "QPushButton { background-color: #ff3b30; color: white; " + controlBtnBase + "}"
        "QPushButton:hover { background-color: #ff4d42; }"
        "QPushButton:pressed { background-color: #cc2e26; padding-top: 3px; }"
        "QPushButton:disabled { background-color: #3a3a3c; color: #8e8e93; }" // Turns gray when ended!
    );

    homeBtn = new QPushButton();
    homeBtn->setFixedSize(115, 50);
    homeBtn->setCursor(Qt::PointingHandCursor);
    homeBtn->setStyleSheet(secondaryBtnStyle); // Uses our master secondary style

    connect(pauseBtn, &QPushButton::clicked, this, [this]() {
        if(m_isWorkoutPaused) { sensor->resumeWorkout(); m_isWorkoutPaused = false; }
        else { sensor->pauseWorkout(); m_isWorkoutPaused = true; }
        updateStaticTexts();
    });

    connect(endBtn, &QPushButton::clicked, this, [this]() {
        sensor->stopWorkout();

        // Disable the buttons! The new stylesheet handles turning them gray automatically.
        pauseBtn->setEnabled(false);
        endBtn->setEnabled(false);

        updateStaticTexts();
    });

    connect(homeBtn, &QPushButton::clicked, this, [this]() {
        updateStaticTexts();
        fadeToPage(0);
    });

    controlsLayout->addWidget(pauseBtn);
    controlsLayout->addWidget(endBtn);
    controlsLayout->addWidget(homeBtn);

    workoutLayout->addWidget(workTimeSub);
    workoutLayout->addWidget(workoutTimeLabel);
    workoutLayout->addSpacing(20);
    workoutLayout->addLayout(metricsGrid);
    workoutLayout->addStretch();
    workoutLayout->addLayout(controlsLayout);

    // ==========================================
    // FINAL WIRING
    // ==========================================
    stackedWidget->addWidget(homePage);
    stackedWidget->addWidget(selectPage);
    stackedWidget->addWidget(workoutPage);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(stackedWidget);

    themeButton = new QPushButton("☀️", this);
    themeButton->setGeometry(width() - 55, 15, 40, 40);
    themeButton->setCursor(Qt::PointingHandCursor);
    themeButton->setStyleSheet(
        "QPushButton { border-radius: 20px; background-color: #2c2c2e; font-size: 20px; }"
        "QPushButton:pressed { padding-top: 2px; }" // Tiny bounce on the theme button
    );
    connect(themeButton, &QPushButton::clicked, this, &FitnessTrackerWindow::toggleTheme);

    QPixmap ukPix(24, 16); ukPix.fill(QColor("#012169")); QPainter ukPainter(&ukPix);
    ukPainter.setRenderHint(QPainter::Antialiasing, true); ukPainter.setPen(QPen(Qt::white, 4));
    ukPainter.drawLine(0, 0, 24, 16); ukPainter.drawLine(0, 16, 24, 0);
    ukPainter.setPen(QPen(QColor("#c8102e"), 2)); ukPainter.drawLine(0, 0, 24, 16); ukPainter.drawLine(0, 16, 24, 0);
    ukPainter.setRenderHint(QPainter::Antialiasing, false); ukPainter.fillRect(9, 0, 6, 16, Qt::white); ukPainter.fillRect(0, 5, 24, 6, Qt::white);
    ukPainter.fillRect(10, 0, 4, 16, QColor("#c8102e")); ukPainter.fillRect(0, 6, 24, 4, QColor("#c8102e")); ukPainter.end(); QIcon ukIcon(ukPix);

    QPixmap plPix(24, 16); plPix.fill(Qt::white); QPainter plPainter(&plPix);
    plPainter.fillRect(0, 8, 24, 8, QColor("#dc143c")); plPainter.end(); QIcon plIcon(plPix);

    QPixmap dePix(24, 16); QPainter dePainter(&dePix);
    dePainter.fillRect(0, 0, 24, 5, QColor("#000000")); dePainter.fillRect(0, 5, 24, 6, QColor("#dd0000")); dePainter.fillRect(0, 11, 24, 5, QColor("#ffce00"));
    dePainter.end(); QIcon deIcon(dePix);

    langComboBox = new QComboBox(this);
    langComboBox->setGeometry(width() - 150, 15, 85, 40);
    langComboBox->setCursor(Qt::PointingHandCursor);
    langComboBox->setIconSize(QSize(24, 16));
    langComboBox->addItem(ukIcon, " EN"); langComboBox->addItem(plIcon, " PL"); langComboBox->addItem(deIcon, " DE");
    langComboBox->setStyleSheet("QComboBox { background-color: #2c2c2e; color: white; border-radius: 20px; padding-left: 10px; font-weight: bold; font-family: Arial; } QComboBox::drop-down { border: 0px; } QComboBox QAbstractItemView { background-color: #2c2c2e; color: white; selection-background-color: #0a84ff; }");
    connect(langComboBox, &QComboBox::currentIndexChanged, this, &FitnessTrackerWindow::changeLanguage);

    connect(sensor, &MockSensor::dailyDataUpdated, this, &FitnessTrackerWindow::onDailyUpdated);
    connect(sensor, &MockSensor::workoutDataUpdated, this, &FitnessTrackerWindow::onWorkoutUpdated);

    updateStaticTexts();
}

void FitnessTrackerWindow::fadeToPage(int pageIndex) {
    QPropertyAnimation *fadeOut = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeOut->setDuration(250); fadeOut->setStartValue(1.0); fadeOut->setEndValue(0.0);
    connect(fadeOut, &QPropertyAnimation::finished, this, [this, pageIndex]() {
        stackedWidget->setCurrentIndex(pageIndex);
        QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity", this);
        fadeIn->setDuration(250); fadeIn->setStartValue(0.0); fadeIn->setEndValue(1.0);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    });
    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

void FitnessTrackerWindow::formatTimeLabel(int seconds) {
    int m = seconds / 60; int s = seconds % 60;
    workoutTimeLabel->setText(QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')));
}

void FitnessTrackerWindow::onDailyUpdated(int bpm, int steps) {
    homeBpmLabel->setText(QString::number(bpm) + " BPM"); homeHeartWidget->updateBpm(bpm);
    homeStepsLabel->setText(QLocale().toString(steps)); homeStepsWidget->updateValue(steps);
}

void FitnessTrackerWindow::onWorkoutUpdated(int seconds, int bpm, int spo2, double distance, int calories) {
    formatTimeLabel(seconds);
    workoutBpmLabel->setText(QString::number(bpm) + " BPM"); workoutHeartWidget->updateBpm(bpm);
    workoutSpo2Label->setText(QString::number(spo2) + " %"); workoutSpo2Widget->updateSpo2(spo2);
    workoutDistanceLabel->setText(QString::number(distance, 'f', 2) + " KM");
    workoutCalorieLabel->setText(QString::number(calories) + " KCAL");
}

void FitnessTrackerWindow::changeLanguage(int index) {
    m_currentLang = static_cast<Language>(index);
    updateStaticTexts();
}

void FitnessTrackerWindow::updateStaticTexts() {
    bool active = sensor->isWorkoutActive();

    switch (m_currentLang) {
        case EN:
            homeTitle->setText("Daily Summary");
            homeHeartSub->setText("HEART RATE"); homeStepsSub->setText("DAILY STEPS");
            startTrainingBtn->setText(active ? "RETURN TO TRAINING" : "START TRAINING");
            selectTitle->setText("Choose Activity"); cancelSelectionBtn->setText("Back");
            workTimeSub->setText("ELAPSED TIME"); workHeartSub->setText("PULSE"); workSpo2Sub->setText("SpO2");
            workDistSub->setText("DISTANCE"); workCalSub->setText("CALORIES");
            if(!m_isWorkoutPaused) pauseBtn->setText("PAUSE"); else pauseBtn->setText("RESUME");
            endBtn->setText("END"); homeBtn->setText("HOME");
            break;
        case PL:
            homeTitle->setText("Podsumowanie Dnia");
            homeHeartSub->setText("TĘTNO"); homeStepsSub->setText("KROKI DZISIAJ");
            startTrainingBtn->setText(active ? "WRÓĆ DO TRENINGU" : "ZACZNIJ TRENING");
            selectTitle->setText("Wybierz Aktywność"); cancelSelectionBtn->setText("Wróć");
            workTimeSub->setText("CZAS TRENINGU"); workHeartSub->setText("TĘTNO"); workSpo2Sub->setText("SpO2");
            workDistSub->setText("DYSTANS"); workCalSub->setText("KALORIE");
            if(!m_isWorkoutPaused) pauseBtn->setText("PAUZA"); else pauseBtn->setText("WZNÓW");
            endBtn->setText("KONIEC"); homeBtn->setText("PULPIT");
            break;
        case DE:
            homeTitle->setText("Tägliche Übersicht");
            homeHeartSub->setText("HERZFREQUENZ"); homeStepsSub->setText("TÄGLICHE SCHRITTE");
            startTrainingBtn->setText(active ? "ZURÜCK ZUM TRAINING" : "TRAINING STARTEN");
            selectTitle->setText("Aktivität Wählen"); cancelSelectionBtn->setText("Zurück");
            workTimeSub->setText("VERSTRICHENE ZEIT"); workHeartSub->setText("PULS"); workSpo2Sub->setText("SpO2");
            workDistSub->setText("DISTANZ"); workCalSub->setText("KALORIEN");
            if(!m_isWorkoutPaused) pauseBtn->setText("PAUSE"); else pauseBtn->setText("WEITER");
            endBtn->setText("BEENDEN"); homeBtn->setText("HOME");
            break;
    }
}

void FitnessTrackerWindow::toggleTheme() {
    m_isDarkMode = !m_isDarkMode;
    QString bgColor = m_isDarkMode ? "#1c1c1e" : "#f2f2f7";
    QString textColor = m_isDarkMode ? "#ffffff" : "#000000";
    setStyleSheet("background-color: " + bgColor + ";");

    QString labelStyle = "color: " + textColor + "; font-size: 32px; font-weight: bold; font-family: Arial;";
    homeTitle->setStyleSheet("color: " + textColor + "; font-size: 36px; font-weight: bold;");
    selectTitle->setStyleSheet("color: " + textColor + "; font-size: 36px; font-weight: bold;");
    homeBpmLabel->setStyleSheet(labelStyle); homeStepsLabel->setStyleSheet(labelStyle);
    workoutBpmLabel->setStyleSheet(labelStyle); workoutSpo2Label->setStyleSheet(labelStyle);
    workoutDistanceLabel->setStyleSheet(labelStyle); workoutCalorieLabel->setStyleSheet(labelStyle);

    homeHeartWidget->setDarkMode(m_isDarkMode); homeStepsWidget->setDarkMode(m_isDarkMode);
    workoutHeartWidget->setDarkMode(m_isDarkMode); workoutSpo2Widget->setDarkMode(m_isDarkMode);

    themeButton->setText(m_isDarkMode ? "☀️" : "🌙");
    themeButton->setStyleSheet(
        "QPushButton { border-radius: 20px; background-color: " + (m_isDarkMode ? QString("#2c2c2e") : QString("#d1d1d6")) + "; font-size: 20px; }"
        "QPushButton:pressed { padding-top: 2px; }"
    );

    langComboBox->setStyleSheet(
        "QComboBox { background-color: " + (m_isDarkMode ? QString("#2c2c2e") : QString("#d1d1d6")) + "; color: " + textColor + "; border-radius: 20px; padding-left: 10px; font-weight: bold; font-family: Arial; }"
        "QComboBox::drop-down { border: 0px; }"
        "QComboBox QAbstractItemView { background-color: " + (m_isDarkMode ? QString("#2c2c2e") : QString("#d1d1d6")) + "; color: " + textColor + "; selection-background-color: #0a84ff; }"
    );
}