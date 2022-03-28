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

#include <ansi_c.h>
#include <userint.h>
#include <utility.h> 
#include <advanlys.h>

#include "toolbox.h" 

#include "main.h"
#include "msg.h"
#include "cfg.h"
#include "panel.h"
#include "panels.h"
#include "balance.h"
#include "DA_DSS_cvi_driver.h"
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

static char *GetFileNameWithoutExtension(char *, const char *, size_t );

//==============================================================================
// Global variables

//==============================================================================
// Global functions

void CVICALLBACK FileMenuNew (int menuBar, int menuItem, void *callbackData,
							  int panel)
{
	int ret = FileSelectPopup("", 
							  "",
							  "*.*",
							  msgStrings[MSG_POPUP_NEW_FILE_TITLE],
							  VAL_SAVE_BUTTON,
							  0,
							  0,
							  1,
							  1,
							  sourceSettings.dataPathName);
	switch (ret) {
		case VAL_NO_FILE_SELECTED:
			lockinSettings.isBalanceInterrupted = 1;
			break;
		case VAL_EXISTING_FILE_SELECTED:
		case VAL_NEW_FILE_SELECTED:
			sourceSettings.dataFileHandle = fopen(sourceSettings.dataPathName, "a");
    		if(sourceSettings.dataFileHandle == NULL) {
        		warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
				strcpy(sourceSettings.dataPathName, "");
				UpdatePanel(panel);
				lockinSettings.isBalanceInterrupted = 1;
        		return;
    		}
			if (fprintf(sourceSettings.dataFileHandle,"Timestamp\tMode\tFrequency") < 0) {
				warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
				goto Error;	
			}
			for (int i = 0; i < DADSS_CHANNELS; ++i) {
				if (fprintf(sourceSettings.dataFileHandle, "\tRe(%s)\tIm(%s)\tThreshold(%s)\tRange(%s)",\
							sourceSettings.label[i], sourceSettings.label[i], 
							sourceSettings.label[i], sourceSettings.label[i]) < 0) {
					warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
					goto Error;
    			}
			}
			if (fprintf(sourceSettings.dataFileHandle,"\tRe(DLA)\tIm(DLA)\tRe(DLB)\tIm(DLB)\tRe(DHA)\tIm(DHA)\tRe(DHB)\tIm(DHB)") < 0) {
				warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
				goto Error;	
			}
			if (fflush(sourceSettings.dataFileHandle)) {
				warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
				goto Error;	
			}
			sourceSettings.isFileModified = 1;
			UpdatePanel(panel);
			break;		
		default:
			die(GetUILErrorString(ret));
	}
	return;
	
Error: 
	FileMenuClose(menuBar, menuItem, callbackData, panel);
	lockinSettings.isBalanceInterrupted = 1;
}

