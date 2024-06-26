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

class Calibration : public QObject
{
    Q_OBJECT
public:
    explicit Calibration(Gantry* gantry, WaveformGenerator* waveformGenerator, PicoScope* picoScope, QObject* parent = nullptr);
    void scan3DVolume(const QString& filePath);

private:
    Gantry* gantry;
    WaveformGenerator* waveformGenerator;
    PicoScope* picoScope;

    void moveToNextPosition(int& x, int& y, int& z);
    void generatePulse();
    void recordData(const QString& filePath, int x, int y, int z);
};

#endif // CALIBRATION_H