#pragma once
#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>

class PulsingHeartWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal scaleFactor READ scaleFactor WRITE setScaleFactor)
    Q_PROPERTY(qreal glowOpacity READ glowOpacity WRITE setGlowOpacity)
public:
    explicit PulsingHeartWidget(QWidget *parent = nullptr);
    qreal scaleFactor() const { return m_scaleFactor; }
    void setScaleFactor(qreal scale);
    qreal glowOpacity() const { return m_glowOpacity; }
    void setGlowOpacity(qreal opacity);
    void setDarkMode(bool dark);
public slots:
    void updateBpm(int bpm);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    qreal m_scaleFactor;
    qreal m_glowOpacity;
    bool m_isDarkMode;
    QPropertyAnimation *singleBeatAnimation;
    QPropertyAnimation *glowFadeAnimation;
    QTimer *beatTimer;
};

class OxygenCircleWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal animatedValue READ animatedValue WRITE setAnimatedValue)
public:
    explicit OxygenCircleWidget(QWidget *parent = nullptr);
    qreal animatedValue() const { return m_animatedValue; }
    void setAnimatedValue(qreal val);
    void setDarkMode(bool dark);
public slots:
    void updateSpo2(int spo2);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    qreal m_animatedValue;
    bool m_isDarkMode;
    QPropertyAnimation *valueAnimation;
};

class ActivityRingWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal animatedValue READ animatedValue WRITE setAnimatedValue)
public:
    explicit ActivityRingWidget(QWidget *parent = nullptr);
    qreal animatedValue() const { return m_animatedValue; }
    void setAnimatedValue(qreal val);
    void setDarkMode(bool dark);
public slots:
    void updateValue(int val);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    qreal m_animatedValue;
    bool m_isDarkMode;
    QPropertyAnimation *valueAnimation;
};

class CalorieRingWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal animatedValue READ animatedValue WRITE setAnimatedValue)
public:
    explicit CalorieRingWidget(QWidget *parent = nullptr);
    qreal animatedValue() const { return m_animatedValue; }
    void setAnimatedValue(qreal val);
    void setDarkMode(bool dark);
public slots:
    void updateValue(int val);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    qreal m_animatedValue;
    bool m_isDarkMode;
    QPropertyAnimation *valueAnimation;
};