// Author: Soroosh Sanatkhani
// Columbia University
// Created: 22 March, 2024
// Last Modified : 22 March, 2024

#include "stdafx.h"
#include "Gantry.h"
#include "FUSMainWindow.h"

Gantry::Gantry(FUSMainWindow* parent) :
    QObject(parent), fus_mainwindow(parent), arduino(new ArduinoDevice("COM3", fus_mainwindow))
{
    // Connect the ArduinoDevice's serialDataReceived signal to the FUSMainWindow's emitPrintSignal slot
    connect(arduino, &ArduinoDevice::serialDataReceived, fus_mainwindow, &FUSMainWindow::emitPrintSignal);
}

Gantry::~Gantry()
{
    delete arduino;
}

void Gantry::open_Click()
{
    arduino->open();
    arduino->readSerialData();
}

void Gantry::move_Click(char Direction, float Distance, float Speed)
{
    arduino->readSerialData();
    Move(Direction, Distance, Speed); // Assuming Move(Direction) is implemented elsewhere in Gantry.cpp
}
void Gantry::stop_Click()
{
	arduino->readSerialData();
    Move('S', '0', '0'); // Assuming Move(Direction) is implemented elsewhere in Gantry.cpp
}