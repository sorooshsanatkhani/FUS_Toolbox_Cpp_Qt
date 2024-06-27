// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 21 June, 2024

#include "stdafx.h"  // Includes precompiled header files
#include "PicoScope.h"  // Includes the PicoScope class
#include <iostream>  // Includes the iostream library for standard input/output
#include <string>  // Includes the string library for using strings
#include <chrono>
#include <thread>
#include "FUSMainWindow.h"  // Includes the FUSMainWindow class



using namespace std;  // Uses the standard namespace

// Defines the constructor of the PicoScope class
PicoScope::PicoScope(FUSMainWindow* parent) : QObject(parent), fus_mainwindow(parent)
{
    customPlot = new QCustomPlot();  // Creates a new QCustomPlot object
    y_limit = 0;
}

// Defines the destructor of the PicoScope class
PicoScope::~PicoScope()
{
}

// Defines the function to read the parameters
PicoScope::Parameters PicoScope::readParameters()
{
    Parameters params;  // Declares a Parameters struct
    params.Timebase = fus_mainwindow->getTimebaseValue();  // Gets the value of Timebase_spinBox
    params.Buffer = fus_mainwindow->getBufferValue();  // Gets the value of Buffer_spinBox
    return params;  // Returns the Parameters struct
}

PicoScope::PicoScope_Vars PicoScope::initializePicoScope()
{
    if (picoVar.status_open != 0)
    {
        picoVar.status_open = ps4000OpenUnit(&(picoVar.unit.handle));
        picoVar.status_close = 1;
        get_info(&picoVar.unit);
        picoVar.unit.channelSettings[1].enabled = FALSE;
        picoVar.unit.channelSettings[0].enabled = TRUE;
        picoVar.unit.channelSettings[0].DCcoupled = TRUE;
        picoVar.status_setBuffer = 0;
        picoVar.status_RunBlock = 0;
        picoVar.status_GetValues = 0;
        picoVar.status_Stop = 0;
        fus_mainwindow->emitPrintSignal(QString::fromStdString("Unit opened!"));
        fus_mainwindow->emitPrintSignal(QString::fromStdString("status_open =  " + to_string(picoVar.status_open)));
    }
    return picoVar;
}

