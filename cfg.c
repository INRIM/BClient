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

#include <advanlys.h>

#include "main.h"
#include "msg.h"
#include "cfg.h"
#include "balance.h"
#include "DA_DSS_cvi_driver.h"
#include "LIA_NV_cvi_driver.h"
#include "DIO_cvi_driver.h"
#include "DADSS_utility.h"
#include "LIANV_utility.h"
#include "DIO_utility.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

static int LoadSourceSettings(SettingsFileDescriptor, SourceSettings *);
static int ValidateSourceSettings(SettingsFileDescriptor, SourceSettings);
static int LoadModeSettings(SettingsFileDescriptor, SourceSettings, ModeSettings *);
static int ValidateModeSettings(SettingsFileDescriptor, SourceSettings, ModeSettings *);
static int LoadRecordSettings(SettingsFileDescriptor, RecordSettings *);
static int ValidateRecordSettings(SettingsFileDescriptor, RecordSettings);
static int LoadLockinSettings(SettingsFileDescriptor, LockinSettings *);
static int LoadSwitchSettings(SettingsFileDescriptor, SwitchSettings *);
static int LoadChannelSwitchSettings(SettingsFileDescriptor, SourceSettings, ChannelSwitchSettings *);
static int SaveSourceSettings(SettingsFileDescriptor);
static int SaveModeSettings(SettingsFileDescriptor);
static int SaveRecordSettings(SettingsFileDescriptor);
static int SaveLockinSettings(SettingsFileDescriptor);
static int SaveSwitchSettings(SettingsFileDescriptor);
static int SaveChannelSwitchSettings(SettingsFileDescriptor);

//==============================================================================
// Global variables

SourceSettings sourceSettings = {.dataPathName = "", .dataFileHandle = NULL, .isFileModified = 0 };
ModeSettings modeSettings[MAX_MODES];
LockinSettings lockinSettings;
SwitchSettings switchSettings;
ChannelSwitchSettings channelSwitchSettings[MAX_CHANNEL_SWITCH_MODES];
RecordSettings recordSettings;

const char defaultSettingsFileName[] = "bclient.ini";
char defaultSettingsFile[MAX_PATHNAME_LEN]; 
char defaultSettingsFileDir[MAX_PATHNAME_LEN];

//==============================================================================
// Global functions

void SetDefaultSettings(void) 
{
	strncpy(sourceSettings.nvServerName, "localhost", DADSS_NAME_SZ);
	strncpy(sourceSettings.nvProcessName, "DSS2A_1", DADSS_NAME_SZ);
	sourceSettings.frequency = 1000.0;
	sourceSettings.clockFrequency = 20.0;
	sourceSettings.realFrequency = sourceSettings.frequency;
	sourceSettings.nSamples = (int)(sourceSettings.clockFrequency*DADSS_CLOCKFREQUENCY_MUL/sourceSettings.frequency);
	for (int i = 0; i < DADSS_CHANNELS; ++i) {
		sourceSettings.range[i] = DADSS_RANGE_2V5;
		snprintf(sourceSettings.label[i], LABEL_SZ, "E%d", i+1);
	}
	sourceSettings.nModes = 2; // Dummy mode 0 is always present
	sourceSettings.activeMode = 1;
	sourceSettings.activeChannel = 0;
	sourceSettings.nChannelSwitchModes = 1; 
	sourceSettings.isFileModified = 1;
	sourceSettings.balanceProcedureIdx = BALANCE_VERSICAL;
	sourceSettings.balanceProcedure = balanceVersical;
	sourceSettings.balanceTarget = BALANCE_CHANNEL;
	
	lockinSettings.isPresent = 1;
	strncpy(lockinSettings.nvProcessName, "LIA-SR830_1", LIANV_NAME_SZ);
	
	switchSettings.isPresent = 1;
	strncpy(switchSettings.nvProcessName, "DIO-PXI-NI6509", DIO_NAME_SZ);
		
	for (int i = 1; i < MAX_MODES; ++i) {
		SetDefaultModeSettings(i);
	}
	modeSettings[0] = modeSettings[sourceSettings.activeMode];
	for (int i = 0; i < MAX_CHANNEL_SWITCH_MODES; ++i) {
		SetDefaultChannelSwitchSettings(i);
	}
	
	strncpy(recordSettings.dataPathName, "lockin_record", MAX_PATHNAME_LEN);
	recordSettings.dataFileHandle = NULL;
	recordSettings.nSamples = 100;
	recordSettings.sampleCount = 0;
	recordSettings.samplingPeriod = 1;
	recordSettings.startTime = 0;
	
	DADSS_SetNameNVServer(sourceSettings.nvServerName);
	DADSS_SetNameNVProcess(sourceSettings.nvProcessName);
	LIANV_SetNameNVServer(sourceSettings.nvServerName);
	DIO_SetNameNVServer(sourceSettings.nvServerName);
	DIO_SetNameNVProcess(switchSettings.nvProcessName);
}

