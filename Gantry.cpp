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
    gantriGoToPosition({ 0, 0, 0 })
{
	// Connect the ArduinoDevice's acknowledgmentReceived signal to this Gantry's slot
	connect(arduino, &ArduinoDevice::acknowledgmentReceived, this, &Gantry::onAcknowledgmentReceived);

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
}

void Gantry::close()
{
	arduino->write('C', 0, 0);	// sending 'C' as close signal to turn off the motors
	arduino->close();
}

void Gantry::move_Click(char Direction, float Distance, float Speed)
{
    Move(Direction, Distance, Speed);
}

void Gantry::stop_Click()
{
	std::queue<std::tuple<char, float, float>> empty;
	std::swap(commandQueue, empty); // Clear the command queue
	arduino->write('S', 0, 0); // Send stop command immediately
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
	if (commandQueue.empty()) {
		//fus_mainwindow->emitPrintSignal("Command queue is empty.");
		return;
	}
	auto [Direction, Distance, Speed] = commandQueue.front();
	//fus_mainwindow->emitPrintSignal("Processing command queue...");

	commandQueue.pop();
	arduino->write(Direction, Distance, Speed);
	waitTimer->start(100); // Start the timer to wait for acknowledgment
}

void Gantry::onAcknowledgmentReceived()
{
	waitTimer->stop(); // Stop the timer as acknowledgment is received
	//fus_mainwindow->emitPrintSignal("Acknowledgment is received");
	processCommandQueue(); // Attempt to process the next command in the queue
}

void Gantry::onWaitTimerTimeout()
{
	//fus_mainwindow->emitPrintSignal("Timeout waiting for acknowledgment from Arduino.");
	processCommandQueue(); // Attempt to process the next command in the queue
}