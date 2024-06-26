// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 21 June, 2024

#include "stdafx.h"  // Includes precompiled header files
#include "Resources/qcustomplot.h"  // Includes the QCustomPlot library for plotting
#include <sstream>
#include "FUSMainWindow.h"  // Includes the FUSMainWindow class
#include "WaveformGenerator.h"
#include "Gantry.h"

using namespace std;

// Defines the constructor of the FUSMainWindow class
FUSMainWindow::FUSMainWindow(QWidget* parent)
    : QMainWindow(parent),
    picoScope(new PicoScope(this)),
    waveformgenerator(new WaveformGenerator(this)),
    gantry(new Gantry(this)),
    completionTimer(new QTimer(this)),
    progressTimer(new QTimer(this))
{
    ui.setupUi(this);
    this->setWindowIcon(QIcon(":/FUSMainWindow/Resources/logo.ico"));
    ui.verticalLayout->addWidget(picoScope->getCustomPlot());

    populateDIRComboBox(); // Now populate the combo box for the Gantry system direction

    // Connections
    connectSignalsAndSlots();

    // Initialize the progress bar for the waveform generation
    progressBar = ui.progressBar;

    // Initialize completionTimer
    completionTimer = nullptr;

    ui.readButton->setEnabled(false);
    ui.CloseButton->setEnabled(false);

    ui.Abort_Button->setEnabled(false);

    // ON/OFF toggle Button Setup for the Gantry system using existing Gantry_ONOFF_Button
    setupGantryToggleButton();
}

// Defines the destructor of the FUSMainWindow class
FUSMainWindow::~FUSMainWindow()
{
    delete picoScope;
    delete waveformgenerator;

    // Properly delete timers to avoid memory leaks
    if (progressTimer)
    {
        progressTimer->stop();
        delete progressTimer;
    }

    delete completionTimer;
    delete gantry;
}

void FUSMainWindow::connectSignalsAndSlots()
{
    // Connects the clicked signal of buttons to their respective slots
    connect(ui.InitializeButton, &QPushButton::clicked, this, &FUSMainWindow::handleInitializeButton);
    connect(ui.readButton, &QPushButton::clicked, this, &FUSMainWindow::handleReadButton);
    connect(ui.CloseButton, &QPushButton::clicked, this, &FUSMainWindow::handleCloseButton);

    // Connects the printSignal of PicoScope to the updateTextBox slot
    connect(this, &FUSMainWindow::printSignal, this, &FUSMainWindow::updateTextBox);

    // Connects the valueChanged signal of the spin boxes to the handleSpinBoxValueChanged slot
    connect(ui.Timebase_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FUSMainWindow::handleSpinBoxValueChanged);
    connect(ui.Buffer_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FUSMainWindow::handleSpinBoxValueChanged);
    connect(ui.yaxisRange_lineEdit, &QLineEdit::textChanged, this, &FUSMainWindow::handleSpinBoxValueChanged);
    connect(ui.Range_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FUSMainWindow::handleSpinBoxValueChanged);
    connect(ui.TriggerVoltage_lineEdit, &QLineEdit::textChanged, this, &FUSMainWindow::handleSpinBoxValueChanged);

    // Connects the clicked signal of Waveform Generator buttons to their respective slots
    connect(ui.CheckDeviceButton, &QPushButton::clicked, this, &FUSMainWindow::handleCheckDeviceButton);
    connect(ui.Frequency_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FUSMainWindow::handleSpinBox_Waveform_ValueChanged);
    connect(ui.Amplitude_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FUSMainWindow::handleSpinBox_Waveform_ValueChanged);
    connect(ui.PulseDuration_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FUSMainWindow::handleSpinBox_PulseDuration_ValueChanged);
    connect(ui.DutyCycle_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FUSMainWindow::handleSpinBox_DutyCycle_ValueChanged);
    connect(ui.PRF_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FUSMainWindow::handleSpinBox_PRF_ValueChanged);
    connect(ui.Length_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &FUSMainWindow::handleSpinBox_Waveform_ValueChanged);
    connect(ui.GenerateWaveform_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGenerateWaveformButton);
    connect(ui.Abort_Button, &QPushButton::clicked, this, &FUSMainWindow::handleAbortButton);

    // Connects the UI parts related to Gantry system to their respective slots
    connect(ui.Gantry_open_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_open_ButtonClicked);
    connect(ui.Gantry_right_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_right_ButtonClicked);
    connect(ui.Gantry_left_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_left_ButtonClicked);
    connect(ui.Gantry_up_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_up_ButtonClicked);
    connect(ui.Gantry_down_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_down_ButtonClicked);
    connect(ui.Gantry_forward_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_forward_ButtonClicked);
    connect(ui.Gantry_backward_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_backward_ButtonClicked);
    connect(ui.Gantry_move_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_move_ButtonClicked);
    connect(ui.Gantry_stop_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_stop_ButtonClicked);
    connect(ui.Gantry_set_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_set_ButtonClicked);
    connect(ui.Gantry_return_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_return_ButtonClicked);
    connect(ui.Gantry_movetoposition_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantry_movetoposition_ButtonClicked);
}

