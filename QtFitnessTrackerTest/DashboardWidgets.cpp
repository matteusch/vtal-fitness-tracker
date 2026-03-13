#include "DashboardWidgets.h"
#include <QPainter>

// --- Pulsing Heart ---
PulsingHeartWidget::PulsingHeartWidget(QWidget *parent) : QWidget(parent), m_scaleFactor(1.0), m_glowOpacity(0.0), m_isDarkMode(true) {
    setFixedSize(140, 140);
    singleBeatAnimation = new QPropertyAnimation(this, "scaleFactor", this);
    // ... (keep the rest of the animation setup exactly the same)
    singleBeatAnimation->setDuration(300);
    singleBeatAnimation->setStartValue(1.0);
    singleBeatAnimation->setKeyValueAt(0.15, 1.15);
    singleBeatAnimation->setKeyValueAt(0.30, 1.0);
    singleBeatAnimation->setKeyValueAt(0.50, 1.3);
    singleBeatAnimation->setKeyValueAt(0.80, 1.0);
    singleBeatAnimation->setEndValue(1.0);

    beatTimer = new QTimer(this);
    connect(beatTimer, &QTimer::timeout, this, [this]() {
        if (singleBeatAnimation->state() != QAbstractAnimation::Running) singleBeatAnimation->start();
    });

    glowFadeAnimation = new QPropertyAnimation(this, "glowOpacity", this);
    glowFadeAnimation->setDuration(600);
    glowFadeAnimation->setEndValue(0.0);
}
void PulsingHeartWidget::setScaleFactor(qreal scale) { m_scaleFactor = scale; update(); }
void PulsingHeartWidget::setGlowOpacity(qreal opacity) { m_glowOpacity = opacity; update(); }
void PulsingHeartWidget::setDarkMode(bool dark) { m_isDarkMode = dark; update(); }

void PulsingHeartWidget::updateBpm(int bpm) {
    if (bpm <= 0) return;
    int msPerBeat = 60000 / bpm;
    beatTimer->setInterval(msPerBeat);
    if (!beatTimer->isActive()) beatTimer->start();

    qreal targetGlow = 0.0;
    if (bpm > 120) {
        targetGlow = (bpm - 120.0) / 60.0;
        if (targetGlow > 1.0) targetGlow = 1.0;
    }
    if (glowFadeAnimation->endValue().toReal() != targetGlow) {
        glowFadeAnimation->stop();
        glowFadeAnimation->setStartValue(m_glowOpacity);
        glowFadeAnimation->setEndValue(targetGlow);
        glowFadeAnimation->start();
    }
}

void PulsingHeartWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2.0, height() / 2.0);

    if (m_glowOpacity > 0.0) {
        painter.save();
        int baseAlpha = static_cast<int>(m_glowOpacity * 255);
        int layers = 2 + static_cast<int>(m_glowOpacity * 4);
        qreal baseThickness = 2.0 + (m_glowOpacity * 3.0);
        for (int i = layers; i >= 1; --i) {
            QColor glowColor(255, 45, 85, baseAlpha / (i * 2));
            painter.setPen(QPen(glowColor, i * baseThickness));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(QRectF(-55, -55, 110, 110), 20, 20);
        }
        painter.setPen(QPen(QColor(255, 45, 85, baseAlpha), 2));
        painter.drawRoundedRect(QRectF(-55, -55, 110, 110), 20, 20);
        painter.restore();
    }
    painter.save();
    painter.scale(m_scaleFactor, m_scaleFactor);
    QFont font = painter.font();
    font.setPixelSize(60);
    painter.setFont(font);
    painter.drawText(QRect(-60, -60, 120, 120), Qt::AlignCenter, "❤️");
    painter.restore();
}

// --- Oxygen Circle ---
OxygenCircleWidget::OxygenCircleWidget(QWidget *parent) : QWidget(parent), m_animatedValue(98.0), m_isDarkMode(true) {
    setFixedSize(140, 140);
    valueAnimation = new QPropertyAnimation(this, "animatedValue", this);
    valueAnimation->setDuration(600);
    valueAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}
void OxygenCircleWidget::setAnimatedValue(qreal val) { m_animatedValue = val; update(); }
void OxygenCircleWidget::setDarkMode(bool dark) { m_isDarkMode = dark; update(); }

void OxygenCircleWidget::updateSpo2(int spo2) {
    valueAnimation->stop();
    valueAnimation->setStartValue(m_animatedValue);
    valueAnimation->setEndValue(static_cast<qreal>(spo2));
    valueAnimation->start();
}

void OxygenCircleWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2.0, height() / 2.0);

    int radius = 55;
    QRectF rect(-radius, -radius, radius * 2, radius * 2);

    // Dynamic Track Color
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_isDarkMode ? QColor("#2c2c2e") : QColor("#d1d1d6"));
    painter.drawEllipse(rect);

    qreal minSpO2 = 85.0;
    qreal maxSpO2 = 100.0;
    qreal fillRatio = (m_animatedValue - minSpO2) / (maxSpO2 - minSpO2);
    if (fillRatio < 0.0) fillRatio = 0.0;
    if (fillRatio > 1.0) fillRatio = 1.0;
    qreal fillHeight = (radius * 2) * fillRatio;

    painter.save();
    QRectF clipRect(-radius, radius - fillHeight, radius * 2, fillHeight);
    painter.setClipRect(clipRect);

    painter.setBrush(QColor("#0a84ff"));
    painter.drawEllipse(rect);

    painter.setBrush(QColor("#5ac8fa"));
    painter.drawEllipse(QPointF(-20, 15), 4, 4);
    painter.drawEllipse(QPointF(15, 30), 6, 6);
    painter.drawEllipse(QPointF(8, -10), 3, 3);
    painter.drawEllipse(QPointF(-10, 35), 5, 5);
    painter.drawEllipse(QPointF(-25, -5), 4, 4);
    painter.drawEllipse(QPointF(25, 5), 3, 3);
    painter.restore();
}

// --- Activity Ring ---
ActivityRingWidget::ActivityRingWidget(QWidget *parent) : QWidget(parent), m_animatedValue(0.0), m_isDarkMode(true) {
    setFixedSize(140, 140);
    valueAnimation = new QPropertyAnimation(this, "animatedValue", this);
    valueAnimation->setDuration(600);
    valueAnimation->setEasingCurve(QEasingCurve::OutBack);
}
void ActivityRingWidget::setAnimatedValue(qreal val) { m_animatedValue = val; update(); }
void ActivityRingWidget::setDarkMode(bool dark) { m_isDarkMode = dark; update(); }

void ActivityRingWidget::updateValue(int val) {
    valueAnimation->stop();
    valueAnimation->setStartValue(m_animatedValue);
    valueAnimation->setEndValue(static_cast<qreal>(val));
    valueAnimation->start();
}

void ActivityRingWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2.0, height() / 2.0);

    int radius = 55;
    QRectF rect(-radius, -radius, radius * 2, radius * 2);

    // Dynamic Track Color
    QPen bgPen(m_isDarkMode ? QColor("#2c2c2e") : QColor("#d1d1d6"), 12, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(bgPen);
    painter.drawArc(rect, 0, 360 * 16);

    QPen fgPen(QColor("#ff9500"), 12, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(fgPen);

    qreal goal = 10000.0;
    qreal progress = m_animatedValue / goal;
    if (progress > 1.0) progress = 1.0;

    int startAngle = 90 * 16;
    int spanAngle = -static_cast<int>(progress * 360 * 16);
    painter.drawArc(rect, startAngle, spanAngle);

    QFont font = painter.font();
    font.setPixelSize(40);
    painter.setFont(font);
    painter.drawText(rect, Qt::AlignCenter, "👣");
}

// --- Calorie Ring ---
CalorieRingWidget::CalorieRingWidget(QWidget *parent) : QWidget(parent), m_animatedValue(0.0), m_isDarkMode(true) {
    setFixedSize(140, 140);
    valueAnimation = new QPropertyAnimation(this, "animatedValue", this);
    valueAnimation->setDuration(600);
    valueAnimation->setEasingCurve(QEasingCurve::OutBack);
}
void CalorieRingWidget::setAnimatedValue(qreal val) { m_animatedValue = val; update(); }
void CalorieRingWidget::setDarkMode(bool dark) { m_isDarkMode = dark; update(); }

void CalorieRingWidget::updateValue(int val) {
    valueAnimation->stop();
    valueAnimation->setStartValue(m_animatedValue);
    valueAnimation->setEndValue(static_cast<qreal>(val));
    valueAnimation->start();
}

void CalorieRingWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2.0, height() / 2.0);

    int radius = 55;
    QRectF rect(-radius, -radius, radius * 2, radius * 2);

    // Dynamic Track Color
    QPen bgPen(m_isDarkMode ? QColor("#2c2c2e") : QColor("#d1d1d6"), 12, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(bgPen);
    painter.drawArc(rect, 0, 360 * 16);

    QPen fgPen(QColor("#34c759"), 12, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(fgPen);

    qreal goal = 2500.0;
    qreal progress = m_animatedValue / goal;
    if (progress > 1.0) progress = 1.0;

    int startAngle = 90 * 16;
    int spanAngle = -static_cast<int>(progress * 360 * 16);
    painter.drawArc(rect, startAngle, spanAngle);

    QFont font = painter.font();
    font.setPixelSize(40);
    painter.setFont(font);
    painter.drawText(rect, Qt::AlignCenter, "🔥");
}