void CVICALLBACK FileMenuSave (int menuBar, int menuItem, void *callbackData,
							   int panel)
{
	int nSamples;
	int samples[DADSS_SAMPLES_MAX] = {0};
	double xReal[DADSS_SAMPLES_MAX] = {0.0};
	double xImag[DADSS_SAMPLES_MAX] = {0.0};
	double dacScale;
	double timeStamp;
	char timeStampBuf[16]; // YYYYMMDDTHHMMSS

	UpdateActiveMode(panel, PANEL_UPDATE_MODE, EVENT_COMMIT, NULL, 0, 0);
	if (sourceSettings.dataFileHandle == NULL) 
		FileMenuNew(menuBar, menuItem, callbackData, panel);
	if (sourceSettings.dataFileHandle == NULL) {
		lockinSettings.isBalanceInterrupted = 1;
		return;
	}
	
	GetCurrentDateTime(&timeStamp);
	FormatDateTimeString(timeStamp, "%Y%m%dT%H%M%S", timeStampBuf, sizeof timeStampBuf);
	if (fprintf(sourceSettings.dataFileHandle,"\n%s\t%s\t%.11g", timeStampBuf, modeSettings[0].label, sourceSettings.realFrequency) < 0) {
		warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
		goto Error;
	}

	DSSERRCHK(DADSS_GetNumberSamples(&nSamples));
	for (int i = 0; i < DADSS_CHANNELS; ++i) {
		DSSERRCHK(DADSS_GetWaveform(i+1,samples,nSamples));
		for (int j = 0; j < DADSS_SAMPLES_MAX; ++j)
			xReal[j] = samples[j];
		ALERRCHK(FFT(xReal, xImag, nSamples)); 
		dacScale = (modeSettings[0].channelSettings[i].mdac2Val) * \ 
				   (DADSS_RangeMultipliers[sourceSettings.range[i]]/DADSS_MDAC1_CODE_RANGE) * \
				   DADSS_REFERENCE_VOLTAGE/nSamples;
		if (fprintf(sourceSettings.dataFileHandle,"\t% 16.10e\t% 16.10e\t% 16.10e\t%d",
					(xImag[nSamples-1]-xImag[1])*dacScale,
					(xReal[1]+xReal[nSamples-1])*dacScale,
					modeSettings[0].channelSettings[i].checkThreshold,
					sourceSettings.range[i]) < 0) {
			warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
			goto Error;
		}
	}
	if (sourceSettings.balanceProcedureIdx == BALANCE_GIQS || sourceSettings.balanceProcedureIdx == BALANCE_GIQS_NO_KELVIN_ARM) {
		if (fprintf(sourceSettings.dataFileHandle,"\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e",
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[INJECTION_CHANNEL]].lockinReading[0],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[INJECTION_CHANNEL]].lockinReading[1],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL]].lockinReading[0],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL]].lockinReading[1],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]].lockinReading[0],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]].lockinReading[1],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]].lockinReading[0],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]].lockinReading[1]) < 0) {
				warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
				goto Error;
		}
	} else {
		if (fprintf(sourceSettings.dataFileHandle,"\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e\t% 11.5e",
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B]].lockinReading[0],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B]].lockinReading[1],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL]].lockinReading[0],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL]].lockinReading[1],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]].lockinReading[0],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]].lockinReading[1],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]].lockinReading[0],
						modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]].lockinReading[1]) < 0) {
				warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
				goto Error;
		}
	}
	if (fflush(sourceSettings.dataFileHandle)) {
		warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
		goto Error;	
	}
	sourceSettings.isFileModified = 0;
	UpdatePanelTitle(panel);
	return;

Error: 
	FileMenuClose(menuBar, menuItem, callbackData, panel);
	lockinSettings.isBalanceInterrupted = 1;
}


void CVICALLBACK FileMenuClose (int menuBar, int menuItem, void *callbackData,
								int panel)
{
	if (sourceSettings.dataFileHandle != NULL) {
		if (fprintf(sourceSettings.dataFileHandle,"\n") < 0) {
				warn("%s %s", msgStrings[MSG_SAVING_ERROR], sourceSettings.dataPathName);
				lockinSettings.isBalanceInterrupted = 1;
		}
		fclose(sourceSettings.dataFileHandle);
		sourceSettings.dataFileHandle = NULL;
		sourceSettings.isFileModified = 0;

		char settingsFile[MAX_PATHNAME_LEN];
		GetFileNameWithoutExtension(settingsFile, sourceSettings.dataPathName, MAX_PATHNAME_LEN);
		if (strlen(settingsFile) < MAX_PATHNAME_LEN-4) {
			strcat(settingsFile, ".ini");
			if (!FileExists(settingsFile, 0))
				SaveSettings(settingsFile);
			else
				warn("%s %s\n%s", msgStrings[MSG_SAVING_ERROR], settingsFile, msgStrings[MSG_FILE_ALREADY_EXISTS]);
		}
		
		strcpy(sourceSettings.dataPathName, "");
		
		UpdatePanel(panel);
	}
}


void CVICALLBACK FileMenuExit (int menuBar, int menuItem, void *callbackData,
							   int panel)
{
	ManagePanel(panel, EVENT_CLOSE, NULL, 0, 0);
}

