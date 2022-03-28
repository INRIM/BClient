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
#include <cvirte.h>     
#include <userint.h>
#include <utility.h>

#include "toolbox.h" 

#include "main.h"
#include "msg.h"
#include "cfg.h"
#include "panel.h"
#include "panels.h"

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

//==============================================================================
// Global functions

void InitPanelAttributes(int panel)    
{
	UIERRCHK(SetCtrlAttribute(panel, PANEL_LABEL, ATTR_MAX_ENTRY_LENGTH, LABEL_SZ-1));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_CLOCKFREQUENCY, ATTR_MIN_VALUE, DADSS_CLOCKFREQUENCY_MIN));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_CLOCKFREQUENCY, ATTR_MAX_VALUE, DADSS_CLOCKFREQUENCY_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_FREQUENCY, ATTR_MIN_VALUE, DADSS_FREQUENCY_MIN));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_FREQUENCY, ATTR_MAX_VALUE, DADSS_FREQUENCY_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_MIN_VALUE, DADSS_SAMPLES_MIN));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_MAX_VALUE, DADSS_SAMPLES_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC1_OFFSET, ATTR_MIN_VALUE, DADSS_MDAC1_OFFSET_MIN));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC1_OFFSET, ATTR_MAX_VALUE, DADSS_MDAC1_OFFSET_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_CODE, ATTR_MIN_VALUE, DADSS_MDAC2_CODE_MIN));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_CODE, ATTR_MAX_VALUE, DADSS_MDAC2_CODE_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_VAL, ATTR_MIN_VALUE, DADSS_MDAC2_VALUE_MIN));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_VAL, ATTR_MAX_VALUE, DADSS_MDAC2_VALUE_MAX+DADSS_MDAC2_VALUE_LSB/8));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_AMPLITUDE, ATTR_MIN_VALUE, DADSS_AMPLITUDE_MIN));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_AMPLITUDE, ATTR_MAX_VALUE, DADSS_AMPLITUDE_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE, ATTR_MIN_VALUE, DADSS_PHASE_MIN));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE, ATTR_MAX_VALUE, DADSS_PHASE_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL, ATTR_MIN_VALUE, -DADSS_AMPLITUDE_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL, ATTR_MAX_VALUE, DADSS_AMPLITUDE_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_IMAG, ATTR_MIN_VALUE, -DADSS_AMPLITUDE_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_IMAG, ATTR_MAX_VALUE, DADSS_AMPLITUDE_MAX));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_MIN_VALUE, 0.0));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_MIN_VALUE, 0.0));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_RANGE, ATTR_TOOLTIP_TEXT, tooltipStrings[TOOLTIP_RANGE]));
	for (int i = 1; i < sourceSettings.nModes; ++i)
		UIERRCHK(InsertListItem(panel, PANEL_ACTIVE_MODE, -1, modeSettings[i].label, i));
	UIERRCHK(SetCtrlVal(panel, PANEL_ACTIVE_MODE, sourceSettings.activeMode));
	UIERRCHK(SetCtrlAttribute(panel, PANEL_RECORD_TIMER, ATTR_ENABLED, 0));
	for (int i = BALANCE_CHANNEL; i <= BALANCE_FULL_ABBA; ++i) 
		UIERRCHK(InsertListItem(panel, PANEL_BALANCE_TARGET, -1, balanceTargetStrings[i], i));
}

void UpdatePanel(int panel)
{	
	switch (programState) {
		case STATE_IDLE:
		case STATE_CONNECTED:
		case STATE_RUNNING:
			UIERRCHK(SetWaitCursor(0));
			UIERRCHK(SetPanelAttribute(panel, ATTR_DIMMED, 0));
			break;
		case STATE_UPDATING:
		case STATE_BALANCING:
		case STATE_RECORDING:
			UIERRCHK(SetWaitCursor(1));
			UIERRCHK(SetPanelAttribute(panel, ATTR_DIMMED, 1));
			break;
	}
	
	UIERRCHK(ProcessDrawEvents());
	
	UpdatePanelMenuBar(panel);
	UpdatePanelSource(panel);
	UpdatePanelActiveMode(panel);
	UpdatePanelActiveChannel(panel);
	UpdatePanelSwitchActiveMode(panel);
	UpdatePanelLockin(panel);
	UpdatePanelBalance(panel);
	UpdatePanelTitle(panel);
}

