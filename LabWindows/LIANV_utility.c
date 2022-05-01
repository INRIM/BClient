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

//==============================================================================
// Include files

#include <utility.h>
#include <ansi_c.h>
#include "LIA_NV_cvi_driver.h"
#include "LIANV_utility.h"

//==============================================================================
// Constants


//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

const double LIANV_SensitivityValues[] = {
	[LIANV_SENSITIVITY_2NV] = 2e-9,
	[LIANV_SENSITIVITY_5NV] = 5e-9,
	[LIANV_SENSITIVITY_10NV] = 10e-9,
	[LIANV_SENSITIVITY_20NV] = 20e-9,
	[LIANV_SENSITIVITY_50NV] = 50e-9,
	[LIANV_SENSITIVITY_100NV] = 100e-9,
	[LIANV_SENSITIVITY_200NV] = 200e-9,
	[LIANV_SENSITIVITY_500NV] = 500e-9,
	[LIANV_SENSITIVITY_1UV] = 1e-6,
	[LIANV_SENSITIVITY_2UV] = 2e-6,
	[LIANV_SENSITIVITY_5UV] = 5e-6,
	[LIANV_SENSITIVITY_10UV] = 10e-6,
	[LIANV_SENSITIVITY_20UV] = 20e-6,
	[LIANV_SENSITIVITY_50UV] = 50e-6,
	[LIANV_SENSITIVITY_100UV] = 100e-6,
	[LIANV_SENSITIVITY_200UV] = 200e-6,
	[LIANV_SENSITIVITY_500UV] = 500e-6,
	[LIANV_SENSITIVITY_1MV] = 1e-3,
	[LIANV_SENSITIVITY_2MV] = 2e-3,
	[LIANV_SENSITIVITY_5MV] = 5e-3,
	[LIANV_SENSITIVITY_10MV] = 10e-3,
	[LIANV_SENSITIVITY_20MV] = 20e-3,
	[LIANV_SENSITIVITY_50MV] = 50e-3,
	[LIANV_SENSITIVITY_100MV] = 100e-3,
	[LIANV_SENSITIVITY_200MV] = 200e-3,
	[LIANV_SENSITIVITY_500MV] = 500e-3,
	[LIANV_SENSITIVITY_1V] = 1.0
};

const double LIANV_SensitivityScaleValues[] = {
	[LIANV_INPUT_VOLTAGE_SINGLE_ENDED] = 1.0,
	[LIANV_INPUT_VOLTAGE_DIFFERENTIAL] = 1.0,
	[LIANV_INPUT_CURRENT_1MOHM] = 1e-6,
	[LIANV_INPUT_CURRENT_100MOHM] = 1e-8
};

const double LIANV_TimeConstantValues[] = {
	[LIANV_TIME_CONSTANT_10US] = 10e-6,
	[LIANV_TIME_CONSTANT_30US] = 30e-6,
	[LIANV_TIME_CONSTANT_100US] = 100e-6,
	[LIANV_TIME_CONSTANT_300US] = 300e-6,
	[LIANV_TIME_CONSTANT_1MS] = 1e-3,
	[LIANV_TIME_CONSTANT_3MS] = 3e-3,
	[LIANV_TIME_CONSTANT_10MS] = 10e-3,
	[LIANV_TIME_CONSTANT_30MS] = 30e-3,
	[LIANV_TIME_CONSTANT_100MS] = 100e-3,
	[LIANV_TIME_CONSTANT_300MS] = 300e-3,
	[LIANV_TIME_CONSTANT_1S] = 1.0,
	[LIANV_TIME_CONSTANT_3S] = 3.0,
	[LIANV_TIME_CONSTANT_10S] = 10.0,
	[LIANV_TIME_CONSTANT_30S] = 30.0,
	[LIANV_TIME_CONSTANT_100S] = 100.0,
	[LIANV_TIME_CONSTANT_300S] = 300.0,
	[LIANV_TIME_CONSTANT_1KS] = 1e3,
	[LIANV_TIME_CONSTANT_3KS] = 3e3,
	[LIANV_TIME_CONSTANT_10KS] = 10e3,
	[LIANV_TIME_CONSTANT_30KS] = 30e3
};

const double LIANV_SettlingTimeMultipliers[] = {
	[LIANV_SLOPE_6] = 5.0,
	[LIANV_SLOPE_12] = 7.0,
	[LIANV_SLOPE_18] = 9.0,
	[LIANV_SLOPE_24] = 10.0
};

//==============================================================================
// Global functions

int LIANV_SetInputParameters(const char *nvProcess, LIANV_InputSettings inputSettings)
{
	int ret = 0;
	
	if ((ret = LIANV_SetSensitivity(nvProcess, inputSettings.sensitivity)) < 0 ||
			(ret = LIANV_SetTimeConstant(nvProcess, inputSettings.timeConstant)) < 0 ||
			(ret = LIANV_SetLowPassFilterSlope(nvProcess, inputSettings.filterSlope)) < 0 ||
			(ret = LIANV_SetInputConfiguration(nvProcess, inputSettings.inputConfiguration)) < 0 ||
			(ret = LIANV_SetInputCoupling(nvProcess, inputSettings.inputCoupling)) < 0 ||
			(ret = LIANV_SetInputShieldGrounding(nvProcess, inputSettings.inputShieldGrounding)) < 0 ||
			(ret = LIANV_SetReserveMode(nvProcess, inputSettings.inputReserve)) < 0 ||
			(ret = LIANV_SetInputLineNotchFilter(nvProcess, inputSettings.inputLineNotchFilter)) < 0) {
				return ret;
	}
	return 0;
}

