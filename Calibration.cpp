#include "stdafx.h"
#include "Calibration.h"
#include "FUSMainWindow.h"

Calibration::Calibration(Gantry* gantry, WaveformGenerator* waveformGenerator, PicoScope* picoScope, FUSMainWindow* fus_mainwindow, QObject* parent)
    : QObject(parent), gantry(gantry), waveformGenerator(waveformGenerator), picoScope(picoScope), fus_mainwindow(fus_mainwindow)
{
}

Calibration::~Calibration()
{
}

void Calibration::scan3DVolume(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

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
                // Move to the next position
                gantry->move_Click('X', x, 5); // Example command, adjust as necessary
                gantry->move_Click('Y', y, 5);
                gantry->move_Click('Z', z, 5);


                // Record data
                recordData(filePath, x, y, z);

                // Example data writing format
                out << x << "," << y << "," << z << "," << "data_placeholder" << "\n";
            }
        }
    }

    file.close();
}

void Calibration::generatePulse()
{
    waveformGenerator->readParameters(
        500000,
        80,
        10,
        2,
        2,
        120);
    waveformGenerator->GenerateWaveform_Click();
    fus_mainwindow->ui.WaveformGenerator_GroupBox->setEnabled(false);
    fus_mainwindow->ui.GenerateWaveform_Button->setEnabled(false);
    fus_mainwindow->ui.Abort_Button->setEnabled(true);
}

void Calibration::recordData(const QString& filePath, int x, int y, int z)
{
    // Record data from the PicoScope
    // This is a placeholder function. Implement according to your project's requirements.
}
