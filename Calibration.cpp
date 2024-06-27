#include "stdafx.h"
#include "Calibration.h"
#include "FUSMainWindow.h"
#include <QEventLoop>

Calibration::Calibration(Gantry* gantry, WaveformGenerator* waveformGenerator, PicoScope* picoScope, ArduinoDevice* Arduino, FUSMainWindow* fus_mainwindow, QObject* parent)
    : QObject(parent), gantry(gantry), waveformGenerator(waveformGenerator), picoScope(picoScope), Arduino(Arduino), fus_mainwindow(fus_mainwindow)
{
}

Calibration::~Calibration()
{
}

void Calibration::scan3DVolume()
{
    // Define the 3D volume bounds and step size
    int xMax = 10, yMax = 10, zMax = 10;
    int stepSize = 1;

    // Generate a pulse
    generatePulse();

    for (int x = 0; x <= xMax; x += stepSize)
    {
        for (int y = 0; y <= yMax; y += stepSize)
        {
            for (int z = 0; z <= zMax; z += stepSize)
            {
                QEventLoop loop;
                connect(Arduino, &ArduinoDevice::gantryReady, &loop, &QEventLoop::quit);
                
                // Move to the next position
                gantry->move_Click('R', x, 5); // Example command, adjust as necessary
                gantry->move_Click('F', y, 5);
                gantry->move_Click('U', z, 5);

                loop.exec(); // Wait here until gantryReady is emitted
                // Record data
                recordData(x, y, z);
            }
        }
    }
}

void Calibration::generatePulse()
{
    fus_mainwindow->Calibration_Pulse();
}

void Calibration::recordData(int x, int y, int z)
{
    picoScope->writePicoDataToBinaryFile(x,y,z);
}