void PicoScope::readBlockPicoScope()
{
    fus_mainwindow->emitPrintSignal("Initialize reading...");
    ///////////// Set parameters ///////////////////
    SetParameters(readParameters().Timebase, 1, TRUE, 0, readParameters().Buffer);
    fus_mainwindow->emitPrintSignal("Parameters set.");
    switch (fus_mainwindow->getRangeValue())
    {
    case 0:
        picoVar.unit.channelSettings[0].range = PS4000_10MV;
        break;
    case 1:
        picoVar.unit.channelSettings[0].range = PS4000_20MV;
        break;
    case 2:
        picoVar.unit.channelSettings[0].range = PS4000_50MV;
        break;
    case 3:
        picoVar.unit.channelSettings[0].range = PS4000_100MV;
        break;
    case 4:
        picoVar.unit.channelSettings[0].range = PS4000_200MV;
        break;
    case 5:
        picoVar.unit.channelSettings[0].range = PS4000_500MV;
        break;
    case 6:
        picoVar.unit.channelSettings[0].range = PS4000_1V;
        break;
    case 7:
        picoVar.unit.channelSettings[0].range = PS4000_2V;
        break;
    case 8:
        picoVar.unit.channelSettings[0].range = PS4000_5V;
        break;
    case 9:
        picoVar.unit.channelSettings[0].range = PS4000_10V;
        break;
    case 10:
        picoVar.unit.channelSettings[0].range = PS4000_20V;
        break;
    case 11:
        picoVar.unit.channelSettings[0].range = PS4000_50V;
        break;
    }
    picoData.MV_numbers.clear();
    picoData.t_numbers.clear();
    ////////////////////////////////////////////////

    //////////// Setting up the trigger /////////
    /////////////////////////////////////////////
    uint16_t trigger_thr = fus_mainwindow->getTriggerVoltageValue();
    int16_t	triggerVoltage = mv_to_adc(trigger_thr, picoVar.unit.channelSettings[PS4000_CHANNEL_A].range); // ChannelInfo stores ADC counts

    struct tTriggerChannelProperties sourceDetails = { triggerVoltage,
                                  (uint16_t)(trigger_thr/2),
                                  triggerVoltage,
                                  (uint16_t)(trigger_thr/2),
                                  PS4000_CHANNEL_A,
                                  LEVEL };

    struct tTriggerConditions conditions = { CONDITION_TRUE,
                              CONDITION_DONT_CARE,
                              CONDITION_DONT_CARE,
                              CONDITION_DONT_CARE,
                              CONDITION_DONT_CARE,
                              CONDITION_DONT_CARE,
                              CONDITION_DONT_CARE };

    struct tPwq pulseWidth;

    struct tTriggerDirections directions = { RISING,
                              NONE,
                              NONE,
                              NONE,
                              NONE,
                              NONE };

    memset(&pulseWidth, 0, sizeof(struct tPwq));

    fus_mainwindow->emitPrintSignal("Collect block triggered...");
        
    SetDefaults(&picoVar.unit);

    /* Trigger enabled
    * Rising edge*/
    SetTrigger(picoVar.unit.handle, &sourceDetails, 1, &conditions, 1, &directions, &pulseWidth, 0, 0, 0);

    ////////////////////////////////////////////////////
    //////////// Reading a block of samples ////////////
    int32_t i;
    int32_t timeInterval;
    int32_t sampleCount = readParameters().Buffer;
    FILE* fp = NULL;
    int32_t maxSamples;
    int16_t* buffers[2];
    int32_t timeIndisposed;
    int32_t offset = 0;

    buffers[0] = (int16_t*)malloc(sampleCount * sizeof(int16_t));
    buffers[1] = (int16_t*)malloc(sampleCount * sizeof(int16_t));
    picoVar.status_setBuffer = ps4000SetDataBuffers(picoVar.unit.handle, (PS4000_CHANNEL)0, buffers[0], buffers[1], sampleCount);

    fus_mainwindow->emitPrintSignal(QString::fromStdString("ps4000SetDataBuffers(channel " + to_string(0) + "------------" + to_string(picoVar.status_setBuffer)));

    /*
    * Find the maximum number of samples, and the time interval (in nanoseconds), at the current timebase if it is valid.
    * If the timebase index is not valid, increment by 1 and try again.
    */
    while (ps4000GetTimebase(picoVar.unit.handle, timebase, sampleCount, &timeInterval, oversample, &maxSamples, 0))
    {
        timebase++;
    }

    fus_mainwindow->emitPrintSignal(QString::fromStdString("timebase: " + to_string(timebase) + "------- oversample: " + to_string(oversample)));

    /* Start it collecting, then wait for completion*/
    g_ready = FALSE;
    picoVar.status_RunBlock = ps4000RunBlock(picoVar.unit.handle, 0, sampleCount, timebase, oversample, &timeIndisposed, 0, CallBackBlock, NULL);

    fus_mainwindow->emitPrintSignal(QString::fromStdString("BlockDataHandler:ps4000RunBlock ------ " + to_string(picoVar.status_RunBlock)));

    const auto maxWaitTime = std::chrono::seconds(5);
    auto startTime = std::chrono::steady_clock::now();
    while (!g_ready) {
        // Check if the maximum wait time has been reached
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime >= maxWaitTime) {
            fus_mainwindow->emitPrintSignal("Timeout!");
            break;
        }

        // Sleep for a short duration before checking again
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    

    if (g_ready)    // g_read == TRUE --> done reading data
    {
        picoVar.status_GetValues = ps4000GetValues(picoVar.unit.handle, 0, (uint32_t*)&sampleCount, 1, RATIO_MODE_NONE, 0, NULL);
        fus_mainwindow->emitPrintSignal(QString::fromStdString("BlockDataHandler:ps4000GetValues ------ " + to_string(picoVar.status_GetValues)));
            
        sampleCount = __min(sampleCount, BUFFER_SIZE);


        for (i = 0; i < sampleCount; i++)
        {
            picoData.t_numbers.push_back(g_times[0] + (int64_t)(i * timeInterval));
            picoData.MV_numbers.push_back(adc_to_mv(buffers[0][i], picoVar.unit.channelSettings[PS4000_CHANNEL_A].range));
        }

        plotPico();
    }
    else
    {
        fus_mainwindow->emitPrintSignal("data collection aborted");
    }

    if ((picoVar.status_Stop = ps4000Stop(picoVar.unit.handle)) != PICO_OK)
    {
        fus_mainwindow->emitPrintSignal(QString::fromStdString("BlockDataHandler:ps4000Stop ------ 0x%08lx " + to_string(picoVar.status_Stop)));
    }

    for (i = 0; i < 2; i++)
    {
        free(buffers[i]);
    }
}

