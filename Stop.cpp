// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 26 October, 2023

#include "stdafx.h"
#include "WaveformGenerator.h"
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

void WaveformGenerator::Stop(ViSession defaultRM, ViSession instr, ViStatus status)
{
	status = viPrintf(instr, "C1:OUTP OFF\n");
	status = viClose(instr);
	status = viClose(defaultRM);

	if (status < VI_SUCCESS)
	{
		// Error handling
		status = viClose(instr);
		status = viClose(defaultRM);
	}
}
