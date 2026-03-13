#include <QApplication>
#include "FitnessTrackerWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    FitnessTrackerWindow window;
    window.show();

    return app.exec();
}