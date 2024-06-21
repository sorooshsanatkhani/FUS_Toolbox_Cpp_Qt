// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 26 October, 2023

#ifndef PICOSCOPE_H  // Include guard to prevent multiple inclusions
#define PICOSCOPE_H

#include <QObject>  // Includes the QObject class for creating custom Qt objects
#include <QTimer>  // Includes the QTimer class for creating timers
#include <random>  // Includes the random library for generating random numbers
#include <deque>  // Includes the deque library for using double-ended queues
#include "Resources/qcustomplot.h"  // Includes the QCustomPlot library for plotting
#include "windows.h"  // Includes the windows library for using Windows APIs
#include <conio.h>  // Includes the conio library for console input/output
#include <stdio.h>  // Includes the stdio library for standard input/output
#include "Resources/ps4000.h"  // Includes the ps4000 library for using the PicoScope 4000 series

class FUSMainWindow;  // Forward declaration of the FUSMainWindow class

// Declares the PicoScope class as a subclass of QObject
class PicoScope : public QObject
{
    Q_OBJECT  // Enables the class to use signals and slots

public:
    // Declares a struct to hold the parameters
    struct Parameters
    {
        unsigned int Timebase;
        int32_t Buffer;
    };

    struct PicoScope_Vars
    {
        PICO_STATUS status_open = 1;
        PICO_STATUS status_close = 1;
        PICO_STATUS status_setBuffer;
        PICO_STATUS status_RunBlock;
        PICO_STATUS status_GetValues;
        PICO_STATUS status_Stop;
        UNIT_MODEL  unit;
    };
    PicoScope_Vars picoVar;

    explicit PicoScope(FUSMainWindow* parent = nullptr);  // Constructor
    ~PicoScope();  // Destructor

    Parameters readParameters();  // Function to read the parameters
    PicoScope_Vars initializePicoScope();
    PicoScope_Vars closePicoScope();
    void readBlockPicoScope();  // Function to read the PicoScope in block mode

    int y_limit;

private slots:
    void plotPico();  // Slot to plot the PicoScope data

private:
    std::deque<long> MV_numbers;  // Deque to hold the MV numbers
    std::deque<int64_t> t_numbers;  // Deque to hold the t numbers
    QCustomPlot* customPlot;  // Pointer to a QCustomPlot object
    FUSMainWindow* fus_mainwindow;  // Pointer to a FUSMainWindow object

public:
    QCustomPlot* getCustomPlot() const { return customPlot; }  // Getter for the customPlot
};

#endif // PICOSCOPE_H