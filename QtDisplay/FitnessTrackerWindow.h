#ifndef FITNESSTRACKERWINDOW_H
#define FITNESSTRACKERWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>

class FitnessTrackerWindow : public QWidget {
    Q_OBJECT

public:
    explicit FitnessTrackerWindow(QWidget *parent = nullptr);

private:
    QPushButton *btnLeft;
    QPushButton *btnMiddle;
    QPushButton *btnRight;

    QPushButton *btnThemeToggle;
    QComboBox *cbLanguage;

    QStackedWidget *stackedWidget;

    bool isDarkTheme;

    void applyTheme();
};

#endif