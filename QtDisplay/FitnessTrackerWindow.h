#pragma once
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QComboBox>
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
    void toggleTheme();
    void changeLanguage(int index); // NEW: Handles the dropdown change
    void updateStaticTexts();       // NEW: Refreshes all the text on the screen

private:
    // Language tracking
    enum Language { EN, PL, DE };
    Language m_currentLang;

    // Caching the latest sensor values so we can redraw them on language change
    int m_lastBpm;
    int m_lastSpo2;
    int m_lastSteps;
    double m_lastDistance;
    int m_lastCalories;

    QStackedWidget *stackedWidget;
    QPushButton *themeButton;
    QComboBox *langComboBox;  // NEW: The floating dropdown menu
    bool m_isDarkMode;

    PulsingHeartWidget *heartWidget;
    OxygenCircleWidget *oxygenWidget;
    ActivityRingWidget *activityWidget;
    CalorieRingWidget  *calorieWidget;

    // Promoted labels so our update function can change their text
    QLabel *startTitleLabel;
    QPushButton *startButton;

    QLabel *heartSubtitle;
    QLabel *oxygenSubtitle;
    QLabel *activitySubtitle;
    QLabel *calorieSubtitle;

    QLabel *bpmLabel;
    QLabel *spo2Label;
    QLabel *stepLabel;
    QLabel *distanceLabel;
    QLabel *calorieLabel;

    MockSensor *sensor;
};