void CVICALLBACK SettingsMenuConnection (int menuBar, int menuItem, void *callbackData,
										 int panel)
{
	int settingsPanel = 0;
	UIERRCHK(settingsPanel = LoadPanel(0, panelsFile, PANEL_CON2));
	
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CON2_NV_SERVER_NAME, ATTR_MAX_ENTRY_LENGTH, DADSS_NAME_SZ-1));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CON2_SOURCE_PROCESS_NAME, ATTR_MAX_ENTRY_LENGTH, DADSS_NAME_SZ-1));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CON2_LOCKIN_PROCESS_NAME, ATTR_MAX_ENTRY_LENGTH, LIANV_NAME_SZ-1));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CON2_SWITCH_PROCESS_NAME, ATTR_MAX_ENTRY_LENGTH, DIO_NAME_SZ-1));
	
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CON2_NV_SERVER_NAME, ATTR_TOOLTIP_TEXT, tooltipStrings[TOOLTIP_CON2_NV_SERVER_NAME]));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CON2_LOCKIN_ISPRESENT, ATTR_TOOLTIP_TEXT, tooltipStrings[TOOLTIP_CON2_CHECK]));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CON2_SWITCH_ISPRESENT, ATTR_TOOLTIP_TEXT, tooltipStrings[TOOLTIP_CON2_CHECK]));
	
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_CON2_NV_SERVER_NAME, sourceSettings.nvServerName));
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_CON2_SOURCE_PROCESS_NAME, sourceSettings.nvProcessName));
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_CON2_LOCKIN_ISPRESENT, lockinSettings.isPresent));
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_CON2_LOCKIN_PROCESS_NAME, lockinSettings.nvProcessName));
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_CON2_SWITCH_ISPRESENT, switchSettings.isPresent));
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_CON2_SWITCH_PROCESS_NAME, switchSettings.nvProcessName));
	
	UIERRCHK(InstallPopup(settingsPanel));
}

void CVICALLBACK SettingsMenuModes (int menuBar, int menuItem, void *callbackData,
									int panel)
{
	int settingsPanel = 0;
	UIERRCHK(settingsPanel = LoadPanel(0, panelsFile, PANEL_MOD));
	
	if (switchSettings.isPresent) {
		UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_MOD_SWITCH_SETTINGS, ATTR_DIMMED, 0));
	} else {
		UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_MOD_SWITCH_SETTINGS, ATTR_DIMMED, 1));
	}
	
	UIERRCHK(SetTableColumnAttribute (settingsPanel, PANEL_MOD_LIST, 1, ATTR_MAX_ENTRY_LENGTH, LABEL_SZ-1));
	UIERRCHK(InsertTableRows(settingsPanel, PANEL_MOD_LIST, -1, sourceSettings.nModes-1, VAL_CELL_STRING));
	for (int i = 1; i < sourceSettings.nModes; ++i)
		UIERRCHK(SetTableCellVal(settingsPanel, PANEL_MOD_LIST, MakePoint(1,i), modeSettings[i].label));
	
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_MOD_LIST, ATTR_ENABLE_COLUMN_SIZING, 0));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_MOD_LIST, ATTR_ENABLE_ROW_SIZING, 0));
	UIERRCHK(SetPanelAttribute(settingsPanel, ATTR_CLOSE_ITEM_VISIBLE, 0));
	
	UIERRCHK(InstallPopup(settingsPanel));
}

void CVICALLBACK SettingsMenuChannelSwitch (int menuBar, int menuItem, void *callbackData,
											int panel)
{
	int settingsPanel = 0;
	UIERRCHK(settingsPanel = LoadPanel(0, panelsFile, PANEL_CHSW));
	
	UIERRCHK(SetTableColumnAttribute (settingsPanel, PANEL_CHSW_LIST, 1, ATTR_MAX_ENTRY_LENGTH, LABEL_SZ-1));
	UIERRCHK(InsertTableRows(settingsPanel, PANEL_CHSW_LIST, -1, sourceSettings.nChannelSwitchModes, VAL_CELL_STRING));
	for (int i = 0; i < sourceSettings.nChannelSwitchModes; ++i)
		UIERRCHK(SetTableCellVal(settingsPanel, PANEL_CHSW_LIST, MakePoint(1,i+1), channelSwitchSettings[i].label));
	
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CHSW_LIST, ATTR_ENABLE_COLUMN_SIZING, 0));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_CHSW_LIST, ATTR_ENABLE_ROW_SIZING, 0));
	UIERRCHK(SetPanelAttribute(settingsPanel, ATTR_CLOSE_ITEM_VISIBLE, 0));
	
	UIERRCHK(InstallPopup(settingsPanel));
}