void SetDefaultModeSettings(int mode)
{
	snprintf(modeSettings[mode].label, LABEL_SZ, "Mode %d", mode);
	for (int i = 0; i < DADSS_CHANNELS; ++i) {
		modeSettings[mode].channelSettings[i].isLocked = 0;
		modeSettings[mode].channelSettings[i].amplitude = 0.0;
		modeSettings[mode].channelSettings[i].phase = 0.0;
		modeSettings[mode].channelSettings[i].real = 0.0;
		modeSettings[mode].channelSettings[i].imag = 0.0;
		modeSettings[mode].channelSettings[i].mdac1Offset = 0.0;
		modeSettings[mode].channelSettings[i].mdac2Code = DADSS_MDAC2_CODE_MAX;
		DADSS_Mdac2CodeToValue(modeSettings[mode].channelSettings[i].mdac2Code, &modeSettings[mode].channelSettings[i].mdac2Val);
		modeSettings[mode].channelSettings[i].gainMode = LIANV_GAIN_AUTO;
		modeSettings[mode].channelSettings[i].lockinInputSettings.sensitivity = LIANV_SENSITIVITY_1V;
		modeSettings[mode].channelSettings[i].lockinInputSettings.timeConstant = LIANV_TIME_CONSTANT_300MS;
		modeSettings[mode].channelSettings[i].lockinInputSettings.filterSlope = LIANV_SLOPE_24;
		modeSettings[mode].channelSettings[i].lockinInputSettings.inputConfiguration = LIANV_INPUT_VOLTAGE_SINGLE_ENDED;
		modeSettings[mode].channelSettings[i].lockinInputSettings.inputCoupling = LIANV_INPUT_COUPLING_AC;
		modeSettings[mode].channelSettings[i].lockinInputSettings.inputShieldGrounding = LIANV_INPUT_FLOAT;
		modeSettings[mode].channelSettings[i].lockinInputSettings.inputReserve = LIANV_INPUT_RESERVE_NORMAL; 
		modeSettings[mode].channelSettings[i].lockinInputSettings.inputLineNotchFilter = LIANV_INPUT_LINE_NOTCH_FILTER_BOTH;  
		modeSettings[mode].channelSettings[i].lockinReading[0] = 0.0;
		modeSettings[mode].channelSettings[i].lockinReading[1] = 0.0;
		modeSettings[mode].channelSettings[i].balanceThreshold = 1e-5;
		modeSettings[mode].channelSettings[i].checkThreshold = 1e-5;
		modeSettings[mode].channelSettings[i].activeSwitchMode = 0;
	}
	modeSettings[mode].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A] = 0;
	modeSettings[mode].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A] = 1;
	modeSettings[mode].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B] = 3;
	modeSettings[mode].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B] = 2;
	modeSettings[mode].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL] = 4;
	modeSettings[mode].bridgeSettings.channelAssignment[INJECTION_CHANNEL] = 5;
	modeSettings[mode].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_A] = 0.0;
	modeSettings[mode].bridgeSettings.seriesResistance[CURRENT_CHANNEL_A] = 0.0;
	modeSettings[mode].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_B] = 0.0;
	modeSettings[mode].bridgeSettings.seriesResistance[CURRENT_CHANNEL_B] = 0.0;
	modeSettings[mode].bridgeSettings.seriesResistance[KELVIN_ARM_CHANNEL] = 0.0;
	modeSettings[mode].bridgeSettings.seriesResistance[INJECTION_CHANNEL] = 0.0;
	
	for (int i = 0; i < DIO_PORTS; ++i) {
		modeSettings[mode].switchPorts[i] = 0;
	}
}

void SetDefaultChannelSwitchSettings(int mode)
{
	snprintf(channelSwitchSettings[mode].label, LABEL_SZ, "Channel_switch_%d", mode+1);
	for (int i = 0; i < DIO_PORTS; ++i) {
		channelSwitchSettings[mode].switchPorts[i] = 0;
	}
}

