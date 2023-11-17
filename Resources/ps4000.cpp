/*******************************************************************************
 *
 * Filename: ps4000.cpp
 *
 * Description:
 *   This is a console mode program that demonstrates how to perform
 *	 operations using a PicoScope 4000 Series device using the
 *	 PicoScope 4000 Series (ps4000) driver API functions.
 *
 *  Supported PicoScope models:
 *
 *		PicoScope 4223, 4224 & 4224 IEPE
 *		PicoScope 4423 & 4424
 *		PicoScope 4226 & 4227
 *		PicoScope 4262
 *
 * Examples:
 *    Collect a block of samples immediately
 *    Collect a block of samples when a trigger event occurs
 *	  Collect a block of samples using Equivalent Time Sampling (ETS)
 *    Collect samples using a rapid block capture with trigger
 *	  Collect samples using a rapid block capture without a trigger
 *    Collect a stream of data immediately
 *    Collect a stream of data when a trigger event occurs
 *    Set Signal Generator (where available), using built in or custom signals
 *
 *	To build this application:-
 *
 *		If Microsoft Visual Studio (including Express) is being used:
 *
 *			Select the solution configuration (Debug/Release) and platform (Win32/x64)
 *			Ensure that the 32-/64-bit ps2000a.lib can be located
 *			Ensure that the ps4000Api.h and PicoStatus.h files can be located
 *
 *		Otherwise:
 *
 *			Set up a project for a 32-/64-bit console mode application
 *			Add this file to the project
 *			Add ps4000.lib to the project (Microsoft C only)
 *			Add ps4000Api.h and PicoStatus.h to the project
 *			Build the project
 *
 *  Linux platforms:
 *
 *		Ensure that the libps4000 driver package has been installed using the
 *		instructions from https://www.picotech.com/downloads/linux
 *
 *		Place this file in the same folder as the files from the linux-build-files
 *		folder. In a terminal window, use the following commands to build
 *		the ps4000Con application:
 *
 *			./autogen.sh <ENTER>
 *			make <ENTER>
 *
 * Copyright (C) 2009-2019 Pico Technology Ltd. See LICENSE file for terms.
 *
 ******************************************************************************/

#include "ps4000.h"

int32_t BUFFER_SIZE;
int16_t SEGMEM;
int32_t     cycles;
uint32_t	timebase;
int16_t		oversample;
BOOL		scaleVoltages;

uint16_t inputRanges[PS4000_MAX_RANGES] = {
                        10,
                        20,
                        50,
                        100,
                        200,
                        500,
                        1000,
                        2000,
                        5000,
                        10000,
                        20000,
                        50000};


int16_t     g_ready = FALSE;
int64_t 	g_times[PS4000_MAX_CHANNELS];
int16_t     g_timeUnit;
int32_t     g_sampleCount;
uint32_t	g_startIndex;
int16_t		g_autoStop;
int16_t		g_trig = 0;
uint32_t	g_trigAt = 0;

/****************************************************************************
* Callback
* used by ps4000 data streaming collection calls, on receipt of data.
* used to set global flags etc checked by user routines
* This callback copies data into application buffers that are the same size
* as the driver buffers. An alternative would be to specify application
* buffers large enough to store all the data and copy in the data from the
* driver into the correct location.
****************************************************************************/
void PREF4 CallBackStreaming
(
    int16_t handle,
    int32_t noOfSamples,
    uint32_t startIndex,
    int16_t overflow,
    uint32_t triggerAt,
    int16_t triggered,
    int16_t autoStop,
    void* pParameter
)
{
    int32_t channel;
    BUFFER_INFO* bufferInfo = NULL;

    if (pParameter != NULL)
    {
        bufferInfo = (BUFFER_INFO*)pParameter;
    }

    // Copy data in callback
    if (bufferInfo != NULL && noOfSamples)
    {

        for (channel = 0; channel < bufferInfo->unit->channelCount; channel++)
        {
            if (bufferInfo->unit->channelSettings[channel].enabled)
            {
                if (bufferInfo->appBuffers && bufferInfo->driverBuffers)
                {
                    // Max buffers
                    if (bufferInfo->appBuffers[channel * 2] && bufferInfo->driverBuffers[channel * 2])
                    {
                        memcpy_s(&bufferInfo->appBuffers[channel * 2][startIndex], noOfSamples * sizeof(int16_t),
                            &bufferInfo->driverBuffers[channel * 2][startIndex], noOfSamples * sizeof(int16_t));
                    }

                    // Min buffers
                    if (bufferInfo->appBuffers[channel * 2 + 1] && bufferInfo->driverBuffers[channel * 2 + 1])
                    {
                        memcpy_s(&bufferInfo->appBuffers[channel * 2 + 1][startIndex], noOfSamples * sizeof(int16_t),
                            &bufferInfo->driverBuffers[channel * 2 + 1][startIndex], noOfSamples * sizeof(int16_t));
                    }
                }
            }
        }
    }

    // Used for streaming
    g_sampleCount = noOfSamples;
    g_startIndex = startIndex;
    g_autoStop = autoStop;

    // Flags to show if & where a trigger has occurred
    g_trig = triggered;
    g_trigAt = triggerAt;

    // Flag to say done reading data
    g_ready = TRUE;

}