void CVICALLBACK SettingsMenuPreset (int menuBar, int menuItem, void *callbackData,
									 int panel)
{
	int settingsPanel = 0;
	UIERRCHK(settingsPanel = LoadPanel(0, panelsFile, PANEL_PRE));
	
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_PRE_PRIMARY_A, ATTR_MIN_VALUE, 0.0));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_PRE_PRIMARY_A, ATTR_MAX_VALUE, INFINITY));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_PRE_PRIMARY_B, ATTR_MIN_VALUE, 0.0));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_PRE_PRIMARY_B, ATTR_MAX_VALUE, INFINITY));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_PRE_SECONDARY_A, ATTR_MAX_VALUE, INFINITY));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_PRE_SECONDARY_B, ATTR_MAX_VALUE, INFINITY));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_PRE_RMS_CURRENT, ATTR_MIN_VALUE, 0.0));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_PRE_RMS_CURRENT, ATTR_MAX_VALUE, DADSS_MAX_RMS_OUTPUT_CURRENT));
	
	UIERRCHK(InstallPopup(settingsPanel));
}


void CVICALLBACK SettingsMenuLoad (int menuBar, int menuItem, void *callbackData,
								   int panel)
{
	char settingsFile[MAX_PATHNAME_LEN];
	int ret = FileSelectPopup("", 
							  "*.ini", 
							  "*.*", 
							  msgStrings[MSG_POPUP_LOAD_SETTINGS_TITLE], 
							  VAL_LOAD_BUTTON , 
							  0, 
							  0, 
							  1, 
							  0, 
							  settingsFile);
	switch (ret) {
		case VAL_NO_FILE_SELECTED:
			break;
		case VAL_EXISTING_FILE_SELECTED:
			LoadSettings(settingsFile);
			UpdatePanel(panel);
			break;
		default:
			die(GetUILErrorString(ret));
	}
}


void CVICALLBACK SettingsMenuSave (int menuBar, int menuItem, void *callbackData,
								   int panel)
{
	char settingsFile[MAX_PATHNAME_LEN];

	int ret = FileSelectPopup("",
							  "*.ini", 
							  "*.*", 
							  msgStrings[MSG_POPUP_SAVE_SETTINGS_TITLE], 
							  VAL_SAVE_BUTTON, 
							  0, 
							  0, 
							  1, 
							  1, 
							  settingsFile);
	switch (ret) {
		case VAL_NO_FILE_SELECTED:
			break;
		case VAL_EXISTING_FILE_SELECTED:
		case VAL_NEW_FILE_SELECTED:
			SaveSettings(settingsFile);
			break;
		default:
			die(GetUILErrorString(ret));
	}
}

void CVICALLBACK SettingsMenuReset (int menuBar, int menuItem, void *callbackData,
									int panel)
{
	int ret = 0;
	UIERRCHK(ret = ConfirmPopup(msgStrings[MSG_POPUP_CONFIRM_TITLE], msgStrings[MSG_POPUP_RESET_SETTINGS]));
	if (ret == 0)
		return;
	
	SetDefaultSettings();
	UpdatePanel(panel);
}

void CVICALLBACK SettingsMenuRecord (int menuBar, int menuItem, void *callbackData,
									 int panel)
{
	int settingsPanel = 0;
	UIERRCHK(settingsPanel = LoadPanel(0, panelsFile, PANEL_REC));
	
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_REC_DATA_FILE_NAME, ATTR_MAX_ENTRY_LENGTH, MAX_PATHNAME_LEN-1));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_REC_SAMPLES, ATTR_MIN_VALUE, 1));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_REC_SAMPLES, ATTR_MAX_VALUE, INT_MAX));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_REC_SAMPLING_PERIOD, ATTR_MIN_VALUE, LIANV_MIN_SAMPLING_PERIOD));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_REC_SAMPLING_PERIOD, ATTR_MAX_VALUE, LIANV_MAX_SAMPLING_PERIOD));
	
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_REC_SAMPLING_PERIOD, ATTR_TOOLTIP_TEXT, tooltipStrings[TOOLTIP_REC_SAMPLING_PERIOD]));
	UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_REC_DATA_FILE_NAME, ATTR_TOOLTIP_TEXT, tooltipStrings[TOOLTIP_REC_DATA_FILE_NAME]));
	
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_REC_DATA_FILE_NAME, recordSettings.dataPathName));
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_REC_SAMPLES, recordSettings.nSamples));
	UIERRCHK(SetCtrlVal(settingsPanel, PANEL_REC_SAMPLING_PERIOD, recordSettings.samplingPeriod));
		
	UIERRCHK(InstallPopup(settingsPanel));
}