void LoadSettings(const char *fileName)
{
	// Initialize temporary settings variables
	SourceSettings sourceSettingsTmp = {.dataFileHandle = sourceSettings.dataFileHandle, .isFileModified = 1, .balanceTarget = BALANCE_CHANNEL};
	strncpy(sourceSettingsTmp.dataPathName, sourceSettings.dataPathName, MAX_PATHNAME_LEN);
	LockinSettings lockinSettingsTmp = {.isBalanceInterrupted = 0};
	SwitchSettings switchSettingsTmp;
	ChannelSwitchSettings channelSwitchSettingsTmp[MAX_CHANNEL_SWITCH_MODES];
	
	ModeSettings modeSettingsTmp[MAX_MODES];
	for (int i = 0; i < MAX_MODES; ++i) {
		for (int j = 0; j < DADSS_CHANNELS; ++j) {
			modeSettingsTmp[i].channelSettings[j].lockinReading[0] = 0.0;
			modeSettingsTmp[i].channelSettings[j].lockinReading[1] = 0.0;
		}
	}
	
	RecordSettings recordSettingsTmp = {.sampleCount = 0, .dataFileHandle = NULL, .startTime = 0};
	strncpy(recordSettingsTmp.dataPathName, recordSettings.dataPathName, MAX_PATHNAME_LEN);
	
	// Read and check settings file
	SettingsFileDescriptor settingsFileDescriptor;
	settingsFileDescriptor.fileName = fileName;
	settingsFileDescriptor.iniText = Ini_New(TRUE); 
	if (settingsFileDescriptor.iniText == 0) {
		warn("%s %s\n%s", msgStrings[MSG_LOADING_ERROR], fileName, msgStrings[MSG_OUT_OF_MEMORY]);
		return;
	}
	
	if (LoadSourceSettings(settingsFileDescriptor, &sourceSettingsTmp) <= 0 || ValidateSourceSettings(settingsFileDescriptor, sourceSettingsTmp) < 0 ||
			LoadModeSettings(settingsFileDescriptor, sourceSettingsTmp, modeSettingsTmp) <= 0 || ValidateModeSettings(settingsFileDescriptor, sourceSettingsTmp, modeSettingsTmp) < 0 ||
			LoadRecordSettings(settingsFileDescriptor, &recordSettingsTmp) <= 0 || ValidateRecordSettings(settingsFileDescriptor, recordSettingsTmp) < 0 ||
			LoadLockinSettings(settingsFileDescriptor, &lockinSettingsTmp) <= 0 || 
			LoadSwitchSettings(settingsFileDescriptor, &switchSettingsTmp) <= 0 ||
	   		LoadChannelSwitchSettings(settingsFileDescriptor, sourceSettingsTmp, channelSwitchSettingsTmp) <= 0) {
		Ini_Dispose(settingsFileDescriptor.iniText);
		return;
	}
	Ini_Dispose(settingsFileDescriptor.iniText);
	
	// Settings verified, copy to global variables
	sourceSettings = sourceSettingsTmp;
	switch (sourceSettings.balanceProcedureIdx)
	{
		case BALANCE_TWO_PORT:
			sourceSettings.balanceProcedure = balanceTwoPort;
			break;
		case BALANCE_VERSICAL:
			sourceSettings.balanceProcedure = balanceVersical;
			break;
		case BALANCE_VERSICAL_NO_KELVIN_ARM:
			sourceSettings.balanceProcedure = balanceVersicalNoKelvinArm;
			break;
		case BALANCE_GIQS:
			sourceSettings.balanceProcedure = balanceGiqs;
			break;
		case BALANCE_GIQS_NO_KELVIN_ARM:
			sourceSettings.balanceProcedure = balanceGiqsNoKelvinArm;
			break;
	}
	lockinSettings = lockinSettingsTmp;
	switchSettings = switchSettingsTmp;
	for (int j = 0; j < sourceSettingsTmp.nModes; ++j) {
		modeSettings[j] = modeSettingsTmp[j];
	}
	for (int j = 0; j < sourceSettingsTmp.nChannelSwitchModes; ++j) {
		channelSwitchSettings[j] = channelSwitchSettingsTmp[j];
	}
	recordSettings = recordSettingsTmp;	
	
	DADSS_SetNameNVServer(sourceSettings.nvServerName);
	DADSS_SetNameNVProcess(sourceSettings.nvProcessName);
	LIANV_SetNameNVServer(sourceSettings.nvServerName);
	DIO_SetNameNVServer(sourceSettings.nvServerName);
	DIO_SetNameNVProcess(switchSettings.nvProcessName);
}

void SaveSettings(const char *fileName)
{
	SettingsFileDescriptor settingsFileDescriptor;
	settingsFileDescriptor.fileName = fileName;
	settingsFileDescriptor.iniText = Ini_New(TRUE); 
	if (!settingsFileDescriptor.iniText) {
		warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], fileName, msgStrings[MSG_OUT_OF_MEMORY]);
		return;
	}

	if (SaveSourceSettings(settingsFileDescriptor) < 0 || 
			SaveModeSettings(settingsFileDescriptor) < 0 || 
			SaveRecordSettings(settingsFileDescriptor) < 0 || 
			SaveLockinSettings(settingsFileDescriptor) < 0 || 
			SaveSwitchSettings(settingsFileDescriptor) < 0 ||
	   		SaveChannelSwitchSettings(settingsFileDescriptor) < 0) {
		Ini_Dispose(settingsFileDescriptor.iniText);
		return;
	}
	
	int ret = 0;
	if ((ret = Ini_WriteToFile(settingsFileDescriptor.iniText, settingsFileDescriptor.fileName)) < 0)
		warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
	
	Ini_Dispose(settingsFileDescriptor.iniText);
	return;
}


static int LoadSourceSettings(SettingsFileDescriptor settingsFileDescriptor, SourceSettings *sourceSettings)
{
	int ret = 0;
	if ((ret = Ini_ReadFromFile(settingsFileDescriptor.iniText, settingsFileDescriptor.fileName)) < 0 ||
			(ret = Ini_GetStringIntoBuffer(settingsFileDescriptor.iniText, "Source", "Server name", sourceSettings->nvServerName, DADSS_NAME_SZ)) <= 0 ||
			(ret = Ini_GetStringIntoBuffer(settingsFileDescriptor.iniText, "Source", "Process name", sourceSettings->nvProcessName, DADSS_NAME_SZ)) <= 0 ||
			(ret = Ini_GetDouble(settingsFileDescriptor.iniText, "Source", "Clock frequency", &sourceSettings->clockFrequency)) <= 0 ||
			(ret = Ini_GetDouble(settingsFileDescriptor.iniText, "Source", "Frequency", &sourceSettings->frequency)) <= 0 ||
			(ret = Ini_GetInt(settingsFileDescriptor.iniText, "Source", "Modes", &sourceSettings->nModes)) <= 0 ||
			(ret = Ini_GetInt(settingsFileDescriptor.iniText, "Source", "Active mode", &sourceSettings->activeMode)) <= 0 ||
			(ret = Ini_GetInt(settingsFileDescriptor.iniText, "Source", "Channel switch modes", &sourceSettings->nChannelSwitchModes)) <= 0 ||
			(ret = Ini_GetInt(settingsFileDescriptor.iniText, "Source", "Balance procedure", &sourceSettings->balanceProcedureIdx)) <= 0 ||
			(ret = Ini_GetInt(settingsFileDescriptor.iniText, "Source", "Active channel", &sourceSettings->activeChannel)) <= 0) {
		if (ret == 0)
			warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], "Source");
		else
			warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], "Source");
		return ret;
	}
	
	sourceSettings->realFrequency = sourceSettings->frequency;
	sourceSettings->nSamples = (int)(sourceSettings->clockFrequency*DADSS_CLOCKFREQUENCY_MUL/sourceSettings->frequency);
	
	for (int i = 0; i < DADSS_CHANNELS; ++i) {
		char buf[BUF_SZ];
		snprintf(buf, BUF_SZ, "Range %d", i+1);
		if ((ret = Ini_GetInt(settingsFileDescriptor.iniText, "Source", buf, &sourceSettings->range[i])) <= 0) {
			if (ret == 0)
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], "Source");
			else
				warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], "Source");
			return ret;
		}

		snprintf(buf,BUF_SZ, "Label %d",i+1);
		if ((ret = Ini_GetStringIntoBuffer(settingsFileDescriptor.iniText, "Source", buf, sourceSettings->label[i], LABEL_SZ)) <= 0) {
			if (ret == 0)
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], "Source");
			else
				warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], "Source");
			return ret;
		}
	}

	return ret;
}