PicoScope::PicoScope_Vars PicoScope::closePicoScope()
{
    if ((picoVar.status_close != 0) && (picoVar.status_open == 0))
    {
        picoVar.status_close = ps4000CloseUnit(picoVar.unit.handle);
        fus_mainwindow->emitPrintSignal(QString::fromStdString("Unit Closed!"));
        picoVar.status_open = 1;
        picoVar.status_setBuffer = 0;
        picoVar.status_RunBlock = 0;
        picoVar.status_GetValues = 0;
        picoVar.status_Stop = 0;
    }
    return picoVar;
}

void PicoScope::plotPico()
{
    QVector<double> x(picoData.t_numbers.size()), y(picoData.MV_numbers.size());
    fus_mainwindow->emitPrintSignal(QString::fromStdString("t_numbers size = " + to_string(picoData.t_numbers.size())));
    fus_mainwindow->emitPrintSignal(QString::fromStdString("t_numbers last element = " + to_string(picoData.t_numbers.back())));
    double max_t = picoData.t_numbers.back() / pow(10, 6);
    QString xLabel = "ms";
    double scale = pow(10, 6);
    if (max_t < 1) {
        scale = pow(10, 3);
        xLabel = QString::fromUtf8("\xC2\xB5s");
        if (max_t < 0.001) {
            scale = 1;
            xLabel = "ns";
        }
    }
    else if (max_t > 1000) {
        scale = pow(10, 9);
        xLabel = "s";
    }
    for (int i = 0; i < picoData.MV_numbers.size(); ++i)
    {
        x[i] = picoData.t_numbers[i] / scale;
        y[i] = picoData.MV_numbers[i];
    }
    y_limit = fus_mainwindow->getYaxisRangeValue();
    // clear existing graphs:
    customPlot->clearGraphs();
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->xAxis->setLabel(xLabel);
    customPlot->yAxis->setLabel("mV");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, picoData.t_numbers.back() / scale);
    customPlot->yAxis->setRange(-y_limit, y_limit);
    customPlot->replot();
}
void PicoScope::writePicoDataToBinaryFile()
{
    // Create the directory name with the current date
    QString dirName = "Data" + QDate::currentDate().toString("yyyyMMdd");
    QDir dir(dirName);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    // Construct the file name
    QString fileName = dir.absolutePath() + "/PicoData_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".bin";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        fus_mainwindow->emitPrintSignal("Unable to open file for writing: " + fileName);
        return;
    }

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);  // Assuming little endian for binary data

    // Write t_numbers and MV_numbers to the binary file
    for (int i = 0; i < picoData.t_numbers.size(); ++i)
    {
        out << picoData.t_numbers[i];
        out << static_cast<qint64>(picoData.MV_numbers[i]);
    }

    file.close();
    fus_mainwindow->emitPrintSignal("Data written to binary file: " + fileName);
}