// Defines the updateTextBox slot
void FUSMainWindow::updateTextBox(const QString& text)
{
    ui.textEdit->append(text);  // Appends the text to the textEdit
}

void FUSMainWindow::emitPrintSignal(const QString& text)
{
    emit printSignal(text);
}

void FUSMainWindow::handleInitializeButton()
{
    ui.readButton->setEnabled(true);
    ui.CloseButton->setEnabled(true);
    PicoScope::PicoScope_Vars pico_vars = picoScope->initializePicoScope();  // Reads the parameters from the spin boxes
}

void FUSMainWindow::handleReadButton()
{
    picoScope->readBlockPicoScope();  // Reads the parameters from the spin boxes
}

void FUSMainWindow::handleCloseButton()
{
    PicoScope::PicoScope_Vars pico_vars = picoScope->closePicoScope();  // Reads the parameters from the spin boxes
    ui.readButton->setEnabled(false);
    ui.CloseButton->setEnabled(false);
}

// Defines the handleSpinBoxValueChanged slot
void FUSMainWindow::handleSpinBoxValueChanged()
{
    PicoScope::Parameters params = picoScope->readParameters();  // Reads the parameters from the spin boxes
    std::ostringstream string_to_print;
    string_to_print.precision(3);
    string_to_print << std::fixed << (params.Timebase + 1) / 10.;
    ui.SamplingInterval_label->setText(QString::fromStdString("Sampling interval = " + std::move(string_to_print).str() + " ms"));
    string_to_print << std::fixed << (((params.Timebase + 1) / 10000.) * params.Buffer);
    ui.BlockDuration_label->setText(QString::fromStdString("Block duration =  " + std::move(string_to_print).str() + " ms"));
}

// Defines the getter for the value of Timebase_spinBox
int FUSMainWindow::getTimebaseValue()
{
    return ui.Timebase_spinBox->value();  // Returns the value of Timebase_spinBox
}

// Defines the getter for the value of Buffer_spinBoxOFF
int FUSMainWindow::getBufferValue()
{
    return ui.Buffer_spinBox->value();  // Returns the value of Buffer_spinBox
}

// Defines the getter for the value of yaxisRange_lineEdit
int FUSMainWindow::getYaxisRangeValue()
{
    return ui.yaxisRange_lineEdit->text().toInt();  // Returns the value of yaxisRange_lineEdit
}

uint16_t FUSMainWindow::getRangeValue()
{
    return ui.Range_comboBox->currentIndex();  // Returns the value of Range_comboBox
}

uint16_t FUSMainWindow::getTriggerVoltageValue()
{
    return ui.TriggerVoltage_lineEdit->text().toInt();  // Returns the value of TriggerVoltage_lineEdit
}

///////////////////////////////////
/////// Waveform Generator ///////
void FUSMainWindow::handleSpinBox_Waveform_ValueChanged()
{
    waveformgenerator->readParameters(
        getFrequencyValue(),
        getAmplitudeValue(),
        getPulseDurationValue(),
        getDutyCycleValue(),
        getPRFValue(),
        getLengthValue());  // Reads the parameters from the spin boxes
}

void FUSMainWindow::handleSpinBox_PulseDuration_ValueChanged()
{
    ui.PulseDuration_spinBox->setValue(round(ui.PulseDuration_spinBox->value() * 10.) / 10.);
    ui.DutyCycle_spinBox->setValue(round(((ui.PulseDuration_spinBox->value() / 1000.) * ui.PRF_spinBox->value()) * 100. * 10) / 10.);
    waveformgenerator->readParameters(
        getFrequencyValue(),
        getAmplitudeValue(),
        getPulseDurationValue(),
        getDutyCycleValue(),
        getPRFValue(),
        getLengthValue());
}

void FUSMainWindow::handleSpinBox_DutyCycle_ValueChanged()
{
    ui.DutyCycle_spinBox->setValue(round(ui.DutyCycle_spinBox->value() * 10.) / 10.);
    ui.PRF_spinBox->setValue(round((ui.DutyCycle_spinBox->value() / 100.) / (ui.PulseDuration_spinBox->value() / 1000.) * 10.) / 10.);
    waveformgenerator->readParameters(
        getFrequencyValue(),
        getAmplitudeValue(),
        getPulseDurationValue(),
        getDutyCycleValue(),
        getPRFValue(),
        getLengthValue());// Reads the parameters from the spin boxes
}

