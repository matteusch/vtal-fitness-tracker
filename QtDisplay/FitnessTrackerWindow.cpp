#include "FitnessTrackerWindow.h"
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>

FitnessTrackerWindow::FitnessTrackerWindow(QWidget *parent) : QWidget(parent), isDarkTheme(true) {
    setFixedSize(400, 650);
    setWindowTitle("Fitness Tracker");
    setObjectName("MainWindow");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    //TOP BAR
    QWidget *topBar = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(15, 15, 15, 0);

    topLayout->addStretch();

    btnThemeToggle = new QPushButton(this);
    btnThemeToggle->setFixedSize(40, 40);
    btnThemeToggle->setStyleSheet("background-color: #3a3a3a; border-radius: 20px; font-size: 16px;");

    cbLanguage = new QComboBox(this);
    cbLanguage->addItems({"EN", "PL", "DE"});
    cbLanguage->setFixedSize(65, 35);

    QString comboStyle =
        "QComboBox {"
        "  background-color: #3a3a3a;"
        "  color: white;"
        "  border-radius: 17px;"
        "  padding-left: 15px;"
        "  font-weight: bold;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "}"
        "QComboBox::down-arrow {"
        "  image: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #3a3a3a;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  selection-background-color: #505050;"
        "  outline: none;"
        "}";

    cbLanguage->setStyleSheet(comboStyle);

    topLayout->addWidget(btnThemeToggle);
    topLayout->addWidget(cbLanguage);

    //BOTTOM BAR
    QWidget *bottomBar = new QWidget(this);
    bottomBar->setFixedHeight(80);
    bottomBar->setStyleSheet("background-color: #1e1e1e;");

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(20, 10, 20, 10);

    btnLeft = new QPushButton("Home", this);
    btnMiddle = new QPushButton("Begin\nTraining", this);
    btnRight = new QPushButton("Settings", this);

    QString buttonStyle =
        "QPushButton {"
        "  background-color: #3a3a3a;"
        "  color: white;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #505050;"
        "}";

    btnLeft->setFixedSize(60, 60);
    btnLeft->setStyleSheet(buttonStyle);

    btnMiddle->setFixedSize(60, 60);
    btnMiddle->setStyleSheet(buttonStyle);

    btnRight->setFixedSize(60, 60);
    btnRight->setStyleSheet(buttonStyle);

    bottomLayout->addWidget(btnLeft);
    bottomLayout->addStretch();
    bottomLayout->addWidget(btnMiddle);
    bottomLayout->addStretch();
    bottomLayout->addWidget(btnRight);

    //MAIN LAYOUT
    stackedWidget = new QStackedWidget(this);

    QWidget *pageHome = new QWidget();
    QVBoxLayout *homeLayout = new QVBoxLayout(pageHome);
    QLabel *lblHome = new QLabel("Home Screen Content", pageHome);
    lblHome->setStyleSheet("color: white; font-size: 24px;");
    lblHome->setAlignment(Qt::AlignCenter);
    homeLayout->addWidget(lblHome);

    QWidget *pageTraining = new QWidget();
    QVBoxLayout *trainLayout = new QVBoxLayout(pageTraining);
    QLabel *lblTrain = new QLabel("Begin Training Content", pageTraining);
    lblTrain->setStyleSheet("color: white; font-size: 24px;");
    lblTrain->setAlignment(Qt::AlignCenter);
    trainLayout->addWidget(lblTrain);

    QWidget *pageSettings = new QWidget();
    QVBoxLayout *settingsLayout = new QVBoxLayout(pageSettings);
    QLabel *lblSettings = new QLabel("Settings Content", pageSettings);
    lblSettings->setStyleSheet("color: white; font-size: 24px;");
    lblSettings->setAlignment(Qt::AlignCenter);
    settingsLayout->addWidget(lblSettings);

    stackedWidget->addWidget(pageHome);
    stackedWidget->addWidget(pageTraining);
    stackedWidget->addWidget(pageSettings);

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(stackedWidget);
    mainLayout->addWidget(bottomBar);

    connect(btnThemeToggle, &QPushButton::clicked, this, [this]() {
        isDarkTheme = !isDarkTheme;
        applyTheme();
    });

    connect(btnLeft, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentIndex(0);
    });

    connect(btnMiddle, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentIndex(1);
    });

    connect(btnRight, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentIndex(2);
    });

    applyTheme();
}

void FitnessTrackerWindow::applyTheme() {
    if (isDarkTheme) {
        setStyleSheet("#MainWindow { background-color: #2b2b2b; }");
        btnThemeToggle->setText("☀️");
    } else {
        setStyleSheet("#MainWindow { background-color: #f5f5f5; }");
        btnThemeToggle->setText("🌙");
    }
}