static int ValidateSourceSettings(SettingsFileDescriptor settingsFileDescriptor, SourceSettings sourceSettings)
{
	if (sourceSettings.nModes < 2 || sourceSettings.nModes > MAX_MODES || 
			sourceSettings.activeMode < 1 || sourceSettings.activeMode > sourceSettings.nModes-1 ||
			sourceSettings.clockFrequency < DADSS_CLOCKFREQUENCY_MIN || sourceSettings.clockFrequency > DADSS_CLOCKFREQUENCY_MAX ||
			sourceSettings.frequency < DADSS_FREQUENCY_MIN || sourceSettings.frequency > DADSS_FREQUENCY_MAX ||
			sourceSettings.nChannelSwitchModes < 1 || sourceSettings.nChannelSwitchModes > MAX_CHANNEL_SWITCH_MODES ||
			sourceSettings.activeChannel < 0 || sourceSettings.activeChannel > DADSS_CHANNELS-1 ||
	   		sourceSettings.balanceProcedureIdx < BALANCE_TWO_PORT || sourceSettings.balanceProcedureIdx > BALANCE_GIQS_NO_KELVIN_ARM) {
		warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_PARAMETER_OUT_OF_RANGE], "Source");
		return -1;
	}
	
	for (int i = 0; i < DADSS_CHANNELS; ++i) {
		char buf[BUF_SZ];
		snprintf(buf, BUF_SZ, "Range %d", i+1);
		if (sourceSettings.range[i] < DADSS_RANGE_1V || 
				sourceSettings.range[i] > DADSS_RANGE_10V) {
			warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_PARAMETER_OUT_OF_RANGE], buf);
			return -1;
		}
	}
	
	return 0;	
}

static int LoadRecordSettings(SettingsFileDescriptor settingsFileDescriptor, RecordSettings *recordSettings) 
{
	int ret = 0;
	
	if ((ret = Ini_GetDouble(settingsFileDescriptor.iniText, "Record", "Sampling period", &recordSettings->samplingPeriod)) <= 0 ||
			(ret = Ini_GetInt(settingsFileDescriptor.iniText, "Record", "Samples", &recordSettings->nSamples)) <= 0) {
		if (ret == 0)
			warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], "Record");
		else
			warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], "Record");
		return ret;
	}
	
	return ret;
}

static int ValidateRecordSettings(SettingsFileDescriptor settingsFileDescriptor, RecordSettings recordSettings)
{
	if (recordSettings.samplingPeriod < LIANV_MIN_SAMPLING_PERIOD ||
			recordSettings.nSamples < 1) {
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_PARAMETER_OUT_OF_RANGE], "Record");
				return -1;
	}
	
	return 0;
}

static int LoadLockinSettings(SettingsFileDescriptor settingsFileDescriptor, LockinSettings *lockinSettings)
{
	int ret = 0;
	if ((ret = Ini_GetBoolean(settingsFileDescriptor.iniText, "Lock-in", "Present", &lockinSettings->isPresent)) <= 0 ||
			(ret = Ini_GetStringIntoBuffer(settingsFileDescriptor.iniText, "Lock-in", "Process name", 
										   lockinSettings->nvProcessName, LIANV_NAME_SZ)) <= 0) {
		if (ret == 0)
			warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], "Lock-in");
		else
			warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], "Lock-in");
		return ret;
	}
	
	return ret;
}

static int LoadSwitchSettings(SettingsFileDescriptor settingsFileDescriptor, SwitchSettings *switchSettings)
{
	int ret = 0;
	if ((ret = Ini_GetBoolean(settingsFileDescriptor.iniText, "Switch", "Present", &switchSettings->isPresent)) <= 0 ||
			(ret = Ini_GetStringIntoBuffer(settingsFileDescriptor.iniText, "Switch", "Process name", switchSettings->nvProcessName, DIO_NAME_SZ)) <= 0) {
		if (ret == 0)
			warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], "Switch");
		else
			warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], "Switch");
		return ret;
	}
	
	return ret;
}

