// Author: Soroosh Sanatkhani
// Columbia University
// Created: 22 March, 2024
// Last Modified : 22 March, 2024

#include "stdafx.h"
#include "Gantry.h"
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "FUSMainWindow.h"

void Gantry::Move(char Direction, float Distance, float Speed)
{
	switch (Direction)
	{
		case 'R':
			gantryPosition.x += Distance;
			//fus_mainwindow->emitPrintSignal("Going right");
			break;
		case 'L':
			gantryPosition.x -= Distance;
			//fus_mainwindow->emitPrintSignal("Going left");
			break;
		case 'U':
			gantryPosition.z += Distance;
			//fus_mainwindow->emitPrintSignal("Going up");
			break;
		case 'D':
			gantryPosition.z -= Distance;
			//fus_mainwindow->emitPrintSignal("Going down");
			break;
		case 'F':
			gantryPosition.y += Distance;
			//fus_mainwindow->emitPrintSignal("Going forward");
			break;
		case 'B':
			gantryPosition.y -= Distance;
			//fus_mainwindow->emitPrintSignal("Going backward");
			break;
	}
	// Add the command to the queue instead of sending it directly
	commandQueue.push(std::make_tuple(Direction, Distance, Speed));
	// Attempt to process the next command in the queue if not already processing
	processCommandQueue();

	// Update UI elements with the new position
	fus_mainwindow->ui.Gantry_x_spinBox->setValue(gantryPosition.x);
	fus_mainwindow->ui.Gantry_y_spinBox->setValue(gantryPosition.y);
	fus_mainwindow->ui.Gantry_z_spinBox->setValue(gantryPosition.z);
	//fus_mainwindow->emitPrintSignal("Position updated");
}