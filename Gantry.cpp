// Author: Soroosh Sanatkhani
// Columbia University
// Created: 22 March, 2024
// Last Modified : 22 March, 2024

#include "stdafx.h"
#include "Gantry.h"
#include "FUSMainWindow.h"

Gantry::Gantry(FUSMainWindow* parent) :
    QObject(parent),
    fus_mainwindow(parent),
    arduino(new ArduinoDevice("COM3", fus_mainwindow)),
    gantryPosition({ 0, 0, 0 }),
    gantriGoToPosition({ 0, 0, 0 }),
	ackReceived(true)
{
    // Connect the ArduinoDevice's serialDataReceived signal to the FUSMainWindow's emitPrintSignal slot
    connect(arduino, &ArduinoDevice::serialDataReceived, fus_mainwindow, &FUSMainWindow::emitPrintSignal);

	waitTimer = new QTimer(this);
	connect(waitTimer, &QTimer::timeout, this, &Gantry::onWaitTimerTimeout);
}

Gantry::~Gantry()
{
    delete arduino;
}

void Gantry::open()
{
    arduino->open();
	arduino->write('O', 0, 0);	// sending 'O' as open signal to turn on the motors
    arduino->readSerialData();
}

void Gantry::close()
{
	arduino->write('C', 0, 0);	// sending 'C' as close signal to turn off the motors
	arduino->close();
}

void Gantry::move_Click(char Direction, float Distance, float Speed)
{
    arduino->readSerialData();
    Move(Direction, Distance, Speed);
}

void Gantry::stop_Click()
{
	std::queue<std::tuple<char, float, float>> empty;
	std::swap(commandQueue, empty); // Clear the command queue
	arduino->write('S', 0, 0); // Send stop command immediately
	ackReceived = true; // Allow new commands
	arduino->readSerialData();
}

void Gantry::setOrigin()
{
	gantryPosition = { 0, 0, 0 };
	fus_mainwindow->ui.Gantry_x_spinBox->setValue(0.0);
	fus_mainwindow->ui.Gantry_y_spinBox->setValue(0.0);
	fus_mainwindow->ui.Gantry_z_spinBox->setValue(0.0);
}

void Gantry::returnToOrigin()
{
    if (gantryPosition.x >= 0)
	{
        Move('L', gantryPosition.x, 5);
	}
    else
    {
        Move('R', -gantryPosition.x, 5);
    }
    
    if (gantryPosition.z >= 0)
    {
        Move('D', gantryPosition.z, 5);
    }
    else
	{
		Move('U', -gantryPosition.z, 5);
	}
	
    if (gantryPosition.y >= 0)
	{
		Move('B', gantryPosition.y, 5);
	}
	else
	{
		Move('F', -gantryPosition.y, 5);
	}
}

void Gantry::MoveTo()
{
	if (gantriGoToPosition.x >= gantryPosition.x)
	{
		Move('R', gantriGoToPosition.x - gantryPosition.x, 5);
	}
	else
	{
		Move('L', gantryPosition.x - gantriGoToPosition.x, 5);
	}

	if (gantriGoToPosition.z >= gantryPosition.z)
	{
		Move('U', gantriGoToPosition.z - gantryPosition.z, 5);
	}
	else
	{
		Move('D', gantryPosition.z - gantriGoToPosition.z, 5);
	}

	if (gantriGoToPosition.y >= gantryPosition.y)
	{
		Move('F', gantriGoToPosition.y - gantryPosition.y, 5);
	}
	else
	{
		Move('B', gantryPosition.y - gantriGoToPosition.y, 5);
	}
}

void Gantry::processCommandQueue() {
	if (commandQueue.empty() || !ackReceived) {
		return;
	}
	auto [Direction, Distance, Speed] = commandQueue.front();
	commandQueue.pop();
	arduino->write(Direction, Distance, Speed);
	ackReceived = false;
}

void Gantry::onWaitTimerTimeout()
{
	qDebug() << "Timeout waiting for acknowledgment from Arduino.";
	ackReceived = true; // Assume the command was lost and allow the next command to proceed
	processCommandQueue(); // Attempt to process the next command in the queue
}