int LIANV_SetInputParametersNoSensitivity(const char *nvProcess, LIANV_InputSettings inputSettings)
{
	int ret = 0;
	
	if ((ret = LIANV_SetTimeConstant(nvProcess, inputSettings.timeConstant)) < 0 ||
			(ret = LIANV_SetLowPassFilterSlope(nvProcess, inputSettings.filterSlope)) < 0 ||
			(ret = LIANV_SetInputConfiguration(nvProcess, inputSettings.inputConfiguration)) < 0 ||
			(ret = LIANV_SetInputCoupling(nvProcess, inputSettings.inputCoupling)) < 0 ||
			(ret = LIANV_SetInputShieldGrounding(nvProcess, inputSettings.inputShieldGrounding)) < 0 ||
			(ret = LIANV_SetReserveMode(nvProcess, inputSettings.inputReserve)) < 0 ||
			(ret = LIANV_SetInputLineNotchFilter(nvProcess, inputSettings.inputLineNotchFilter)) < 0) {
				return ret;
	}
	return 0;
}

int LIANV_GetInputParameters(const char *nvProcess, LIANV_InputSettings *inputSettings)
{
	int ret = 0;
	
	if ((ret = LIANV_GetSensitivity(nvProcess, &inputSettings->sensitivity)) < 0 ||
			(ret = LIANV_GetTimeConstant(nvProcess, &inputSettings->timeConstant)) < 0 ||
			(ret = LIANV_GetLowPassFilterSlope(nvProcess, &inputSettings->filterSlope)) < 0 ||
			(ret = LIANV_GetInputConfiguration(nvProcess, &inputSettings->inputConfiguration)) < 0 ||
			(ret = LIANV_GetInputCoupling(nvProcess, &inputSettings->inputCoupling)) < 0 ||
			(ret = LIANV_GetInputShieldGrounding(nvProcess, &inputSettings->inputShieldGrounding)) < 0 ||
			(ret = LIANV_GetReserveMode(nvProcess, &inputSettings->inputReserve)) < 0 ||
			(ret = LIANV_GetInputLineNotchFilter(nvProcess, &inputSettings->inputLineNotchFilter)) < 0) {
				return ret;
	}
	return 0;
}

int LIANV_GetMeasureXYWithAutoGain(const char *nvProcess, double lockinReading[])
{
	do {
		int ret = 0, sensitivity = 0, timeConstant = 0, inputConfiguration = 0, filterSlope = 0;
		unsigned int liaStatus = 0;
		
		if((ret = LIANV_GetSensitivity(nvProcess, &sensitivity)) < 0 ||
				(ret = LIANV_GetInputConfiguration(nvProcess, &inputConfiguration)) < 0 ||
				(ret = LIANV_GetLowPassFilterSlope(nvProcess, &filterSlope)) < 0 ||
				(ret = LIANV_GetTimeConstant(nvProcess, &timeConstant)) < 0 ||
				(ret = LIANV_GetDeviceStatus(nvProcess, "LIAS", &liaStatus)) < 0) {
			return ret;
		}
		if (liaStatus & (LIANV_LIA_STATUS_INPUT | LIANV_LIA_STATUS_FILTER | LIANV_LIA_STATUS_OUTPUT)) {
			if (sensitivity < LIANV_SENSITIVITY_1V) {
				++sensitivity;
				if ((ret = LIANV_SetSensitivity(nvProcess, sensitivity)) < 0) {
					return ret;
				}
				Delay(LIANV_SettlingTimeMultipliers[filterSlope]*LIANV_TimeConstantValues[timeConstant]);
				continue;
			} else {
				return 0;
			}
		} else {
			LIANV_GetMeasureXY(nvProcess, lockinReading);
			if (sensitivity > LIANV_SENSITIVITY_2NV && 
					LIANV_AUTOGAIN_HEADROOM_COEFFICIENT*fabs(lockinReading[0]) < LIANV_SensitivityValues[sensitivity-1]*LIANV_SensitivityScaleValues[inputConfiguration] && 
					LIANV_AUTOGAIN_HEADROOM_COEFFICIENT*fabs(lockinReading[1]) < LIANV_SensitivityValues[sensitivity-1]*LIANV_SensitivityScaleValues[inputConfiguration]) {
				--sensitivity;
				if ((ret = LIANV_SetSensitivity(nvProcess, sensitivity)) < 0) {
					return ret;
				}
				Delay(LIANV_SettlingTimeMultipliers[filterSlope]*LIANV_TimeConstantValues[timeConstant]);
				continue;
			} else {
				return 0;
			}
		}
	} while (1);
}

