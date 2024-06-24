// Author: Soroosh Sanatkhani
// Columbia University
// Created: 22 March, 2024
// Last Modified : 22 March, 2024

#ifndef GANTRY_H  // Include guard to prevent multiple inclusions
#define GANTRY_H

#pragma once

#include <QObject>  // Includes the QObject class for creating custom Qt objects
#include <QTimer>  // Includes the QTimer class for creating timers
#include "ArduinoDevice.h"

class FUSMainWindow;

struct Position3D
{
	float x;
	float y;
	float z;
};

class Gantry : public QObject
{
	Q_OBJECT  // Macro to enable the use of signals and slots

public:
	explicit Gantry(FUSMainWindow* parent = nullptr);  // Constructor
	~Gantry();  // Destructor

	Position3D gantryPosition, gantriGoToPosition;

	void Move(char,float,float);

	void open();
	void close();

	void move_Click(char,float,float);
	void stop_Click();
	void setOrigin();
	void returnToOrigin();
	void MoveTo();

private:
	FUSMainWindow* fus_mainwindow;  // Pointer to the main window
	ArduinoDevice* arduino;
};
#endif // GANTRY_H