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
    void close();
    qint64 write(char direction, float distance, float speed);

signals:
    void serialDataReceived(const QString& data);

public slots:
    void readSerialData();

private:
    QSerialPort* m_serialPort;
    FUSMainWindow* fus_mainwindow;  // Pointer to the main window
};

#endif // ARDUINODEVICE_H