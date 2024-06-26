// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 21 June, 2024

#include "stdafx.h"
#include "WaveformGenerator.h"
#include "FUSMainWindow.h"

using namespace std;

// Defines the constructor of the PicoScope class
WaveformGenerator::WaveformGenerator(FUSMainWindow* parent, PicoScope* picoScope) : 
	QObject(parent), fus_mainwindow(parent), picoScope(picoScope)
{
}

// Defines the destructor of the WaveformGenerator class
WaveformGenerator::~WaveformGenerator()
{
}

void WaveformGenerator::CheckDevice_Click()
{
	pair<std::wstring, int> devicestatus = GetDeviceStatus();

	if (devicestatus.second == 1)
	{
		fus_mainwindow->emitPrintSignal("Function generator is working!");
	}
	else
	{
		fus_mainwindow->emitPrintSignal("Error");
	}
}

void WaveformGenerator::readParameters(
	unsigned int F,
	unsigned int Vpp,
	unsigned int pulseDuration,
	unsigned int dutyCycle,
	unsigned int PRF,
	unsigned int Length)
{
	/*
	WaveformGenerator_Vars.Frequency		= fus_mainwindow->getFrequencyValue();  // Gets the value of 
	WaveformGenerator_Vars.Amplitude		= fus_mainwindow->getAmplitudeValue();  // Gets the value of 
	WaveformGenerator_Vars.PulseDuration	= fus_mainwindow->getPulseDurationValue();  // Gets the value of 
	WaveformGenerator_Vars.DutyCycle		= fus_mainwindow->getDutyCycleValue();  // Gets the value of 
	WaveformGenerator_Vars.PRF				= fus_mainwindow->getPRFValue();  // Gets the value of 
	WaveformGenerator_Vars.Length			= fus_mainwindow->getLengthValue();  // Gets the value of 
	*/
	WaveformGenerator_Vars.Frequency = F;
	WaveformGenerator_Vars.Amplitude = Vpp;
	WaveformGenerator_Vars.PulseDuration = pulseDuration;
	WaveformGenerator_Vars.DutyCycle = dutyCycle;
	WaveformGenerator_Vars.PRF = PRF;
	WaveformGenerator_Vars.Length = Length;

}

void WaveformGenerator::GenerateWaveform_Click()
{
	//DeviceOutput FuncGenOutput;
	FuncGenOutput = Burst_ON(
		WaveformGenerator_Vars.Frequency,	//Frequency().Value(),
		WaveformGenerator_Vars.Amplitude,	//Amplitudepp().Value(),
		WaveformGenerator_Vars.PulseDuration,	//PulseDuration().Value(),
		WaveformGenerator_Vars.DutyCycle);	//DutyCycle().Value());
}