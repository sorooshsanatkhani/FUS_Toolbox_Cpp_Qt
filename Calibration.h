#pragma once
#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "Gantry.h"
#include "WaveformGenerator.h"
#include "PicoScope.h"

class FUSMainWindow;

class Calibration : public QObject
{
    Q_OBJECT
public:
    explicit Calibration(
        Gantry* gantry = nullptr,
        WaveformGenerator* waveformGenerator = nullptr,
        PicoScope* picoScope = nullptr,
        FUSMainWindow* fus_mainwindow = nullptr,
        QObject* parent = nullptr);
    ~Calibration();  // Destructor
    
    void scan3DVolume(const QString& filePath);

private:
    Gantry* gantry;
    WaveformGenerator* waveformGenerator;
    PicoScope* picoScope;
    FUSMainWindow* fus_mainwindow;

    void moveToNextPosition(int& x, int& y, int& z);
    void generatePulse();
    void recordData(const QString& filePath, int x, int y, int z);
};

#endif // CALIBRATION_H