void UpdatePanelTitle(int panel)
{
	char buf[TITLE_BUF_SZ];
	
	if (sourceSettings.dataFileHandle == NULL)
		snprintf(buf, TITLE_BUF_SZ, "%s %s", msgStrings[MSG_TITLE], msgStrings[MSG_VERSION]);
	else {
		char dataDriveName[MAX_DRIVENAME_LEN];
		char dataDirName[MAX_DIRNAME_LEN];
		char dataFileName[MAX_FILENAME_LEN];
		SplitPath(sourceSettings.dataPathName, dataDriveName, dataDirName, dataFileName);
		snprintf(buf, TITLE_BUF_SZ, "%s %s - %s%c", msgStrings[MSG_TITLE], msgStrings[MSG_VERSION], dataFileName, sourceSettings.isFileModified ? '*' : ' ');
	}
	UIERRCHK(SetPanelAttribute(panel, ATTR_TITLE, buf));
	
	UIERRCHK(ProcessDrawEvents());
}

void UpdatePanelMenuBar(int panel)
{
	int menuBar = GetPanelMenuBar(panel);
	if (menuBar <= 0)
		die(msgStrings[MSG_INTERNAL_ERROR]);
	
	UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_SAVE, ATTR_DIMMED, 0)); 
	
	if (lockinSettings.isPresent) {
		UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_RECORD, ATTR_DIMMED, 0));
		UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE, ATTR_DIMMED, 0));
	} else {
		UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_RECORD, ATTR_DIMMED, 1));
		UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE, ATTR_DIMMED, 1));
	}
	
	switch (sourceSettings.balanceProcedureIdx) {
		case BALANCE_TWO_PORT:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_TWO_PORT, ATTR_CHECKED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL_NO_KELVIN, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS_NO_KELVIN, ATTR_CHECKED, 0));
			break;
		case BALANCE_VERSICAL:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_TWO_PORT, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL, ATTR_CHECKED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL_NO_KELVIN, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS_NO_KELVIN, ATTR_CHECKED, 0));
			break;
		case BALANCE_VERSICAL_NO_KELVIN_ARM:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_TWO_PORT, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL_NO_KELVIN, ATTR_CHECKED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS_NO_KELVIN, ATTR_CHECKED, 0));
			break;
		case BALANCE_GIQS:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_TWO_PORT, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL_NO_KELVIN, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS, ATTR_CHECKED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS_NO_KELVIN, ATTR_CHECKED, 0));
			break;
		case BALANCE_GIQS_NO_KELVIN_ARM:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_TWO_PORT, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_VERSICAL_NO_KELVIN, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS, ATTR_CHECKED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_BALANCE_GIQS_NO_KELVIN, ATTR_CHECKED, 1));
			break;
	}

	if (sourceSettings.dataFileHandle == NULL) {
		UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE_NEW, ATTR_DIMMED, 0));
		UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE_CLOSE, ATTR_DIMMED, 1));
	} else {
		UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE_NEW, ATTR_DIMMED, 1));
		UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE_CLOSE, ATTR_DIMMED, 0));
	}
	
	switch (programState) {
		case STATE_IDLE:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE_SAVE, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_CONNECTION, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_MODES, ATTR_DIMMED, 0));
			if (switchSettings.isPresent) {
				UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_CHANNEL_SWITCH, ATTR_DIMMED, 0));
			} else {
				UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_CHANNEL_SWITCH, ATTR_DIMMED, 1));
			}
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_LOAD, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_RESET, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_PRESET, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_HELP, ATTR_DIMMED, 0));
			break;
	  	case STATE_CONNECTED:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE_SAVE, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_CONNECTION, ATTR_DIMMED, 1)); 
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_MODES, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_CHANNEL_SWITCH, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_LOAD, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_RESET, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_PRESET, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_HELP, ATTR_DIMMED, 0));
			break;
		case STATE_RUNNING:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE_SAVE, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS, ATTR_DIMMED, 0));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_CONNECTION, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_MODES, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_CHANNEL_SWITCH, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_LOAD, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_RESET, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS_PRESET, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_HELP, ATTR_DIMMED, 0));
			break;
		case STATE_UPDATING:
		case STATE_BALANCING:
		case STATE_RECORDING:
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_FILE, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_SETTINGS, ATTR_DIMMED, 1));
			UIERRCHK(SetMenuBarAttribute(menuBar, MENUBAR_HELP, ATTR_DIMMED, 1));
			break;
	}
	
	UIERRCHK(ProcessDrawEvents());
}

