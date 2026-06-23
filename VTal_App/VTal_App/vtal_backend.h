#ifndef VTAL_BACKEND_H
#define VTAL_BACKEND_H

#include <QObject>
#include <QBluetoothSocket>
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QByteArray>
#include <QList>

class VTalReceiver : public QObject {
    Q_OBJECT
    Q_PROPERTY(int bpm READ bpm NOTIFY metricsUpdated)
    Q_PROPERTY(float temperature READ temperature NOTIFY metricsUpdated)
    Q_PROPERTY(int steps READ steps NOTIFY metricsUpdated)
    Q_PROPERTY(float distance READ distance NOTIFY metricsUpdated)

public:
    explicit VTalReceiver(QObject *parent = nullptr) : QObject(parent) {
        socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

        connect(socket, &QBluetoothSocket::readyRead, this, [this](){
            m_buffer.append(socket->readAll());

            while (m_buffer.contains('\n')) {
                int newlineIndex = m_buffer.indexOf('\n');
                QByteArray packet = m_buffer.left(newlineIndex).trimmed();
                m_buffer.remove(0, newlineIndex + 1);

                QList<QByteArray> parts = packet.split(',');

                // BPM, Temp, Altitude, Steps, Distance
                if (parts.size() >= 5) {
                    m_bpm = parts[0].toInt();
                    m_temperature = parts[1].toFloat();
                    m_steps = parts[3].toInt();
                    m_distance = parts[4].toFloat();

                    emit metricsUpdated();
                }
            }
        });

        connect(socket, &QBluetoothSocket::connected, this, [this](){
            emit deviceConnected();
        });
    }

    Q_INVOKABLE void connectToHC05(const QString &macAddress) {
        QBluetoothUuid sppUuid(QBluetoothUuid::ServiceClassUuid::SerialPort);
        socket->connectToService(QBluetoothAddress(macAddress), sppUuid);
    }

    Q_INVOKABLE void disconnectDevice() {
        if (socket->isOpen()) {
            socket->close();
        }
    }

    int bpm() const { return m_bpm; }
    float temperature() const { return m_temperature; }
    int steps() const { return m_steps; }
    float distance() const { return m_distance; }

signals:
    void metricsUpdated();
    void deviceConnected();

private:
    QBluetoothSocket *socket;
    QByteArray m_buffer;

    int m_bpm = 0;
    float m_temperature = 0.0f;
    int m_steps = 0;
    float m_distance = 0.0f;
};

#endif // VTAL_BACKEND_H