void FUSMainWindow::handleSpinBox_PRF_ValueChanged()
{
    ui.PRF_spinBox->setValue(round(ui.PRF_spinBox->value() * 10.) / 10.);
    ui.PulseDuration_spinBox->setValue(round(((ui.DutyCycle_spinBox->value() / 100.) / ui.PRF_spinBox->value()) * 1000. * 10.) / 10.);
    waveformgenerator->readParameters(
        getFrequencyValue(),
        getAmplitudeValue(),
        getPulseDurationValue(),
        getDutyCycleValue(),
        getPRFValue(),
        getLengthValue());// Reads the parameters from the spin boxes
}

unsigned int FUSMainWindow::getFrequencyValue()
{
    return ui.Frequency_spinBox->value();  // Returns the value of Length_spinBox
}

unsigned int FUSMainWindow::getAmplitudeValue()
{
    return ui.Amplitude_spinBox->value();  // Returns the value of Length_spinBox
}

unsigned int FUSMainWindow::getPulseDurationValue()
{
    return ui.PulseDuration_spinBox->value();  // Returns the value of Length_spinBox
}

unsigned int FUSMainWindow::getDutyCycleValue()
{
    return ui.DutyCycle_spinBox->value();  // Returns the value of Length_spinBox
}

unsigned int FUSMainWindow::getPRFValue()
{
    return ui.PRF_spinBox->value();  // Returns the value of Length_spinBox
}

unsigned int FUSMainWindow::getLengthValue()
{
    return ui.Length_spinBox->value();  // Returns the value of Length_spinBox
}

void FUSMainWindow::handleCheckDeviceButton()
{
    waveformgenerator->CheckDevice_Click();  // Reads the parameters from the spin boxes
}

void FUSMainWindow::handleGenerateWaveformButton()
{
    waveformgenerator->readParameters(
        getFrequencyValue(),
        getAmplitudeValue(),
        getPulseDurationValue(),
        getDutyCycleValue(),
        getPRFValue(),
        getLengthValue());

    // Start the elapsed timer
    elapsedTimer.start();

    progressBar->setRange(0, waveformgenerator->WaveformGenerator_Vars.Length);
    progressBar->setValue(0);

    ui.WaveformGenerator_GroupBox->setEnabled(false);
    ui.GenerateWaveform_Button->setEnabled(false);
    ui.Abort_Button->setEnabled(true);

    // Delete the old completionTimer if it exists and create a new completionTimer
    if (completionTimer) {
        completionTimer->stop();
        delete completionTimer;
        completionTimer = nullptr;
    }
    completionTimer = new QTimer(this);
    completionTimer->setSingleShot(true);
    // Connect the timeout signal
    connect(completionTimer, &QTimer::timeout, this, [this]() {
        // Emit a print signal
        emitPrintSignal("Waveform Generation Completed.");
        // Handle abort and cleanup
        handleAbortButton();
        });
    // Set the timeout for the completion timer to the length of the waveform
    completionTimer->start(waveformgenerator->WaveformGenerator_Vars.Length * 1000);

    // Start the waveform generation
    waveformgenerator->GenerateWaveform_Click();

    // Create a new progressTimer
    if (progressTimer) {
        progressTimer->stop();
        delete progressTimer;
        progressTimer = nullptr;
    }
    progressTimer = new QTimer(this);
    progressTimer->setSingleShot(false);
    // Connect the timeout signal for updating the progress bar
    connect(progressTimer, &QTimer::timeout, this, &FUSMainWindow::updateProgressBar);
    // Set the timeout interval to a reasonable value (e.g., 100 ms)
    int updateInterval = 100; // milliseconds
    progressTimer->start(updateInterval);
}

void FUSMainWindow::updateProgressBar()
{
    progressBar->setValue(elapsedTimer.elapsed() / 1000); // Update progress based on elapsed time

    if (progressBar->value() >= waveformgenerator->WaveformGenerator_Vars.Length)
    {
        // Ensure the progress bar doesn't go beyond the specified length
        progressBar->setValue(waveformgenerator->WaveformGenerator_Vars.Length);
    }
}

