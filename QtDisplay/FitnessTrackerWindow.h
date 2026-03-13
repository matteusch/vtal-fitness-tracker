#pragma once
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include "DashboardWidgets.h"
#include "MockSensor.h"

class FitnessTrackerWindow : public QWidget {
    Q_OBJECT
public:
    explicit FitnessTrackerWindow(QWidget *parent = nullptr);

private slots:
    void onBpmChanged(int newBpm);
    void onSpo2Changed(int newSpo2);
    void onActivityChanged(int newSteps, double newDistance, int newCalories);
    void toggleTheme(); // NEW: The theme switch logic

private:
    QStackedWidget *stackedWidget;
    QPushButton *themeButton; // NEW: The floating button
    bool m_isDarkMode;        // NEW: Theme tracker

    PulsingHeartWidget *heartWidget;
    OxygenCircleWidget *oxygenWidget;
    ActivityRingWidget *activityWidget;
    CalorieRingWidget  *calorieWidget;

    QLabel *startTitleLabel; // NEW: Tracked to change color
    QLabel *bpmLabel;
    QLabel *spo2Label;
    QLabel *stepLabel;
    QLabel *distanceLabel;
    QLabel *calorieLabel;

    MockSensor *sensor;
};