static int LoadChannelSwitchSettings(SettingsFileDescriptor settingsFileDescriptor, SourceSettings sourceSettings, ChannelSwitchSettings *channelSwitchSettings)
{
	int ret = 0;
	for (int j = 0; j < sourceSettings.nChannelSwitchModes; ++j) {
		char buf[BUF_SZ];
		snprintf(buf, BUF_SZ, "Channel switch mode %d", j);
		if ((ret = Ini_GetStringIntoBuffer(settingsFileDescriptor.iniText, buf, "Label", channelSwitchSettings[j].label, LABEL_SZ)) <= 0) {
			if (ret == 0)
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], buf);
			else
				warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], buf);
			return ret;
		}
		for (int i = 0; i < DIO_PORTS; ++i) {
			char fieldBuf[BUF_SZ];
			snprintf(fieldBuf, BUF_SZ, "Port %d", i);
			unsigned int portTmp;
			if ((ret = Ini_GetUInt(settingsFileDescriptor.iniText, buf, fieldBuf, &portTmp)) <= 0 ) {
				if (ret == 0)
					warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], buf);
				else
					warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], buf);
				return ret;
			}
			if (portTmp > UCHAR_MAX) {
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_PARAMETER_OUT_OF_RANGE], buf);
				return -5021;
			}
			channelSwitchSettings[j].switchPorts[i] = (unsigned char) portTmp;
		}
	}
	
	return ret;
}

static int LoadModeSettings(SettingsFileDescriptor settingsFileDescriptor, SourceSettings sourceSettings, ModeSettings *modeSettings)
{
	int ret = 0;
	for (int j = 0; j < sourceSettings.nModes; ++j) {
		for (int i = 0; i < DADSS_CHANNELS; ++i) {
			char buf[BUF_SZ];
			snprintf(buf, BUF_SZ, "Mode %d Channel %d", j, i+1);
		
			if ((ret = Ini_GetBoolean(settingsFileDescriptor.iniText, buf, "Locked", &modeSettings[j].channelSettings[i].isLocked)) <= 0 ||
					(ret = Ini_GetDouble(settingsFileDescriptor.iniText, buf, "Amplitude", &modeSettings[j].channelSettings[i].amplitude)) <= 0 ||
					(ret = Ini_GetDouble(settingsFileDescriptor.iniText, buf, "Phase", &modeSettings[j].channelSettings[i].phase)) <= 0 ||
					(ret = Ini_GetUInt(settingsFileDescriptor.iniText, buf, "MDAC2 code", &modeSettings[j].channelSettings[i].mdac2Code)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "MDAC1 offset", &modeSettings[j].channelSettings[i].mdac1Offset)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in gain mode", &modeSettings[j].channelSettings[i].gainMode)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in sensitivity", &modeSettings[j].channelSettings[i].lockinInputSettings.sensitivity)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in time constant", &modeSettings[j].channelSettings[i].lockinInputSettings.timeConstant)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in filter slope", &modeSettings[j].channelSettings[i].lockinInputSettings.filterSlope)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in input configuration", &modeSettings[j].channelSettings[i].lockinInputSettings.inputConfiguration)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in input coupling", &modeSettings[j].channelSettings[i].lockinInputSettings.inputCoupling)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in input shield grounding", &modeSettings[j].channelSettings[i].lockinInputSettings.inputShieldGrounding)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in input reserve", &modeSettings[j].channelSettings[i].lockinInputSettings.inputReserve)) <= 0 ||
					(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Lock-in input line notch filter", &modeSettings[j].channelSettings[i].lockinInputSettings.inputLineNotchFilter)) <= 0 ||
					(ret = Ini_GetDouble(settingsFileDescriptor.iniText, buf, "Balance threshold", &modeSettings[j].channelSettings[i].balanceThreshold)) <= 0 ||
					(ret = Ini_GetDouble(settingsFileDescriptor.iniText, buf, "Check threshold", &modeSettings[j].channelSettings[i].checkThreshold)) <= 0 ||
			   		(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Active switch mode", &modeSettings[j].channelSettings[i].activeSwitchMode)) <= 0) {
				if (ret == 0)
					warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], buf);
				else
					warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], buf);
				return ret;
			}
		}
	
		char buf[BUF_SZ];
		snprintf(buf, BUF_SZ, "Mode %d", j);
		if ((ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Voltage channel A", &modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A])) <= 0 ||
				(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Current channel A", &modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A])) <= 0 ||
				(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Voltage channel B", &modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B])) <= 0 ||
				(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Current channel B", &modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B])) <= 0 ||
				(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Kelvin arm channel", &modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL])) <= 0 ||
				(ret = Ini_GetInt(settingsFileDescriptor.iniText, buf, "Injection channel", &modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL])) <= 0 ||
				(ret = Ini_GetDouble(settingsFileDescriptor.iniText, buf, "Voltage channel series resistance A", &modeSettings[j].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_A])) <= 0 ||
				(ret = Ini_GetDouble(settingsFileDescriptor.iniText, buf, "Current channel series resistance A", &modeSettings[j].bridgeSettings.seriesResistance[CURRENT_CHANNEL_A])) <= 0 ||
				(ret = Ini_GetDouble(settingsFileDescriptor.iniText, buf, "Voltage channel series resistance B", &modeSettings[j].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_B])) <= 0 ||
				(ret = Ini_GetDouble(settingsFileDescriptor.iniText, buf, "Current channel series resistance B", &modeSettings[j].bridgeSettings.seriesResistance[CURRENT_CHANNEL_B])) <= 0) {
			if (ret == 0)
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], buf);
			else
				warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], buf);
			return ret;
		}
		
		for (int i = 0; i < DIO_PORTS; ++i) {
			char fieldBuf[BUF_SZ];
			snprintf(fieldBuf, BUF_SZ, "Port %d", i);
			unsigned int portTmp;
			if ((ret = Ini_GetUInt(settingsFileDescriptor.iniText, buf, fieldBuf, &portTmp)) <= 0) {
				if (ret == 0)
					warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], buf);
				else
					warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], buf);
				return ret;
			}
			if (portTmp > UCHAR_MAX) {
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_PARAMETER_OUT_OF_RANGE], buf);
				return -5021;
			}
			modeSettings[j].switchPorts[i] = (unsigned char) portTmp;
		}
		
		if ((ret = Ini_GetStringIntoBuffer(settingsFileDescriptor.iniText, buf, "Label", modeSettings[j].label, LABEL_SZ)) <= 0) {
			if (ret == 0)
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_MISSING_PARAMETER], buf);
			else
				warn("%s %s\n%s %s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret), msgStrings[MSG_SETTINGS_SECTION], buf);
			return ret;
		}
	}
	
	return ret;
}