void UpdatePanelSource(int panel)
{
	UIERRCHK(SetCtrlVal(panel, PANEL_CLOCKFREQUENCY, sourceSettings.clockFrequency));
	UIERRCHK(SetCtrlVal(panel, PANEL_FREQUENCY, sourceSettings.frequency));
	UIERRCHK(SetCtrlVal(panel, PANEL_REAL_FREQUENCY, sourceSettings.realFrequency)); 
	UIERRCHK(SetCtrlVal(panel, PANEL_SAMPLES, sourceSettings.nSamples)); 
	
	switch (programState) {
		case STATE_IDLE:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_CONNECT, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlVal(panel, PANEL_CONNECT_LED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_CLOCKFREQUENCY, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_FREQUENCY, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL_FREQUENCY, ATTR_TEXT_STRIKEOUT, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_REAL_FREQUENCY, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_TEXT_STRIKEOUT, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_START_STOP, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlVal(panel, PANEL_START_STOP_LED, 0));
			break;
	  	case STATE_CONNECTED:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_CONNECT, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlVal(panel, PANEL_CONNECT_LED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_CLOCKFREQUENCY, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_FREQUENCY, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL_FREQUENCY, ATTR_TEXT_STRIKEOUT, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_REAL_FREQUENCY, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_TEXT_STRIKEOUT, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_START_STOP, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlVal(panel, PANEL_START_STOP_LED, 0));
			break;
		case STATE_RUNNING:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_CONNECT, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlVal(panel, PANEL_CONNECT_LED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_CLOCKFREQUENCY, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_FREQUENCY, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL_FREQUENCY, ATTR_TEXT_STRIKEOUT, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_REAL_FREQUENCY, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_TEXT_STRIKEOUT, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_START_STOP, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlVal(panel, PANEL_START_STOP_LED, 1));
			break;
		case STATE_UPDATING:
		case STATE_BALANCING:
		case STATE_RECORDING:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_CONNECT, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_CLOCKFREQUENCY, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_FREQUENCY, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL_FREQUENCY, ATTR_TEXT_STRIKEOUT, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_REAL_FREQUENCY, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SAMPLES, ATTR_TEXT_STRIKEOUT, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_START_STOP, ATTR_DIMMED, 0));
			break;
	}
	
	UIERRCHK(ProcessDrawEvents());
}

void UpdatePanelActiveMode(int panel)
{
	switch (programState) {
		case STATE_IDLE:
		case STATE_UPDATING:
		case STATE_BALANCING:
		case STATE_RECORDING:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_MODE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_UPDATE_MODE, ATTR_DIMMED, 1));
			break;
	  	case STATE_CONNECTED:
		case STATE_RUNNING:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_MODE, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_UPDATE_MODE, ATTR_DIMMED, 0));
			break;
	}
	
	UIERRCHK(ClearListCtrl(panel, PANEL_ACTIVE_MODE));
	for (int i = 1; i < sourceSettings.nModes; ++i)
		UIERRCHK(InsertListItem(panel, PANEL_ACTIVE_MODE, -1, modeSettings[i].label, i));
	UIERRCHK(SetCtrlVal(panel, PANEL_ACTIVE_MODE, sourceSettings.activeMode));
	
	UIERRCHK(ProcessDrawEvents());
}