/****************************************************************************
* Callback
* used by ps4000 data block collection calls, on receipt of data.
* used to set global flags etc checked by user routines
****************************************************************************/
void PREF4 CallBackBlock
(
    int16_t handle,
    PICO_STATUS status,
    void* pParameter
)
{
    // flag to say done reading data
    g_ready = TRUE;
}

/****************************************************************************
* SetDefaults - restore default settings
****************************************************************************/
void SetDefaults(UNIT_MODEL* unit)
{
    PICO_STATUS status;
    int32_t i;

    if (unit->ETS)
    {
        status = ps4000SetEts(unit->handle, PS4000_ETS_OFF, 0, 0, NULL); // Turn off ETS
        printf(status ? "SetDefaults: ps4000SetEts ------ %d \n" : "", status);
    }

    for (i = 0; i < unit->channelCount; i++) // reset channels to most recent settings
    {
        status = ps4000SetChannel(unit->handle,
            static_cast<PS4000_CHANNEL>(PS4000_CHANNEL_A + i),
            unit->channelSettings[PS4000_CHANNEL_A + i].enabled,
            unit->channelSettings[PS4000_CHANNEL_A + i].DCcoupled,
            static_cast<PS4000_RANGE>(unit->channelSettings[PS4000_CHANNEL_A + i].range));


        printf(status ? "SetDefaults: ps4000SetChannel(channel: %d)------ %d \n" : "", i, status);
    }
}

void SetParameters(unsigned int timebase_user, int oversample_user, bool scaleVoltages_user, int cycles_user, int32_t BUFFER_SIZE_user)
{
    timebase = timebase_user;
    oversample = oversample_user;
    scaleVoltages = scaleVoltages_user;
    cycles = cycles_user;
    BUFFER_SIZE = BUFFER_SIZE_user;
}

/****************************************************************************
* adc_to_mv
*
* Convert an 16-bit ADC count into millivolts
****************************************************************************/

int32_t adc_to_mv(int32_t raw, int32_t ch)
{
    return (raw * inputRanges[ch]) / PS4000_MAX_VALUE;
}

/****************************************************************************
* mv_to_adc
*
* Convert a millivolt value into a 16-bit ADC count
*
*  (useful for setting trigger thresholds)
****************************************************************************/
int16_t mv_to_adc(int16_t mv, int16_t ch)
{
    return ((mv * PS4000_MAX_VALUE) / inputRanges[ch]);
}

/****************************************************************************
* SetTrigger
*  this function calls the API trigger functions
****************************************************************************/
PICO_STATUS SetTrigger(int16_t handle,
    struct tTriggerChannelProperties* channelProperties,
    int16_t nChannelProperties,
    struct tTriggerConditions* triggerConditions,
    int16_t nTriggerConditions,
    TRIGGER_DIRECTIONS* directions,
    struct tPwq* pwq,
    uint32_t delay,
    int16_t auxOutputEnabled,
    int32_t autoTriggerMs)
{
    PICO_STATUS status;

    if ((status = ps4000SetTriggerChannelProperties(handle,
        channelProperties,
        nChannelProperties,
        auxOutputEnabled,
        autoTriggerMs)) != PICO_OK)
    {

        printf("SetTrigger:ps4000SetTriggerChannelProperties ------ %d \n", status);
        return status;
    }

    if ((status = ps4000SetTriggerChannelConditions(handle,
        triggerConditions,
        nTriggerConditions)) != PICO_OK)
    {
        printf("SetTrigger:ps4000SetTriggerChannelConditions ------ %d \n", status);
        return status;
    }

    if ((status = ps4000SetTriggerChannelDirections(handle,
        directions->channelA,
        directions->channelB,
        directions->channelC,
        directions->channelD,
        directions->ext,
        directions->aux)) != PICO_OK)
    {
        printf("SetTrigger:ps4000SetTriggerChannelDirections ------ %d \n", status);
        return status;
    }

    if ((status = ps4000SetTriggerDelay(handle, delay)) != PICO_OK)
    {
        printf("SetTrigger:ps4000SetTriggerDelay ------ %d \n", status);
        return status;
    }

    if ((status = ps4000SetPulseWidthQualifier(handle, pwq->conditions, pwq->nConditions,
        pwq->direction, pwq->lower, pwq->upper, pwq->type)) != PICO_OK)
    {
        printf("SetTrigger:ps4000SetPulseWidthQualifier ------ %d \n", status);
        return status;
    }

    return status;
}

