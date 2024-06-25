// Author: Soroosh Sanatkhani
// Columbia University
// Created: 21 June, 2024
// Last Modified : 21 June, 2024

#include "stdafx.h"
#include "ArduinoDevice.h"
#include <QDebug>
#include "FUSMainWindow.h"

ArduinoDevice::ArduinoDevice(const QString& portName, FUSMainWindow* mainWindow)
    : m_serialPort(new QSerialPort(portName)), fus_mainwindow(mainWindow)
{
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(m_serialPort, &QSerialPort::readyRead, this, &ArduinoDevice::readSerialData);
}

ArduinoDevice::~ArduinoDevice() {
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    delete m_serialPort;
}

bool ArduinoDevice::open() {
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Opened port" << m_serialPort->portName();
        fus_mainwindow->emitPrintSignal("Arduino port opened!");
        fus_mainwindow->ui.Gantry_onoff_Button->setEnabled(true);
        return true;
    }
    else {
        qDebug() << "Failed to open port" << m_serialPort->portName();
        fus_mainwindow->emitPrintSignal("Failed to open Arduino port!");
        return false;
    }
}

qint64 ArduinoDevice::write(char direction, float distance, float speed) {
    // Ensure distance and speed are within the specified range
    distance = qBound(0.0f, distance, 100.0f);
    speed = qBound(0.0f, speed, 5.0f);

    // Format the command as a string: "D,0.1,1.0\n"
    QLocale::setDefault(QLocale::C);
    QByteArray command = QByteArray(1, direction) + "," +
        QByteArray::number(distance, 'f', 1) + "," + // One decimal place for distance
        QByteArray::number(speed, 'f', 1) + "\n"; // One decimal place for speed

    // Send the command over the serial port
    return m_serialPort->write(command);
}

void ArduinoDevice::readSerialData() {
    if (m_serialPort->canReadLine()) {
        QByteArray line = m_serialPort->readLine();
        QString data = QString::fromUtf8(line.trimmed()); // Convert to QString and remove any trailing newline
        //qDebug() << "Received:" << data;
        if (data != "ACK") {
            fus_mainwindow->emitPrintSignal(data);
        }
        else {
            fus_mainwindow->emitPrintSignal("Arduino received command!");
            emit acknowledgmentReceived(); // Emit signal indicating an ACK was received
        }
    }
}