static int ValidateModeSettings(SettingsFileDescriptor settingsFileDescriptor, SourceSettings sourceSettings, ModeSettings *modeSettings)
{
	for (int j = 0; j < sourceSettings.nModes; ++j) {
		for (int i = 0; i < DADSS_CHANNELS; ++i) {
			char buf[BUF_SZ];
			snprintf(buf, BUF_SZ, "Mode %d Channel %d", j, i+1);
			if (modeSettings[j].channelSettings[i].amplitude < DADSS_AMPLITUDE_MIN ||
					modeSettings[j].channelSettings[i].amplitude > DADSS_AMPLITUDE_MAX ||
					modeSettings[j].channelSettings[i].phase < DADSS_PHASE_MIN ||
					modeSettings[j].channelSettings[i].phase > DADSS_PHASE_MAX ||
					modeSettings[j].channelSettings[i].mdac1Offset < DADSS_MDAC1_OFFSET_MIN ||
					modeSettings[j].channelSettings[i].mdac1Offset > DADSS_MDAC1_OFFSET_MAX ||
					modeSettings[j].channelSettings[i].mdac2Code > DADSS_MDAC2_CODE_MAX ||
					modeSettings[j].channelSettings[i].balanceThreshold <= 0 ||
					modeSettings[j].channelSettings[i].checkThreshold <= 0 ||
					modeSettings[j].channelSettings[i].activeSwitchMode < 0 ||
					modeSettings[j].channelSettings[i].activeSwitchMode > sourceSettings.nChannelSwitchModes-1 ||
			        modeSettings[j].channelSettings[i].gainMode < LIANV_GAIN_MANUAL ||
			   		modeSettings[j].channelSettings[i].gainMode > LIANV_GAIN_AUTO ||
					modeSettings[j].channelSettings[i].lockinInputSettings.sensitivity < LIANV_SENSITIVITY_2NV ||
					modeSettings[j].channelSettings[i].lockinInputSettings.sensitivity > LIANV_SENSITIVITY_1V ||
					modeSettings[j].channelSettings[i].lockinInputSettings.timeConstant < LIANV_TIME_CONSTANT_10US ||
					modeSettings[j].channelSettings[i].lockinInputSettings.timeConstant > LIANV_TIME_CONSTANT_30KS ||
					modeSettings[j].channelSettings[i].lockinInputSettings.filterSlope < LIANV_SLOPE_6 ||
					modeSettings[j].channelSettings[i].lockinInputSettings.filterSlope > LIANV_SLOPE_24 ||
					modeSettings[j].channelSettings[i].lockinInputSettings.inputReserve < LIANV_INPUT_RESERVE_HIGH ||  
					modeSettings[j].channelSettings[i].lockinInputSettings.inputReserve > LIANV_INPUT_RESERVE_LOW_NOISE ||  
			   		modeSettings[j].channelSettings[i].lockinInputSettings.inputConfiguration < LIANV_INPUT_VOLTAGE_SINGLE_ENDED ||
			   		modeSettings[j].channelSettings[i].lockinInputSettings.inputConfiguration > LIANV_INPUT_CURRENT_100MOHM ||
					modeSettings[j].channelSettings[i].lockinInputSettings.inputCoupling < LIANV_INPUT_COUPLING_AC ||
					modeSettings[j].channelSettings[i].lockinInputSettings.inputCoupling > LIANV_INPUT_COUPLING_DC || 
					modeSettings[j].channelSettings[i].lockinInputSettings.inputLineNotchFilter < LIANV_INPUT_LINE_NOTCH_FILTER_OFF ||   
					modeSettings[j].channelSettings[i].lockinInputSettings.inputLineNotchFilter > LIANV_INPUT_LINE_NOTCH_FILTER_BOTH ||   
			   		modeSettings[j].channelSettings[i].lockinInputSettings.inputShieldGrounding < LIANV_INPUT_FLOAT ||
			   		modeSettings[j].channelSettings[i].lockinInputSettings.inputShieldGrounding > LIANV_INPUT_GROUND) {
				warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_PARAMETER_OUT_OF_RANGE], buf);
				return -1;
			}
		
			ToRect(modeSettings[j].channelSettings[i].amplitude, modeSettings[j].channelSettings[i].phase, 
				   &modeSettings[j].channelSettings[i].real, &modeSettings[j].channelSettings[i].imag);
			DADSS_Mdac2CodeToValue(modeSettings[j].channelSettings[i].mdac2Code, &modeSettings[j].channelSettings[i].mdac2Val);	
		}
		
		char buf[BUF_SZ];
		snprintf(buf, BUF_SZ, "Mode %d", j);		
		if (modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A] < 0 ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A] > DADSS_CHANNELS-1 ||
				modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A] < 0 ||
				modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A] > DADSS_CHANNELS-1 ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B] < 0 ||	
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B] > DADSS_CHANNELS-1 ||
				modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B] < 0 ||
				modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B] > DADSS_CHANNELS-1 ||
				modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL] < 0 ||
				modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL] > DADSS_CHANNELS-1 ||
				modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL] < 0 ||
				modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL] > DADSS_CHANNELS-1 ||
				modeSettings[j].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_A] < 0 ||
				modeSettings[j].bridgeSettings.seriesResistance[CURRENT_CHANNEL_A] < 0 ||
				modeSettings[j].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_B] < 0 ||
				modeSettings[j].bridgeSettings.seriesResistance[CURRENT_CHANNEL_B] < 0 ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A] ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B] ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B] ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL] ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL] ||
				modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B] ||
				modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B] ||
				modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL] ||
				modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A] == modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL] ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B] == modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B] ||
		   		modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B] == modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL] ||
				modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B] == modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL] ||
		   		modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B] == modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL] ||
		   		modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B] == modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL] ||
		   		modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL] == modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL]) {
					warn("%s %s\n%s [%s]", msgStrings[MSG_LOADING_ERROR], settingsFileDescriptor.fileName, msgStrings[MSG_SETTINGS_PARAMETER_OUT_OF_RANGE], buf);
					return -1;
		}
	}
	
	return 0;
}

