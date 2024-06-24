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
			//break;
		case 'L':
			gantryPosition.x -= Distance;
			//break;
		case 'U':
			gantryPosition.z += Distance;
			//break;
		case 'D':
			gantryPosition.z -= Distance;
			//break;
		case 'F':
			gantryPosition.y += Distance;
			//break;
		case 'B':
			gantryPosition.y -= Distance;
			//break;
	}
	arduino->write(Direction, Distance, Speed);
	fus_mainwindow->ui.Gantry_x_spinBox->setValue(gantryPosition.x);
	fus_mainwindow->ui.Gantry_y_spinBox->setValue(gantryPosition.y);
	fus_mainwindow->ui.Gantry_z_spinBox->setValue(gantryPosition.z);
}