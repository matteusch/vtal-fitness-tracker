#pragma once
#include <QObject>
#include <QTimer>

class MockSensor : public QObject {
    Q_OBJECT
public:
    explicit MockSensor(QObject *parent = nullptr);

    enum WorkoutType { None, Running, Cycling, Swimming };

    bool isWorkoutActive() const { return m_currentWorkout != None; }

public slots:
    void startWorkout(WorkoutType type);
    void pauseWorkout();
    void resumeWorkout();
    void stopWorkout();

    signals:
        void dailyDataUpdated(int bpm, int dailySteps);
    void workoutDataUpdated(int elapsedSeconds, int bpm, int spo2, double distance, int calories);

private slots:
    void generateDailyReading();
    void generateWorkoutReading();

private:
    QTimer *dailyTimer;
    QTimer *workoutTimer;

    WorkoutType m_currentWorkout;
    bool m_isPaused;

    int m_currentBpm;
    int m_dailySteps;

    int m_workoutSeconds;
    int m_currentSpO2;
    double m_workoutDistance;
    double m_workoutCalories;
};