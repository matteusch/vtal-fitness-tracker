#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QPermissions>
#include "vtal_backend.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("VTal");
    QCoreApplication::setOrganizationDomain("vtal.com");
    QCoreApplication::setApplicationName("VTal_Telemetry");

    //Android Bluetooth
    QBluetoothPermission btPerm;
    qApp->requestPermission(btPerm, [](const QPermission &){});

    VTalReceiver receiver;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("vtal", &receiver);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("VTal_App", "Main");

    return app.exec();
}