void FUSMainWindow::handleAbortButton()
{
    // Disconnect the progress timer to stop further updates
    disconnect(progressTimer, &QTimer::timeout, this, &FUSMainWindow::updateProgressBar);

    progressTimer->stop();
    delete progressTimer;
    progressTimer = nullptr;
    completionTimer->stop();
    delete completionTimer;
    completionTimer = nullptr;

    // Check if the progress bar is still less than the waveform length
    if (progressBar->value() < waveformgenerator->WaveformGenerator_Vars.Length-1)
        emitPrintSignal("Waveform Generation Aborted.");

    // Stop the waveform generation and the timer
    waveformgenerator->Stop(waveformgenerator->FuncGenOutput.Manager, waveformgenerator->FuncGenOutput.VISAsession, waveformgenerator->FuncGenOutput.deviceStatus);

    // Enable the Generate Waveform button and reset the progress bar
    ui.WaveformGenerator_GroupBox->setEnabled(true);
    ui.GenerateWaveform_Button->setEnabled(true);
    ui.Abort_Button->setEnabled(false);
    progressBar->setValue(waveformgenerator->WaveformGenerator_Vars.Length);
}
////////////////////////////////////////////////
/////// Gantry System /////////
// Setup for the Gantry ON/OFF toggle button
void FUSMainWindow::handleGantry_open_ButtonClicked()
{
    gantry->open_Click();
}

void FUSMainWindow::setupGantryToggleButton()
{
    QStateMachine* machine = new QStateMachine(this);

    QState* offState = new QState(machine);
    offState->assignProperty(ui.Gantry_onoff_Button, "styleSheet", "background-color: red");

    QState* onState = new QState(machine);
    onState->assignProperty(ui.Gantry_onoff_Button, "styleSheet", "background-color: #00FF00");

    // Transitions
    offState->addTransition(ui.Gantry_onoff_Button, &QPushButton::clicked, onState);
    onState->addTransition(ui.Gantry_onoff_Button, &QPushButton::clicked, offState);

    machine->setInitialState(offState);
    machine->start();

    // Connect the Gantry_onoff_Button's clicked signal to a slot to perform actions on toggle
    connect(ui.Gantry_onoff_Button, &QPushButton::clicked, this, &FUSMainWindow::handleGantryToggleButtonClicked);
}
// Slot for handling Gantry_onoff_Button clicks (if needed)
void FUSMainWindow::handleGantryToggleButtonClicked()
{

    // Check the current text of the toggle button to determine its state
    if (ui.Gantry_onoff_Button->styleSheet().contains("#00FF00")) {
        // If the button is in the ON state, call the close method
        gantry->off();
    }
    else {
        // If the button is in the OFF state, call the open method
        gantry->on();
    }
}

void FUSMainWindow::populateDIRComboBox() {
    QStringList options = { "Right", "Left", "Up", "Down", "Forward", "Backward"};
    ui.Gantry_DIR_comboBox->addItems(options);
}
void FUSMainWindow::handleGantry_right_ButtonClicked()
{
    gantry->move_Click('R', 0.1, 0.1);
}
void FUSMainWindow::handleGantry_left_ButtonClicked()
{
    gantry->move_Click('L', 0.1, 0.1);
}
void FUSMainWindow::handleGantry_up_ButtonClicked()
{
	gantry->move_Click('U', 0.1, 0.1);
}
void FUSMainWindow::handleGantry_down_ButtonClicked()
{
	gantry->move_Click('D', 0.1, 0.1);
}
void FUSMainWindow::handleGantry_forward_ButtonClicked()
{
	gantry->move_Click('F', 0.1, 0.1);
}
void FUSMainWindow::handleGantry_backward_ButtonClicked()
{
	gantry->move_Click('B', 0.1, 0.1);
}
void FUSMainWindow::handleGantry_move_ButtonClicked()
{
    gantry->move_Click(ui.Gantry_DIR_comboBox->currentText().toStdString()[0], ui.Gantry_distance_spinBox->value(), ui.Gantry_speed_spinBox->value());
}
void FUSMainWindow::handleGantry_stop_ButtonClicked()
{
	gantry->stop_Click();
}
void FUSMainWindow::handleGantry_set_ButtonClicked()
{
	gantry->setOrigin();
    ui.Gantry_return_Button->setEnabled(true);
    ui.Gantry_movetoposition_Button->setEnabled(true);
    ui.Gantry_x_spinBox->setEnabled(true);
    ui.Gantry_y_spinBox->setEnabled(true);
    ui.Gantry_z_spinBox->setEnabled(true);
}
void FUSMainWindow::handleGantry_return_ButtonClicked()
{
    ui.Gantry_return_Button->setEnabled(false);
	gantry->returnToOrigin();
}
void FUSMainWindow::handleGantry_movetoposition_ButtonClicked()
{
    gantry->gantriGoToPosition.x = ui.Gantry_x_spinBox->value();
	gantry->gantriGoToPosition.y = ui.Gantry_y_spinBox->value();
	gantry->gantriGoToPosition.z = ui.Gantry_z_spinBox->value();
	gantry->MoveTo();
}