/****************************************************************************
* Initialise unit' structure with Variant specific defaults
****************************************************************************/
void get_info(UNIT_MODEL* unit)
{
    int8_t description[11][25] = { "Driver Version",
                    "USB Version",
                    "Hardware Version",
                    "Variant Info",
                    "Serial",
                    "Cal Date",
                    "Kernel",
                    "Digital H/W",
                    "Analogue H/W",
                    "Firmware 1",
                    "Firmware 2" };

    int16_t i, r = 0;
    int8_t line[80];
    int32_t variant;
    PICO_STATUS status = PICO_OK;

    if (unit->handle)
    {
        for (i = 0; i < 11; i++)
        {
            status = ps4000GetUnitInfo(unit->handle, line, sizeof(line), &r, i);

            if (i == PICO_VARIANT_INFO)
            {
                variant = std::atoi(reinterpret_cast<char*>(line));
            }
            else if (i == PICO_ANALOGUE_HARDWARE_VERSION || PICO_DIGITAL_HARDWARE_VERSION)
            {
                // Ignore Analogue and Digital H/W
            }
            else
            {
                // Do nothing
            }

            printf("%s: %s\n", description[i], line);
        }



        switch (variant)
        {
        case MODEL_PS4223:
            unit->model = MODEL_PS4223;
            unit->signalGenerator = FALSE;
            unit->ETS = FALSE;
            unit->firstRange = PS4000_50MV;
            unit->lastRange = PS4000_50V;
            unit->channelCount = DUAL_SCOPE;
            break;

        case MODEL_PS4224:
            unit->model = MODEL_PS4224;
            unit->signalGenerator = FALSE;
            unit->ETS = FALSE;
            unit->firstRange = PS4000_50MV;
            unit->lastRange = PS4000_20V;
            unit->channelCount = DUAL_SCOPE;
            break;

        case MODEL_PS4423:
            unit->model = MODEL_PS4423;
            unit->signalGenerator = FALSE;
            unit->ETS = FALSE;
            unit->firstRange = PS4000_50MV;
            unit->lastRange = PS4000_50V;
            unit->channelCount = QUAD_SCOPE;
            break;

        case MODEL_PS4424:
            unit->model = MODEL_PS4424;
            unit->signalGenerator = FALSE;
            unit->ETS = FALSE;
            unit->firstRange = PS4000_50MV;
            unit->lastRange = PS4000_20V;
            unit->channelCount = QUAD_SCOPE;
            break;

        case MODEL_PS4226:
            unit->model = MODEL_PS4226;
            unit->signalGenerator = TRUE;
            unit->ETS = TRUE;
            unit->firstRange = PS4000_50MV;
            unit->lastRange = PS4000_20V;
            unit->channelCount = DUAL_SCOPE;
            break;

        case MODEL_PS4227:
            unit->model = MODEL_PS4227;
            unit->signalGenerator = TRUE;
            unit->ETS = TRUE;
            unit->firstRange = PS4000_50MV;
            unit->lastRange = PS4000_20V;
            unit->channelCount = DUAL_SCOPE;
            break;

        case MODEL_PS4262:
            unit->model = MODEL_PS4262;
            unit->signalGenerator = TRUE;
            unit->ETS = FALSE;
            unit->firstRange = PS4000_10MV;
            unit->lastRange = PS4000_20V;
            unit->channelCount = DUAL_SCOPE;
            break;

        default:
            break;
        }
    }
}

/****************************************************************************
* Select input voltage ranges for channels A and B
****************************************************************************/
void set_voltages(UNIT_MODEL* unit)
{
    int32_t i, ch;
    int32_t count = 0;

    /* See what ranges are available... */
    for (i = unit->firstRange; i <= unit->lastRange; i++)
    {
        printf("%d -> %d mV\n", i, inputRanges[i]);
    }

    do
    {
        /* Ask the user to select a range */
        printf("Specify voltage range (%d..%d)\n", unit->firstRange, unit->lastRange);
        printf("99 - switches channel off\n");
        for (ch = 0; ch < unit->channelCount; ch++)
        {
            printf("\n");
            do
            {
                printf("Channel %c: ", 'A' + ch);
                fflush(stdin);
                scanf_s("%hd", &unit->channelSettings[ch].range);
            } while (unit->channelSettings[ch].range != 99 && (unit->channelSettings[ch].range < unit->firstRange || unit->channelSettings[ch].range > unit->lastRange));

            if (unit->channelSettings[ch].range != 99)
            {
                printf(" - %d mV\n", inputRanges[unit->channelSettings[ch].range]);
                unit->channelSettings[ch].enabled = TRUE;
                count++;
            }
            else
            {
                printf("Channel Switched off\n");
                unit->channelSettings[ch].enabled = FALSE;
            }
        }
        printf(count == 0 ? "\n** At least 1 channel must be enabled **\n\n" : "");
    } while (count == 0);	// must have at least one channel enabled

    SetDefaults(unit);	// Put these changes into effect
}

/****************************************************************************
*
* Select timebase, set oversample to one
*
****************************************************************************/
void SetTimebase(UNIT_MODEL unit)
{
    int32_t timeInterval;
    int32_t maxSamples;

    printf("Specify desired timebase: ");
    fflush(stdin);
    scanf_s("%lud", &timebase);

    while (ps4000GetTimebase(unit.handle, timebase, BUFFER_SIZE, &timeInterval, 1, &maxSamples, 0))
    {
        timebase++;  // Increase timebase if the one specified can't be used. 
    }

    printf("Timebase used %lu = %ldns Sample Interval\n", timebase, timeInterval);
    oversample = TRUE;
}