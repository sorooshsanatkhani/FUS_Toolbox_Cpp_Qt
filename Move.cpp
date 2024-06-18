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
	arduino->write(Direction, Distance, Speed);
}