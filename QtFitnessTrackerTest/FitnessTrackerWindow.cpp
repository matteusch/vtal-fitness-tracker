#include "FitnessTrackerWindow.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLocale>

FitnessTrackerWindow::FitnessTrackerWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Fitness Tracker");
    setFixedSize(640, 500); 
    setStyleSheet("background-color: #1c1c1e;");

    stackedWidget = new QStackedWidget(this);

    // --- PAGE 0: START SCREEN ---
    QWidget *startPage = new QWidget();
    QVBoxLayout *startLayout = new QVBoxLayout(startPage);

    QLabel *titleLabel = new QLabel("Ready to train?", startPage);
    titleLabel->setStyleSheet("color: white; font-size: 42px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QPushButton *startButton = new QPushButton("START WORKOUT", startPage);
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setFixedSize(350, 80);
    startButton->setStyleSheet(
        "QPushButton { background-color: #34c759; color: white; font-size: 24px; font-weight: bold; border-radius: 40px; }"
        "QPushButton:hover { background-color: #32b353; }"
        "QPushButton:pressed { background-color: #288f42; }"
    );

    startLayout->addStretch();
    startLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    startLayout->addSpacing(40);
    startLayout->addWidget(startButton, 0, Qt::AlignCenter);
    startLayout->addStretch();


    // --- PAGE 1: DASHBOARD SCREEN ---
    QWidget *dashboardPage = new QWidget(); 

    QString labelStyle = "color: #ffffff; font-size: 32px; font-weight: bold; font-family: Arial;";
    QString subStyle = "color: #8e8e93; font-size: 14px; font-weight: bold; letter-spacing: 2px;";

    // 1. Heart Rate
    heartWidget = new PulsingHeartWidget(dashboardPage);
    bpmLabel = new QLabel("Waiting...", dashboardPage);
    bpmLabel->setStyleSheet(labelStyle);
    bpmLabel->setAlignment(Qt::AlignCenter);
    QLabel *heartSubtitle = new QLabel("HEART RATE", dashboardPage);
    heartSubtitle->setStyleSheet(subStyle);
    heartSubtitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *heartLayout = new QVBoxLayout();
    heartLayout->addWidget(heartSubtitle);
    heartLayout->addWidget(heartWidget, 0, Qt::AlignCenter);
    heartLayout->addWidget(bpmLabel);

    // 2. Blood Oxygen
    oxygenWidget = new OxygenCircleWidget(dashboardPage);
    spo2Label = new QLabel("Waiting...", dashboardPage);
    spo2Label->setStyleSheet(labelStyle);
    spo2Label->setAlignment(Qt::AlignCenter);
    QLabel *oxygenSubtitle = new QLabel("BLOOD OXYGEN", dashboardPage);
    oxygenSubtitle->setStyleSheet(subStyle);
    oxygenSubtitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *oxyLayout = new QVBoxLayout();
    oxyLayout->addWidget(oxygenSubtitle);
    oxyLayout->addWidget(oxygenWidget, 0, Qt::AlignCenter);
    oxyLayout->addWidget(spo2Label);

    // 3. Activity
    activityWidget = new ActivityRingWidget(dashboardPage);
    stepLabel = new QLabel("Waiting...", dashboardPage);
    stepLabel->setStyleSheet(labelStyle);
    stepLabel->setAlignment(Qt::AlignCenter);
    distanceLabel = new QLabel("Waiting...", dashboardPage);
    distanceLabel->setStyleSheet("color: #ff9500; font-size: 18px; font-weight: bold; font-family: Arial;");
    distanceLabel->setAlignment(Qt::AlignCenter);
    QLabel *activitySubtitle = new QLabel("DAILY STEPS", dashboardPage);
    activitySubtitle->setStyleSheet(subStyle);
    activitySubtitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *stepLayout = new QVBoxLayout();
    stepLayout->addWidget(activitySubtitle);
    stepLayout->addWidget(activityWidget, 0, Qt::AlignCenter);
    QVBoxLayout *stepTextLayout = new QVBoxLayout();
    stepTextLayout->setSpacing(0); 
    stepTextLayout->addWidget(stepLabel);
    stepTextLayout->addWidget(distanceLabel);
    stepLayout->addLayout(stepTextLayout);

    // 4. Calories
    calorieWidget = new CalorieRingWidget(dashboardPage);
    calorieLabel = new QLabel("Waiting...", dashboardPage);
    calorieLabel->setStyleSheet(labelStyle);
    calorieLabel->setAlignment(Qt::AlignCenter);
    QLabel *calorieSubtitle = new QLabel("CALORIES BURNED", dashboardPage);
    calorieSubtitle->setStyleSheet(subStyle);
    calorieSubtitle->setAlignment(Qt::AlignCenter);

    QVBoxLayout *calLayout = new QVBoxLayout();
    calLayout->addWidget(calorieSubtitle);
    calLayout->addWidget(calorieWidget, 0, Qt::AlignCenter);
    calLayout->addWidget(calorieLabel);
    calLayout->addStretch(); 

    QGridLayout *dashboardLayout = new QGridLayout(dashboardPage);
    dashboardLayout->setContentsMargins(40, 40, 40, 40);
    dashboardLayout->setVerticalSpacing(50);
    dashboardLayout->setHorizontalSpacing(30);

    dashboardLayout->addLayout(heartLayout, 0, 0); 
    dashboardLayout->addLayout(oxyLayout,   0, 1); 
    dashboardLayout->addLayout(stepLayout,  1, 0); 
    dashboardLayout->addLayout(calLayout,   1, 1); 

    // --- FINAL WIRING ---
    stackedWidget->addWidget(startPage);
    stackedWidget->addWidget(dashboardPage);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); 
    mainLayout->addWidget(stackedWidget);

    sensor = new MockSensor(this);
    connect(sensor, &MockSensor::bpmChanged, this, &FitnessTrackerWindow::onBpmChanged);
    connect(sensor, &MockSensor::spo2Changed, this, &FitnessTrackerWindow::onSpo2Changed);
    connect(sensor, &MockSensor::activityChanged, this, &FitnessTrackerWindow::onActivityChanged); 

    onBpmChanged(75);
    onSpo2Changed(98);
    onActivityChanged(0, 0.0, 0); 

    connect(startButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentIndex(1); 
        sensor->startWorkout();            
    });
}

void FitnessTrackerWindow::onBpmChanged(int newBpm) {
    bpmLabel->setText(QString::number(newBpm) + " BPM");
    heartWidget->updateBpm(newBpm);
}

void FitnessTrackerWindow::onSpo2Changed(int newSpo2) {
    spo2Label->setText(QString::number(newSpo2) + " %");
    oxygenWidget->updateSpo2(newSpo2);
}

void FitnessTrackerWindow::onActivityChanged(int newSteps, double newDistance, int newCalories) {
    stepLabel->setText(QLocale().toString(newSteps)); 
    distanceLabel->setText(QString::number(newDistance, 'f', 2) + " KM");
    activityWidget->updateValue(newSteps);

    calorieLabel->setText(QLocale().toString(newCalories) + " KCAL");
    calorieWidget->updateValue(newCalories);
}