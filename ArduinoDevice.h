// Author: Soroosh Sanatkhani
// Columbia University
// Created: 21 June, 2024
// Last Modified : 21 June, 2024

#ifndef ARDUINODEVICE_H
#define ARDUINODEVICE_H

#include <QObject>
#include <QSerialPort>

class FUSMainWindow;

class ArduinoDevice : public QObject {
    Q_OBJECT  // Enable signals and slots

public:
    ArduinoDevice(const QString& portName, FUSMainWindow* mainWindow);
    ~ArduinoDevice();

    bool open();
    qint64 write(char direction, float distance, float speed);

signals:
    void serialDataReceived(const QString& data);
    void acknowledgmentReceived();  // Signal for when an acknowledgment message is received from the Arduino
    void gantryReady();  // Signal for when the gantry is ready to receive new commands

public slots:
    void readSerialData();

private:
    QSerialPort* m_serialPort;
    FUSMainWindow* fus_mainwindow;  // Pointer to the main window
};

#endif // ARDUINODEVICE_H