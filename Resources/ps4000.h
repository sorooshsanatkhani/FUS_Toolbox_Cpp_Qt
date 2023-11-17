#ifndef PS4000_H
#define PS4000_H

/* Headers for Windows */
#include "windows.h"
#include <conio.h>
#include <stdio.h>
#include <cstdlib>


/* Definitions of ps4000 driver routines on Windows*/
#include "ps4000Api.h"

#define PREF4 __stdcall

#define MAX_CHANNELS	4
#define DUAL_SCOPE		2
#define TRIPLE_SCOPE	3
#define QUAD_SCOPE		4

// Signal generator
#define	AWG_DAC_FREQUENCY_4000	20e6f			// 20 MS/s update rate
#define	AWG_DAC_FREQUENCY_4262	500000.0f		// 500 kS/s update rate
#define	AWG_PHASE_ACCUMULATOR	4294967296.0f

typedef struct
{
    int16_t DCcoupled;
    int16_t range;
    int16_t enabled;
} CHANNEL_SETTINGS;

typedef enum
{
    MODEL_NONE = 0,
    MODEL_PS4223 = 4223,
    MODEL_PS4224 = 4224,
    MODEL_PS4423 = 4423,
    MODEL_PS4424 = 4424,
    MODEL_PS4226 = 4226,
    MODEL_PS4227 = 4227,
    MODEL_PS4262 = 4262
} MODEL_TYPE;

typedef struct tTriggerDirections
{
    enum enThresholdDirection channelA;
    enum enThresholdDirection channelB;
    enum enThresholdDirection channelC;
    enum enThresholdDirection channelD;
    enum enThresholdDirection ext;
    enum enThresholdDirection aux;
} TRIGGER_DIRECTIONS;

typedef struct tPwq
{
    struct tPwqConditions* conditions;
    int16_t nConditions;
    enum enThresholdDirection direction;
    uint32_t lower;
    uint32_t upper;
    enum enPulseWidthType type;
} PWQ;

typedef struct
{
    int16_t					handle;
    MODEL_TYPE				model;
    PS4000_RANGE			firstRange;
    PS4000_RANGE			lastRange;
    uint16_t				signalGenerator;
    uint16_t 				ETS;
    int16_t					channelCount;
    CHANNEL_SETTINGS		channelSettings[MAX_CHANNELS];
    PS4000_RANGE			triggerRange;
} UNIT_MODEL;


extern int32_t cycles;
extern uint32_t	timebase;
extern int32_t BUFFER_SIZE;
extern int16_t SEGMEM;
extern int16_t	oversample;
extern BOOL	scaleVoltages;
extern uint16_t inputRanges[PS4000_MAX_RANGES];
extern int16_t g_ready;
extern int64_t g_times[PS4000_MAX_CHANNELS];
extern int16_t g_timeUnit;
extern int32_t g_sampleCount;
extern uint32_t g_startIndex;
extern int16_t g_autoStop;
extern int16_t g_trig;
extern uint32_t g_trigAt;


typedef struct tBufferInfo
{
	UNIT_MODEL* unit;
	int16_t** driverBuffers;
	int16_t** appBuffers;
} BUFFER_INFO;

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
);

void PREF4 CallBackBlock
(
    int16_t handle,
    PICO_STATUS status,
    void* pParameter
);

void SetDefaults(UNIT_MODEL* unit);

void SetParameters(unsigned int timebase_user, int oversample_user, bool scaleVoltages_user, int cycles_user, int32_t BUFFER_SIZE_user);

int32_t adc_to_mv(int32_t raw, int32_t ch);

int16_t mv_to_adc(int16_t mv, int16_t ch);

PICO_STATUS SetTrigger(int16_t handle,
	struct tTriggerChannelProperties* channelProperties,
	int16_t nChannelProperties,
	struct tTriggerConditions* triggerConditions,
	int16_t nTriggerConditions,
	TRIGGER_DIRECTIONS* directions,
	struct tPwq* pwq,
	uint32_t delay,
	int16_t auxOutputEnabled,
	int32_t autoTriggerMs);

void get_info(UNIT_MODEL* unit);

void set_voltages(UNIT_MODEL* unit);

void SetTimebase(UNIT_MODEL unit);

#endif // PS4000_H