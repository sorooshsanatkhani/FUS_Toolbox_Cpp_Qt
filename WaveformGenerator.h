// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 26 October, 2023

#ifndef WAVEFORMGENERATOR_H  // Include guard to prevent multiple inclusions
#define WAVEFORMGENERATOR_H

#pragma once
#include <visa.h>
#pragma comment(lib,"visa64.lib")

#include <QObject>  // Includes the QObject class for creating custom Qt objects
#include <QTimer>  // Includes the QTimer class for creating timers

class FUSMainWindow;
class PicoScope;

class WaveformGenerator : public QObject
{

    Q_OBJECT  // Enables the class to use signals and slots

public:

    struct Parameters
    {
        unsigned int Frequency;
        unsigned int Amplitude;
        unsigned int PulseDuration;
        unsigned int DutyCycle;
        unsigned int PRF;
        unsigned int Length;
    };
    Parameters WaveformGenerator_Vars;

    struct DeviceOutput
    {
        std::wstring Message;
        bool ConnectionStatus{};
        ViSession VISAsession{};
        ViSession Manager{};
        ViStatus deviceStatus{};
    };
    DeviceOutput FuncGenOutput;

    explicit WaveformGenerator(FUSMainWindow* parent = nullptr, PicoScope* picoScope = nullptr);  // Constructor
    ~WaveformGenerator();  // Destructor

    void readParameters();  // Function to read the parameters
    std::pair<std::wstring, int> GetDeviceStatus();
    DeviceOutput Burst_ON(int,int, double, double);
    void Stop(ViSession, ViSession, ViStatus);

    void CheckDevice_Click();

    void GenerateWaveform_Click();

    std::wstring removeEnd(std::wstring str);

private:

    FUSMainWindow* fus_mainwindow;  // Pointer to a FUSMainWindow object
    PicoScope* picoScope;
};

#endif // WAVEFORMGENERATOR_H