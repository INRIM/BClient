//==============================================================================
//
// VersICaL impedance bridge client
//
// Copyright 2018-2022	Massimo Ortolano <massimo.ortolano@polito.it> 
//                		Martina Marzano <m.marzano@inrim.it>
//
// This code is licensed under MIT license (see LICENSE.txt for details)
//
//==============================================================================

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
	extern "C" {
#endif

//==============================================================================
// Include files

#include <stdio.h>
#include "toolbox.h"
		
#include "DADSS_utility.h"
#include "LIANV_utility.h"
#include "DIO_utility.h"
		
//==============================================================================
// Constants
		
#define TITLE_BUF_SZ 120
		
#define PI 3.1415926535897932

#define BUF_SZ 1024
#define CLIPBOARD_BUF_SZ 393216
#define LABEL_SZ 32
		
#define MAX_AUTOZERO_STEPS 100
#define AUTOZERO_ADJ_DELAY_BASE 0.0 
#define AUTOZERO_ADJ_DELAY_FACTOR 10.0
#define AUTOZERO_GRAPH_RANGE_FACTOR 40.0
#define MAX_FULL_BALANCE_STEPS 50
#define CHECK_BALANCE_DELAY_BASE 5.0 
		
#define MAX_MODES 21
#define MAX_CHANNEL_SWITCH_MODES 21

//==============================================================================
// Types
		
typedef enum {
	STATE_IDLE,
	STATE_CONNECTED,
	STATE_RUNNING,
	STATE_BALANCING,
	STATE_UPDATING,
	STATE_RECORDING
} ProgramState;

typedef enum {
	LIANV_GAIN_MANUAL, 
	LIANV_GAIN_AUTO
} LIANV_GainMode;

typedef enum {
	RESISTANCE,
	CAPACITANCE,
	INDUCTANCE,
	IMPEDANCE,
	ADMITTANCE
} StandardType;

typedef enum {
	LIANV_GAIN = 1,
	LIANV_SENSITIVITY,
	LIANV_TIME_CONSTANT,
	LIANV_FILTER_SLOPE,
	LIANV_INPUT_CONFIGURATION,
	LIANV_INPUT_COUPLING,
	LIANV_INPUT_SHIELD_GROUNDING,
	LIANV_INPUT_RESERVE,
	LIANV_INPUT_LINE_NOTCH_FILTER
} LIANV_SettingsList;

typedef enum {
	VOLTAGE_CHANNEL_A,
	VOLTAGE_CHANNEL_B,
	CURRENT_CHANNEL_A,
	CURRENT_CHANNEL_B,
	KELVIN_ARM_CHANNEL,
	INJECTION_CHANNEL,
	CHANNEL_COUNT
} BridgeChannels;

typedef enum {
	BALANCE_TWO_PORT,
	BALANCE_VERSICAL,
	BALANCE_VERSICAL_NO_KELVIN_ARM,
	BALANCE_GIQS,
	BALANCE_GIQS_NO_KELVIN_ARM
} BalanceProcedure;

typedef enum {
	BALANCE_CHANNEL,
	BALANCE_MODE,
	BALANCE_DOUBLE_MODE,
	BALANCE_FULL_AB,
	BALANCE_FULL_ABBA
} BalanceTarget;

typedef struct {
	StandardType standardType;
	union {
		double r;
		double c;
		double l;
		double g;
	} primary;
	union {
		double tau;
		double d;
		double r;
		double x;
		double b;
	} secondary;
} Standard;

typedef struct {
	int channelAssignment[CHANNEL_COUNT];
	double seriesResistance[CHANNEL_COUNT];
} BridgeSettings;

typedef struct {
	char nvServerName[DADSS_NAME_SZ];
	char nvProcessName[DADSS_NAME_SZ];
	double clockFrequency;
	double frequency;
	double realFrequency;
	int nSamples;
	DADSS_RangeList range[DADSS_CHANNELS];
	int nModes;
	int activeMode;
	int nChannelSwitchModes;
	int activeChannel;
	char label[DADSS_CHANNELS][LABEL_SZ]; 
	char dataPathName[MAX_PATHNAME_LEN];
	FILE *dataFileHandle;
	int isFileModified;
	BalanceProcedure balanceProcedureIdx;
	ListType balanceProcedure;
	BalanceTarget balanceTarget;
} SourceSettings;

typedef struct {
	int isPresent;
	char nvProcessName[LIANV_NAME_SZ];
	int isBalanceInterrupted;
} LockinSettings;

typedef struct {
	int isLocked;
	double amplitude;
	double phase;
	double real;
	double imag;
	unsigned int mdac2Code;
	int mdac1Offset;
	double mdac2Val;
	LIANV_GainMode gainMode;
	LIANV_InputSettings lockinInputSettings;
	double lockinReading[2];
	double balanceThreshold;
	double checkThreshold;
	int activeSwitchMode;
} ChannelSettings;

typedef struct {
	int isPresent;
	char nvProcessName[LIANV_NAME_SZ];
} SwitchSettings;

typedef struct {
	char label[LABEL_SZ];
	ChannelSettings channelSettings[DADSS_CHANNELS];
	BridgeSettings bridgeSettings;
	unsigned char switchPorts[DIO_PORTS];
} ModeSettings;

typedef struct {
	char label[LABEL_SZ];
	unsigned char switchPorts[DIO_PORTS];
} ChannelSwitchSettings;

typedef struct {
	double samplingPeriod;
	int nSamples;
	int sampleCount;
	double startTime;
	char dataPathName[MAX_PATHNAME_LEN];
	FILE *dataFileHandle;
} RecordSettings;

//==============================================================================
// External variables

extern const char panelsFile[];
extern ProgramState programState;

//==============================================================================
// Global functions

#ifdef __cplusplus
	}
#endif

#endif /* MAIN_H */