void CVICALLBACK SettingsMenuBalance (int menuBar, int menuItem, void *callbackData,
									  int panel)
{
	switch (menuItem)
	{
		case MENUBAR_SETTINGS_BALANCE_TWO_PORT:
			sourceSettings.balanceProcedureIdx = BALANCE_TWO_PORT;
			sourceSettings.balanceProcedure = balanceTwoPort;
			break;
		case MENUBAR_SETTINGS_BALANCE_VERSICAL:
			sourceSettings.balanceProcedureIdx = BALANCE_VERSICAL;
			sourceSettings.balanceProcedure = balanceVersical;
			break;
		case MENUBAR_SETTINGS_BALANCE_VERSICAL_NO_KELVIN:
			sourceSettings.balanceProcedureIdx = BALANCE_VERSICAL_NO_KELVIN_ARM;
			sourceSettings.balanceProcedure = balanceVersicalNoKelvinArm;
			break;
		case MENUBAR_SETTINGS_BALANCE_GIQS:
			sourceSettings.balanceProcedureIdx = BALANCE_GIQS;
			sourceSettings.balanceProcedure = balanceGiqs;
			break;
		case MENUBAR_SETTINGS_BALANCE_GIQS_NO_KELVIN:
			sourceSettings.balanceProcedureIdx = BALANCE_GIQS_NO_KELVIN_ARM;
			sourceSettings.balanceProcedure = balanceGiqsNoKelvinArm;
			break;
		default:
			break;
	}
	UpdatePanelMenuBar(panel);
}

void CVICALLBACK HelpMenuVersicalBridge (int menuBar, int menuItem, void *callbackData, 
											   int panel)
{
	int helpPanel = 0;
	switch (menuItem) {
		case MENUBAR_HELP_VERSICAL_BRIDGE_SCHEMATIC:
			UIERRCHK(helpPanel = LoadPanel(0, panelsFile, PANEL_SCH1));
			UIERRCHK(DisplayPanel(helpPanel));
			break;
		case MENUBAR_HELP_VERSICAL_BRIDGE_BALANCE_SHORTING:
			UIERRCHK(ShowHtmlHelp("Help\\BClient_help.chm", HH_DISPLAY_TOPIC, "VersICaL_balance_shorting.htm"));
			break;
		case MENUBAR_HELP_VERSICAL_BRIDGE_BALANCE_NONSHORTING:
			UIERRCHK(ShowHtmlHelp("Help\\BClient_help.chm", HH_DISPLAY_TOPIC, "VersICaL_balance_nonshorting.htm"));
			break;				
		default:
			break;
	}
}

void CVICALLBACK HelpMenuGiqsBridge (int menuBar, int menuItem, void *callbackData,
									 int panel)
{
	int helpPanel = 0;
	switch (menuItem) {
		case MENUBAR_HELP_GIQS_BRIDGE_SCHEMATIC:
			UIERRCHK(helpPanel = LoadPanel(0, panelsFile, PANEL_SCH2));
			UIERRCHK(DisplayPanel(helpPanel));
			break;
		case MENUBAR_HELP_GIQS_BRIDGE_BALANCE:
			UIERRCHK(ShowHtmlHelp("Help\\BClient_help.chm", HH_DISPLAY_TOPIC, "GIQS_balance.htm"));
			break;
		default:
			break;
	}
}

static char *GetFileNameWithoutExtension(char *filenameWithoutExtension, const char *filename, size_t count) 
{
	strncpy(filenameWithoutExtension, filename, count);
    char *dot = strrchr(filenameWithoutExtension, '.');
    if(dot != NULL)
		*dot = '\0';
    return filenameWithoutExtension;
}
