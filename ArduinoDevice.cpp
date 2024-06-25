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
    connect(this, &ArduinoDevice::portOpened, fus_mainwindow, &FUSMainWindow::handlePortOpened);
}

ArduinoDevice::~ArduinoDevice() {
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    delete m_serialPort;
}

bool ArduinoDevice::open() {
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        //qDebug() << "Opened port" << m_serialPort->portName();
        fus_mainwindow->emitPrintSignal("Arduino port opened!");
        emit portOpened(true); // Emit signal indicating success
        fus_mainwindow->ui.Gantry_DIR_comboBox->setEnabled(true);
        fus_mainwindow->ui.Gantry_distance_spinBox->setEnabled(true);
        fus_mainwindow->ui.Gantry_speed_spinBox->setEnabled(true);
        fus_mainwindow->ui.Gantry_move_Button->setEnabled(true);
        fus_mainwindow->ui.Gantry_stop_Button->setEnabled(true);
        fus_mainwindow->ui.Gantry_set_Button->setEnabled(true);
        fus_mainwindow->ui.Gantry_right_Button->setEnabled(true);
        fus_mainwindow->ui.Gantry_left_Button->setEnabled(true);
        fus_mainwindow->ui.Gantry_up_Button->setEnabled(true);
        fus_mainwindow->ui.Gantry_down_Button->setEnabled(true);
        fus_mainwindow->ui.Gantry_forward_Button->setEnabled(true);
        fus_mainwindow->ui.Gantry_backward_Button->setEnabled(true);
        return true;
    }
    else {
        //qDebug() << "Failed to open port" << m_serialPort->portName();
        fus_mainwindow->emitPrintSignal("Failed to open Arduino port!");
        emit portOpened(false); // Emit signal indicating failure
        return false;
    }
}

void ArduinoDevice::close() {
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        //qDebug() << "Closed port" << m_serialPort->portName();
        fus_mainwindow->emitPrintSignal("Arduino port closed!");
        fus_mainwindow->ui.Gantry_onoff_Button->setStyleSheet("background-color: red");
        fus_mainwindow->ui.Gantry_DIR_comboBox->setEnabled(false);
        fus_mainwindow->ui.Gantry_distance_spinBox->setEnabled(false);
        fus_mainwindow->ui.Gantry_speed_spinBox->setEnabled(false);
        fus_mainwindow->ui.Gantry_move_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_stop_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_set_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_return_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_movetoposition_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_right_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_left_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_up_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_down_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_forward_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_backward_Button->setEnabled(false);
        fus_mainwindow->ui.Gantry_x_spinBox->setEnabled(false);
        fus_mainwindow->ui.Gantry_y_spinBox->setEnabled(false);
        fus_mainwindow->ui.Gantry_z_spinBox->setEnabled(false);
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