static int SaveSourceSettings(SettingsFileDescriptor settingsFileDescriptor)
{
	int ret = 0;
	if ((ret = Ini_PutString(settingsFileDescriptor.iniText, "Source", "Server name", sourceSettings.nvServerName)) < 0 ||
			(ret = Ini_PutString(settingsFileDescriptor.iniText, "Source", "Process name", sourceSettings.nvProcessName)) < 0 ||
			(ret = Ini_PutDouble(settingsFileDescriptor.iniText, "Source", "Clock frequency", sourceSettings.clockFrequency)) < 0 ||
			(ret = Ini_PutDouble(settingsFileDescriptor.iniText, "Source", "Frequency", sourceSettings.frequency)) < 0 ||
			(ret = Ini_PutInt(settingsFileDescriptor.iniText, "Source", "Modes", sourceSettings.nModes)) < 0 ||
			(ret = Ini_PutInt(settingsFileDescriptor.iniText, "Source", "Active mode", sourceSettings.activeMode)) < 0 ||
			(ret = Ini_PutInt(settingsFileDescriptor.iniText, "Source", "Channel switch modes", sourceSettings.nChannelSwitchModes)) < 0 ||
			(ret = Ini_PutInt(settingsFileDescriptor.iniText, "Source", "Active channel", sourceSettings.activeChannel)) < 0 ||
			(ret = Ini_PutInt(settingsFileDescriptor.iniText, "Source", "Balance procedure", sourceSettings.balanceProcedureIdx)) < 0) {
				warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
				return ret;
	}	
	
	for (int i = 0; i < DADSS_CHANNELS; ++i) {
		char buf[BUF_SZ];
		snprintf(buf, BUF_SZ, "Range %d", i+1);
		if ((ret = Ini_PutInt(settingsFileDescriptor.iniText, "Source", buf, sourceSettings.range[i])) < 0) {
			warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
			return ret;
		}
		snprintf(buf, BUF_SZ, "Label %d", i+1);
		if ((ret = Ini_PutString(settingsFileDescriptor.iniText, "Source", buf, sourceSettings.label[i])) < 0) {
			warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
			return ret;
		}
	}
	
	return ret;
}

