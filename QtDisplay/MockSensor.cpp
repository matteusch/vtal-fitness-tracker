#include "MockSensor.h"
#include <QRandomGenerator>

MockSensor::MockSensor(QObject *parent) : QObject(parent),
    m_currentWorkout(None), m_isPaused(false), m_currentBpm(75), m_dailySteps(8432),
    m_workoutSeconds(0), m_currentSpO2(98), m_workoutDistance(0.0), m_workoutCalories(0.0) {

    // Daily tracking
    dailyTimer = new QTimer(this);
    connect(dailyTimer, &QTimer::timeout, this, &MockSensor::generateDailyReading);
    dailyTimer->start(2000);

    workoutTimer = new QTimer(this);
    connect(workoutTimer, &QTimer::timeout, this, &MockSensor::generateWorkoutReading);
}

void MockSensor::startWorkout(WorkoutType type) {
    m_currentWorkout = type;
    m_isPaused = false;
    m_workoutSeconds = 0;
    m_workoutDistance = 0.0;
    m_workoutCalories = 0.0;
    workoutTimer->start(1000);
}

void MockSensor::pauseWorkout() { m_isPaused = true; }
void MockSensor::resumeWorkout() { m_isPaused = false; }
void MockSensor::stopWorkout() {
    workoutTimer->stop();
    m_currentWorkout = None;
}

void MockSensor::generateDailyReading() {
    m_currentBpm += QRandomGenerator::global()->bounded(-5, 6);
    if (m_currentBpm < 50) m_currentBpm = 50;
    if (m_currentBpm > 180) m_currentBpm = 180;

    m_dailySteps += QRandomGenerator::global()->bounded(2, 7);

    emit dailyDataUpdated(m_currentBpm, m_dailySteps);
}

void MockSensor::generateWorkoutReading() {
    if (m_isPaused) return;

    m_workoutSeconds++;

    // SpO2 logic (only relevant during workouts for this app)
    m_currentSpO2 += QRandomGenerator::global()->bounded(-1, 2);
    if (m_currentSpO2 < 90) m_currentSpO2 = 90;
    if (m_currentSpO2 > 100) m_currentSpO2 = 100;

    // Adjust metrics based on the sport
    double speedMult = 1.0;
    double calMult = 1.0;
    if (m_currentWorkout == Running) { speedMult = 0.003; calMult = 0.15; }
    else if (m_currentWorkout == Cycling) { speedMult = 0.008; calMult = 0.10; }
    else if (m_currentWorkout == Swimming) { speedMult = 0.001; calMult = 0.20; }

    m_workoutDistance += speedMult;
    m_workoutCalories += calMult;

    emit workoutDataUpdated(m_workoutSeconds, m_currentBpm, m_currentSpO2, m_workoutDistance, static_cast<int>(m_workoutCalories));
}