void UpdatePanelActiveChannel(int panel)
{
	switch (programState) {
		case STATE_IDLE:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_CHANNEL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SWAP_ACTIVE_CHANNEL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_LABEL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_IS_LOCKED, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_RANGE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_AMPLITUDE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PIHALF, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PI, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PIHALF, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PI, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_IMAG, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC1_OFFSET, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_CODE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_VAL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_PHASOR, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_PHASOR_1, ATTR_DIMMED, 1));     
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PASTE_PHASOR, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_SAMPLES, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_OUT_OF_RANGE_LED, ATTR_DIMMED, 1));
			break;
		case STATE_CONNECTED:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_IS_LOCKED, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_CHANNEL, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_PHASOR, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_PHASOR_1, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_SAMPLES, ATTR_DIMMED, 0));
			if (!modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked) {
				UIERRCHK(SetCtrlAttribute(panel, PANEL_SWAP_ACTIVE_CHANNEL, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LABEL, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_RANGE, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AMPLITUDE, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PIHALF, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PI, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PIHALF, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PI, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_IMAG, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC1_OFFSET, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_CODE, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_VAL, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PASTE_PHASOR, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_OUT_OF_RANGE_LED, ATTR_DIMMED, 0));
			} else {
				UIERRCHK(SetCtrlAttribute(panel, PANEL_SWAP_ACTIVE_CHANNEL, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LABEL, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_RANGE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AMPLITUDE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PIHALF, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PI, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PIHALF, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PI, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_IMAG, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC1_OFFSET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_CODE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_VAL, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PASTE_PHASOR, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_OUT_OF_RANGE_LED, ATTR_DIMMED, 1));
			}
			break;
		case STATE_RUNNING:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_IS_LOCKED, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_CHANNEL, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_PHASOR, ATTR_DIMMED, 0));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_PHASOR_1, ATTR_DIMMED, 0));  
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_SAMPLES, ATTR_DIMMED, 0)); 
			UIERRCHK(SetCtrlAttribute(panel, PANEL_RANGE, ATTR_DIMMED, 1)); 
			if (!modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked) {
				UIERRCHK(SetCtrlAttribute(panel, PANEL_SWAP_ACTIVE_CHANNEL, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LABEL, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AMPLITUDE, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PIHALF, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PI, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PIHALF, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PI, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_IMAG, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC1_OFFSET, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_CODE, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_VAL, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PASTE_PHASOR, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_OUT_OF_RANGE_LED, ATTR_DIMMED, 0));
			} else {
				UIERRCHK(SetCtrlAttribute(panel, PANEL_SWAP_ACTIVE_CHANNEL, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LABEL, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AMPLITUDE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PIHALF, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PI, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PIHALF, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PI, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_IMAG, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC1_OFFSET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_CODE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_VAL, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_PASTE_PHASOR, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_OUT_OF_RANGE_LED, ATTR_DIMMED, 1));
			}
			break;
		case STATE_UPDATING:
		case STATE_BALANCING:
		case STATE_RECORDING:
			UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_CHANNEL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_SWAP_ACTIVE_CHANNEL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_LABEL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_IS_LOCKED, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_RANGE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_AMPLITUDE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PIHALF, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_ADD_PI, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PIHALF, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PHASE_SUBTRACT_PI, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_REAL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_IMAG, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC1_OFFSET, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_CODE, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_MDAC2_VAL, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_PHASOR, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_PHASOR_1, ATTR_DIMMED, 1));     
			UIERRCHK(SetCtrlAttribute(panel, PANEL_PASTE_PHASOR, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_COPY_SAMPLES, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_OUT_OF_RANGE_LED, ATTR_DIMMED, 1));
			break;
	}
	
	UIERRCHK(SetCtrlVal(panel, PANEL_ACTIVE_CHANNEL, sourceSettings.activeChannel));
	UIERRCHK(SetCtrlVal(panel, PANEL_LABEL, sourceSettings.label[sourceSettings.activeChannel]));
	UIERRCHK(SetCtrlVal(panel, PANEL_IS_LOCKED, modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked));
	UIERRCHK(SetCtrlVal(panel, PANEL_RANGE, sourceSettings.range[sourceSettings.activeChannel]));
	
	UIERRCHK(ProcessDrawEvents());  
	
	UpdatePanelWaveformParameters(panel);
}

void UpdatePanelWaveformParameters(int panel)
{
	UIERRCHK(SetCtrlVal(panel, PANEL_AMPLITUDE, modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude));
	UIERRCHK(SetCtrlVal(panel, PANEL_PHASE, modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
	UIERRCHK(SetCtrlVal(panel, PANEL_REAL, modeSettings[0].channelSettings[sourceSettings.activeChannel].real));
	UIERRCHK(SetCtrlVal(panel, PANEL_IMAG, modeSettings[0].channelSettings[sourceSettings.activeChannel].imag));
	UIERRCHK(SetCtrlVal(panel, PANEL_MDAC1_OFFSET, modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac1Offset));
	UIERRCHK(SetCtrlVal(panel, PANEL_MDAC2_CODE, modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Code));
	UIERRCHK(SetCtrlVal(panel, PANEL_MDAC2_VAL, modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Val));
	
	UIERRCHK(ProcessDrawEvents());
}

void UpdatePanelLockinReading(int panel, double x, double y) 
{
	UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_X, x));
	UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_Y, y));
	UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_R, sqrt(x*x+y*y)));
	
	UIERRCHK(ProcessDrawEvents());
}

