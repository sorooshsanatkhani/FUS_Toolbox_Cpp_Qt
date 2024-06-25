// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 21 June, 2024

#pragma once  // Ensures the header file is included only once in a single compilation

#include <QtWidgets/QMainWindow>  // Includes QMainWindow which provides a main application window
#include <QComboBox>
#include "ui_FUSMainWindow.h"  // Includes the UI for the FUSMainWindow class
#include "PicoScope.h"  // Includes the PicoScope class
#include "WaveformGenerator.h"
#include "Gantry.h"
#include <QProgressBar>
#include <QStateMachine>
#include <QState>
#include <QSignalTransition>

// Declares the FUSMainWindow class as a subclass of QMainWindow
class FUSMainWindow : public QMainWindow
{
    Q_OBJECT  // Enables the class to use signals and slots

public:
    FUSMainWindow(QWidget* parent = nullptr);  // Constructor
    ~FUSMainWindow();  // Destructor
    Ui::FUSMainWindowClass ui;  // Instance of the UI class

    void populateDIRComboBox(); // Method to populate the combo box

    void emitPrintSignal(const QString& text);  // Function to emit the printSignal

    int getTimebaseValue();  // Getter for the value of Timebase_spinBox
    int getBufferValue();  // Getter for the value of Buffer_spinBox
    int getYaxisRangeValue();  // Getter for the value of yaxisRange_lineEdit
    uint16_t getRangeValue();  // Getter for the value of Range_comboBox
    uint16_t getTriggerVoltageValue();  // Getter for the value of TriggerVoltage_lineEdit

    /////// Waveform Generator
    unsigned int getFrequencyValue();
    unsigned int getAmplitudeValue();
    unsigned int getPulseDurationValue();
    unsigned int getDutyCycleValue();
    unsigned int getPRFValue();
    unsigned int getLengthValue();

public slots:
    //void handlePortOpened(bool opened);

signals:
    void printSignal(const QString& text);  // Signal to print text

private slots:
    void connectSignalsAndSlots();

    void updateTextBox(const QString& text);  // Slot to update the text box
    void handleSpinBoxValueChanged();  // Slot to handle value changes of the spin boxes
    void handleInitializeButton();
    void handleCloseButton();
    void handleReadButton();

    ///// Waveform Generator Functions //////
    void handleSpinBox_Waveform_ValueChanged();
    void handleSpinBox_PulseDuration_ValueChanged();
    void handleSpinBox_DutyCycle_ValueChanged();
    void handleSpinBox_PRF_ValueChanged();
    void handleCheckDeviceButton();
    void handleGenerateWaveformButton();
    void updateProgressBar();
    void handleAbortButton();
    
    // Gantry System Functions
    void handleGantry_right_ButtonClicked();
    void handleGantry_left_ButtonClicked();
    void handleGantry_up_ButtonClicked();
    void handleGantry_down_ButtonClicked();
    void handleGantry_forward_ButtonClicked();
    void handleGantry_backward_ButtonClicked();
    void handleGantry_move_ButtonClicked();
    void handleGantry_stop_ButtonClicked();
    // Gantry ON/OFF Toggle Button
    void setupGantryToggleButton(); // Setup method for the Gantry ON/OFF toggle button
    void handleGantryToggleButtonClicked(); // Slot to handle Gantry ON/OFF button clicks
    void handleGantry_set_ButtonClicked();
    void handleGantry_return_ButtonClicked();
    void handleGantry_movetoposition_ButtonClicked();

private:
    PicoScope* picoScope;  // Pointer to a PicoScope object
    WaveformGenerator* waveformgenerator;
    Gantry* gantry;
    QProgressBar* progressBar;
    QElapsedTimer elapsedTimer;
    QTimer* progressTimer;
    QTimer* completionTimer;

    QGroupBox* waveformGroupBox;
};