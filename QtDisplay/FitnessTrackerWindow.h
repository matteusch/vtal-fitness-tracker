#pragma once
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QComboBox>
#include <QGraphicsOpacityEffect>
#include "DashboardWidgets.h"
#include "MockSensor.h"

class FitnessTrackerWindow : public QWidget {
    Q_OBJECT
public:
    explicit FitnessTrackerWindow(QWidget *parent = nullptr);

private slots:
    void onDailyUpdated(int bpm, int steps);
    void onWorkoutUpdated(int seconds, int bpm, int spo2, double distance, int calories);
    void toggleTheme();
    void changeLanguage(int index);
    void updateStaticTexts();
    void fadeToPage(int pageIndex);
    void formatTimeLabel(int seconds);

private:
    enum Language { EN, PL, DE };
    Language m_currentLang;

    QStackedWidget *stackedWidget;
    QGraphicsOpacityEffect *opacityEffect;

    QPushButton *themeButton;
    QComboBox *langComboBox;
    bool m_isDarkMode;
    bool m_isWorkoutPaused;

    // --- UI Elements ---
    PulsingHeartWidget *homeHeartWidget;
    ActivityRingWidget *homeStepsWidget;
    QLabel *homeBpmLabel;
    QLabel *homeStepsLabel;
    QPushButton *startTrainingBtn;

    QPushButton *runBtn, *cycleBtn, *swimBtn, *cancelSelectionBtn;

    QLabel *workoutTimeLabel;
    PulsingHeartWidget *workoutHeartWidget;
    OxygenCircleWidget *workoutSpo2Widget;
    QLabel *workoutBpmLabel;
    QLabel *workoutSpo2Label;
    QLabel *workoutDistanceLabel;
    QLabel *workoutCalorieLabel;
    QPushButton *pauseBtn, *endBtn, *homeBtn;

    // Language references
    QLabel *homeTitle, *homeHeartSub, *homeStepsSub;
    QLabel *selectTitle;
    QLabel *workTimeSub, *workHeartSub, *workSpo2Sub, *workDistSub, *workCalSub;

    MockSensor *sensor;
};