#pragma once
#include <QObject>
#include <QTimer>

class MockSensor : public QObject {
    Q_OBJECT
public:
    explicit MockSensor(QObject *parent = nullptr);

public slots:
    void startWorkout();

    signals:
        void bpmChanged(int newBpm);
    void spo2Changed(int newSpo2);
    void activityChanged(int newSteps, double newDistance, int newCalories);

private slots:
    void generateNewReading();

private:
    QTimer *timer;
    int m_currentBpm;
    int m_currentSpO2;
    int m_currentSteps;
    double m_currentDistance;
    double m_currentCalories;
};