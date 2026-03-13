#include "MockSensor.h"
#include <QRandomGenerator>

MockSensor::MockSensor(QObject *parent) : QObject(parent), 
    m_currentBpm(75), m_currentSpO2(98), m_currentSteps(0), m_currentCalories(0.0) { 
    
    m_currentDistance = 0.0; 

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MockSensor::generateNewReading);
}

void MockSensor::startWorkout() {
    timer->start(2000); 
}

void MockSensor::generateNewReading() {
    int bpmChange = QRandomGenerator::global()->bounded(-5, 6);
    m_currentBpm += bpmChange;
    if (m_currentBpm < 50) m_currentBpm = 50;
    if (m_currentBpm > 180) m_currentBpm = 180;
    emit bpmChanged(m_currentBpm);

    int spo2Change = QRandomGenerator::global()->bounded(-1, 2);
    m_currentSpO2 += spo2Change;
    if (m_currentSpO2 < 92) m_currentSpO2 = 92;
    if (m_currentSpO2 > 100) m_currentSpO2 = 100;
    emit spo2Changed(m_currentSpO2);

    int stepsTaken = QRandomGenerator::global()->bounded(2, 7);
    m_currentSteps += stepsTaken;
    m_currentDistance = m_currentSteps * 0.000762; 
    m_currentCalories += stepsTaken * 0.045;
    
    emit activityChanged(m_currentSteps, m_currentDistance, static_cast<int>(m_currentCalories));
}