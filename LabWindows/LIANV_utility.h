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

#ifndef LIANV_UTILITY_H
#define LIANV_UTILITY_H

#ifdef __cplusplus
	extern "C" {
#endif

//==============================================================================
// Constants

#define LIANV_NAME_SZ 100
#define LIANV_AUTOGAIN_HEADROOM_COEFFICIENT 6.0
#define LIANV_MIN_SAMPLING_PERIOD 0.1
#define LIANV_MAX_SAMPLING_PERIOD 3600.0

//==============================================================================
// Types
		
typedef enum {
	LIANV_SENSITIVITY_2NV,
	LIANV_SENSITIVITY_5NV,
	LIANV_SENSITIVITY_10NV,
	LIANV_SENSITIVITY_20NV,
	LIANV_SENSITIVITY_50NV,
	LIANV_SENSITIVITY_100NV,
	LIANV_SENSITIVITY_200NV,
	LIANV_SENSITIVITY_500NV,
	LIANV_SENSITIVITY_1UV,
	LIANV_SENSITIVITY_2UV,
	LIANV_SENSITIVITY_5UV,
	LIANV_SENSITIVITY_10UV,
	LIANV_SENSITIVITY_20UV,
	LIANV_SENSITIVITY_50UV,
	LIANV_SENSITIVITY_100UV,
	LIANV_SENSITIVITY_200UV,
	LIANV_SENSITIVITY_500UV,
	LIANV_SENSITIVITY_1MV,
	LIANV_SENSITIVITY_2MV,
	LIANV_SENSITIVITY_5MV,
	LIANV_SENSITIVITY_10MV,
	LIANV_SENSITIVITY_20MV,
	LIANV_SENSITIVITY_50MV,
	LIANV_SENSITIVITY_100MV,
	LIANV_SENSITIVITY_200MV,
	LIANV_SENSITIVITY_500MV,
	LIANV_SENSITIVITY_1V
} LIANV_Sensitivity;

typedef enum {
	LIANV_TIME_CONSTANT_10US,
	LIANV_TIME_CONSTANT_30US,
	LIANV_TIME_CONSTANT_100US,
	LIANV_TIME_CONSTANT_300US,
	LIANV_TIME_CONSTANT_1MS,
	LIANV_TIME_CONSTANT_3MS,
	LIANV_TIME_CONSTANT_10MS,
	LIANV_TIME_CONSTANT_30MS,
	LIANV_TIME_CONSTANT_100MS,
	LIANV_TIME_CONSTANT_300MS,
	LIANV_TIME_CONSTANT_1S,
	LIANV_TIME_CONSTANT_3S,
	LIANV_TIME_CONSTANT_10S,
	LIANV_TIME_CONSTANT_30S,
	LIANV_TIME_CONSTANT_100S,
	LIANV_TIME_CONSTANT_300S,
	LIANV_TIME_CONSTANT_1KS,
	LIANV_TIME_CONSTANT_3KS,
	LIANV_TIME_CONSTANT_10KS,
	LIANV_TIME_CONSTANT_30KS
} LIANV_TimeConstant;

typedef enum {
	LIANV_SLOPE_6,
	LIANV_SLOPE_12,
	LIANV_SLOPE_18,
	LIANV_SLOPE_24
} LIANV_FilterSlope;
		
typedef enum {
	LIANV_INPUT_RESERVE_HIGH,
	LIANV_INPUT_RESERVE_NORMAL,
	LIANV_INPUT_RESERVE_LOW_NOISE
} LIANV_InputReserve;

typedef enum {
	LIANV_INPUT_VOLTAGE_SINGLE_ENDED,
	LIANV_INPUT_VOLTAGE_DIFFERENTIAL,
	LIANV_INPUT_CURRENT_1MOHM,
	LIANV_INPUT_CURRENT_100MOHM
} LIANV_InputConfiguration;

typedef enum {
	LIANV_INPUT_COUPLING_AC,
    LIANV_INPUT_COUPLING_DC
} LIANV_InputCoupling;

typedef enum {
	LIANV_INPUT_FLOAT, 
	LIANV_INPUT_GROUND
} LIANV_InputShieldGrounding;

typedef enum {
	LIANV_INPUT_LINE_NOTCH_FILTER_OFF,
	LIANV_INPUT_LINE_NOTCH_FILTER_LINE,
	LIANV_INPUT_LINE_NOTCH_FILTER_2X,
	LIANV_INPUT_LINE_NOTCH_FILTER_BOTH
} LIANV_InputLineNotchFilter;

typedef enum {
	LIANV_LIA_STATUS_INPUT = 1,
	LIANV_LIA_STATUS_FILTER = 2,
	LIANV_LIA_STATUS_OUTPUT = 4,
	LIANV_LIA_STATUS_UNLOCK = 8,
	LIANV_LIA_STATUS_RANGE = 16,
	LIANV_LIA_STATUS_TIME_CONSTANT = 32,
	LIANV_LIA_STATUS_TRIGGER = 64
} LIANV_LIAStatusBits;

typedef struct {
	LIANV_Sensitivity sensitivity;
	LIANV_TimeConstant timeConstant;
	LIANV_FilterSlope filterSlope;
	LIANV_InputConfiguration inputConfiguration;
	LIANV_InputCoupling inputCoupling;
	LIANV_InputShieldGrounding inputShieldGrounding;
	LIANV_InputReserve inputReserve;
	LIANV_InputLineNotchFilter inputLineNotchFilter;
} LIANV_InputSettings;

//==============================================================================
// Global functions

int LIANV_SetInputParameters(const char *, LIANV_InputSettings);
int LIANV_SetInputParametersNoSensitivity(const char *, LIANV_InputSettings);
int LIANV_GetInputParameters(const char *, LIANV_InputSettings *);
int LIANV_GetMeasureXYWithAutoGain(const char *, double []);

//==============================================================================
// Global variables

extern const double LIANV_SensitivityValues[];
extern const double LIANV_SensitivityScaleValues[];
extern const double LIANV_TimeConstantValues[];
extern const double LIANV_SettlingTimeMultipliers[];

#ifdef __cplusplus
	}
#endif

#endif /* DADSS_UTILITY_H */
