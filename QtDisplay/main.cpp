/**
* @file main.cpp
 * @brief Punkt wejścia dla aplikacji desktopowej.
 * Inicjalizuje środowisko Qt, tworzy główną pętlę zdarzeń i uruchamia okno Fitness Tracker.
 */


#include <QApplication>
#include "FitnessTrackerWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    FitnessTrackerWindow window;
    window.show();

    return app.exec();
}