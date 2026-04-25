#include "FitnessTrackerWindow.h"

FitnessTrackerWindow::FitnessTrackerWindow(QWidget *parent) : QWidget(parent) {
    // Typical mobile screen size (9:16 aspect ratio)
    setFixedSize(400, 800);

    // Set the dark grey background using Qt Style Sheets
    setStyleSheet("background-color: #2b2b2b;");

    setWindowTitle("Fitness Tracker");
}