void UpdatePanelLockin(int panel)
{
	UIERRCHK(SetTableCellValFromIndex(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_GAIN),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].gainMode));
	UIERRCHK(SetTableCellValFromIndex(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_SENSITIVITY),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.sensitivity));
	UIERRCHK(SetTableCellValFromIndex(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_TIME_CONSTANT),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.timeConstant));
	UIERRCHK(SetTableCellValFromIndex(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_FILTER_SLOPE),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.filterSlope));
	UIERRCHK(SetTableCellValFromIndex(panel,PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_INPUT_CONFIGURATION),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputConfiguration));
	UIERRCHK(SetTableCellValFromIndex(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_INPUT_COUPLING),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputCoupling));
	UIERRCHK(SetTableCellValFromIndex(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_INPUT_SHIELD_GROUNDING),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputShieldGrounding));
	UIERRCHK(SetTableCellValFromIndex(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_INPUT_RESERVE),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputReserve));
	UIERRCHK(SetTableCellValFromIndex(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, LIANV_INPUT_LINE_NOTCH_FILTER),
									  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputLineNotchFilter));
	
	if (!lockinSettings.isPresent || modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked) {
			UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_PARAMETERS, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_X, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_Y, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_R, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_READ, ATTR_DIMMED, 1));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_RECORD, ATTR_DIMMED, 1));
	} else  {
		switch (programState)
		{
			case STATE_IDLE:
			case STATE_UPDATING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_PARAMETERS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_X, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_Y, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_R, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_READ, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_RECORD, ATTR_DIMMED, 1));
				break;
			case STATE_CONNECTED:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_PARAMETERS, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_X, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_Y, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_R, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_READ, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_RECORD, ATTR_DIMMED, 1));
				break;
			case STATE_RUNNING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_PARAMETERS, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_X, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_Y, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_R, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_READ, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_RECORD, ATTR_DIMMED, 0));
				break;
			case STATE_BALANCING:
			case STATE_RECORDING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_PARAMETERS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_X, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_Y, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_R, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_READ, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_LOCKIN_RECORD, ATTR_DIMMED, 1));
				break;
		}
	} 
	
	UIERRCHK(ProcessDrawEvents());
}

void UpdatePanelBalance(int panel)
{
	UIERRCHK(SetCtrlVal(panel, PANEL_BALANCE_THRESHOLD, 
						modeSettings[0].channelSettings[sourceSettings.activeChannel].balanceThreshold));
	UIERRCHK(SetCtrlVal(panel, PANEL_CHECK_THRESHOLD, 
						modeSettings[0].channelSettings[sourceSettings.activeChannel].checkThreshold));
	UIERRCHK(SetCtrlVal(panel, PANEL_BALANCE_TARGET, sourceSettings.balanceTarget));
	
	if (!lockinSettings.isPresent || modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked) {
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
	} else if (!switchSettings.isPresent ) {
		switch (programState)
		{
			case STATE_IDLE:
			case STATE_UPDATING:
			case STATE_RECORDING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
				break;
			case STATE_CONNECTED:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
				break;
			case STATE_RUNNING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
				break;
			case STATE_BALANCING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
				break;
		}
	} else {
		switch (programState)
		{
			case STATE_IDLE:
			case STATE_UPDATING:
			case STATE_RECORDING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
				break;
			case STATE_CONNECTED:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
				break;
			case STATE_RUNNING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 0));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 0));
				if (sourceSettings.balanceTarget == BALANCE_CHANNEL || sourceSettings.balanceTarget == BALANCE_MODE) {
					UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
					UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
				} else {
					UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 0));
					UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 0));
				}
				break;
			case STATE_BALANCING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CHECK_THRESHOLD, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_TARGET, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_AUTOZERO_LED, ATTR_DIMMED, 0));
				UIERRCHK(SetCtrlVal(panel, PANEL_AUTOZERO_LED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_BALANCE_REPETITIONS, ATTR_DIMMED, 1));
				UIERRCHK(SetCtrlAttribute(panel, PANEL_CURRENT_REPETITION, ATTR_DIMMED, 1));
				break;
		}
	}
	
	UIERRCHK(ProcessDrawEvents());
}

void UpdatePanelSwitchActiveMode(int panel)
{
	if (!switchSettings.isPresent || modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked) {
		UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_SWITCH_MODE, ATTR_DIMMED, 1));
	} else {
		switch (programState) {
			case STATE_IDLE:
			case STATE_UPDATING:
			case STATE_BALANCING:
			case STATE_RECORDING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_SWITCH_MODE, ATTR_DIMMED, 1));
				break;
		  	case STATE_CONNECTED:
			case STATE_RUNNING:
				UIERRCHK(SetCtrlAttribute(panel, PANEL_ACTIVE_SWITCH_MODE, ATTR_DIMMED, 0));
				break;
		}
	}
	
	UIERRCHK(ClearListCtrl(panel, PANEL_ACTIVE_SWITCH_MODE));
	for (int i = 0; i < sourceSettings.nChannelSwitchModes; ++i)
		UIERRCHK(InsertListItem(panel, PANEL_ACTIVE_SWITCH_MODE, -1, channelSwitchSettings[i].label, i));
	UIERRCHK(SetCtrlVal(panel, PANEL_ACTIVE_SWITCH_MODE, modeSettings[0].channelSettings[sourceSettings.activeChannel].activeSwitchMode));
	
	UIERRCHK(ProcessDrawEvents());
}