static int SaveModeSettings(SettingsFileDescriptor settingsFileDescriptor)
{
	int ret = 0;
	for (int j = 0; j < sourceSettings.nModes; ++j) {
		for (int i = 0; i < DADSS_CHANNELS; ++i) {
			char buf[BUF_SZ];
			snprintf(buf, BUF_SZ, "Mode %d Channel %d", j, i+1);
		
			if ((ret = Ini_PutBoolean(settingsFileDescriptor.iniText, buf, "Locked", modeSettings[j].channelSettings[i].isLocked)) < 0 ||
					(ret = Ini_PutDouble(settingsFileDescriptor.iniText, buf, "Amplitude", modeSettings[j].channelSettings[i].amplitude)) < 0 ||
					(ret = Ini_PutDouble(settingsFileDescriptor.iniText, buf, "Phase", modeSettings[j].channelSettings[i].phase)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "MDAC1 offset", modeSettings[j].channelSettings[i].mdac1Offset)) < 0 ||
					(ret = Ini_PutUInt(settingsFileDescriptor.iniText, buf, "MDAC2 code", modeSettings[j].channelSettings[i].mdac2Code)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in gain mode", modeSettings[j].channelSettings[i].gainMode)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in sensitivity", modeSettings[j].channelSettings[i].lockinInputSettings.sensitivity)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in time constant", modeSettings[j].channelSettings[i].lockinInputSettings.timeConstant)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in filter slope", modeSettings[j].channelSettings[i].lockinInputSettings.filterSlope)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in input configuration", modeSettings[j].channelSettings[i].lockinInputSettings.inputConfiguration)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in input coupling", modeSettings[j].channelSettings[i].lockinInputSettings.inputCoupling)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in input shield grounding", modeSettings[j].channelSettings[i].lockinInputSettings.inputShieldGrounding)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in input reserve", modeSettings[j].channelSettings[i].lockinInputSettings.inputReserve)) < 0 || 
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Lock-in input line notch filter", modeSettings[j].channelSettings[i].lockinInputSettings.inputLineNotchFilter)) < 0 ||
					(ret = Ini_PutDouble(settingsFileDescriptor.iniText, buf, "Balance threshold", modeSettings[j].channelSettings[i].balanceThreshold)) < 0 ||
					(ret = Ini_PutDouble(settingsFileDescriptor.iniText, buf, "Check threshold", modeSettings[j].channelSettings[i].checkThreshold)) < 0 ||
					(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Active switch mode", modeSettings[j].channelSettings[i].activeSwitchMode)) < 0) {
						warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
						return ret;
			}
		}
		
		char buf[BUF_SZ];
		snprintf(buf, BUF_SZ, "Mode %d", j);
		if ((ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Voltage channel A", modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A])) < 0 ||
				(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Current channel A", modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A])) < 0 ||
				(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Voltage channel B", modeSettings[j].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B])) < 0 ||
				(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Current channel B", modeSettings[j].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B])) < 0 ||
				(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Kelvin arm channel", modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL])) < 0 ||
				(ret = Ini_PutInt(settingsFileDescriptor.iniText, buf, "Injection channel", modeSettings[j].bridgeSettings.channelAssignment[INJECTION_CHANNEL])) < 0 ||
				(ret = Ini_PutDouble(settingsFileDescriptor.iniText, buf, "Voltage channel series resistance A", modeSettings[j].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_A])) < 0 ||
				(ret = Ini_PutDouble(settingsFileDescriptor.iniText, buf, "Current channel series resistance A", modeSettings[j].bridgeSettings.seriesResistance[CURRENT_CHANNEL_A])) < 0 ||
				(ret = Ini_PutDouble(settingsFileDescriptor.iniText, buf, "Voltage channel series resistance B", modeSettings[j].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_B])) < 0 ||
				(ret = Ini_PutDouble(settingsFileDescriptor.iniText, buf, "Current channel series resistance B", modeSettings[j].bridgeSettings.seriesResistance[CURRENT_CHANNEL_B])) < 0) {
					warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
					return ret;
		}
		
		for (int i = 0; i < DIO_PORTS; ++i) {
			char fieldBuf[BUF_SZ];
			snprintf(fieldBuf, BUF_SZ, "Port %d", i);
			if ((ret = Ini_PutUInt(settingsFileDescriptor.iniText, buf, fieldBuf, modeSettings[j].switchPorts[i])) < 0 ) {
				warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
				return ret;			
			}
		}
		
		if ((ret = Ini_PutString(settingsFileDescriptor.iniText, buf, "Label", modeSettings[j].label)) < 0) {
			warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
			return ret;			
		}
	}	
	
	return ret;
}

static int SaveRecordSettings(SettingsFileDescriptor settingsFileDescriptor)
{
	int ret = 0;
	if ((ret = Ini_PutInt(settingsFileDescriptor.iniText, "Record", "Samples", recordSettings.nSamples)) < 0 ||
			(ret = Ini_PutDouble(settingsFileDescriptor.iniText, "Record", "Sampling period", recordSettings.samplingPeriod)) < 0) {
				warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
				return ret;
	}
	
	return ret;
}

static int SaveLockinSettings(SettingsFileDescriptor settingsFileDescriptor)
{
	int ret = 0;
	if ((ret = Ini_PutBoolean(settingsFileDescriptor.iniText, "Lock-in", "Present", lockinSettings.isPresent)) < 0 ||
			(ret = Ini_PutString(settingsFileDescriptor.iniText, "Lock-in", "Process name", lockinSettings.nvProcessName)) < 0) {
				warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
				return ret;
	}
	
	return ret;
}

static int SaveSwitchSettings(SettingsFileDescriptor settingsFileDescriptor)
{
	int ret = 0;
	if ((ret = Ini_PutBoolean(settingsFileDescriptor.iniText, "Switch", "Present", switchSettings.isPresent)) < 0 ||
			(ret = Ini_PutString(settingsFileDescriptor.iniText, "Switch", "Process name", switchSettings.nvProcessName)) < 0) {
				warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
				return ret;
	}
	
	return ret;
}

static int SaveChannelSwitchSettings(SettingsFileDescriptor settingsFileDescriptor)
{
	int ret = 0;
	for (int j = 0; j < sourceSettings.nChannelSwitchModes; ++j) {
		char buf[BUF_SZ];
		snprintf(buf, BUF_SZ, "Channel switch mode %d", j);
		if ((ret = Ini_PutString(settingsFileDescriptor.iniText, buf, "Label", channelSwitchSettings[j].label)) < 0) {
			warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
			return ret;
		}
		for (int i = 0; i < DIO_PORTS; ++i) {
			char fieldBuf[BUF_SZ];
			snprintf(fieldBuf, BUF_SZ, "Port %d", i);
			if ((ret = Ini_PutUInt(settingsFileDescriptor.iniText, buf, fieldBuf, channelSwitchSettings[j].switchPorts[i])) < 0 ) {
				warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFileDescriptor.fileName, GetGeneralErrorString(ret));
				return ret;
			}
		}
	}
	
	return ret;
}

