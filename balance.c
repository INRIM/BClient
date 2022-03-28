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
#include <windows.h>
#include <advanlys.h>

#include "DA_DSS_cvi_driver.h"
#include "LIA_NV_cvi_driver.h"
#include "main.h"
#include "cfg.h"
#include "msg.h"
#include "panel.h"
#include "panels.h"
#include "balance.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

static void InitBalanceTwoPort(void);
static void InitBalanceVersical(void);
static void InitBalanceVersicalNoKelvinArm(void);
static void InitBalanceGiqs(void);
static void InitBalanceGiqsNoKelvinArm(void);
static void BalanceChannel(int);
static void BalanceMode(int, ListType);
static void BalanceDoubleMode(int, ListType);
static void BalanceFullAB(int, ListType);
static void BalanceFullABBA(int, ListType);

//==============================================================================
// Global variables

ListType balanceTwoPort;
ListType balanceVersical;
ListType balanceVersicalNoKelvinArm;
ListType balanceGiqs;
ListType balanceGiqsNoKelvinArm;

//==============================================================================
// Global functions

void InitBalanceProcedures(void)
{
	InitBalanceTwoPort();
	InitBalanceVersical();
	InitBalanceVersicalNoKelvinArm();
	InitBalanceGiqs();
	InitBalanceGiqsNoKelvinArm();
}

void InitBalanceTwoPort(void)
{
	balanceTwoPort = ListCreate(sizeof(int));
	if (balanceTwoPort == 0 || ListInsertItem(balanceTwoPort, &(int){VOLTAGE_CHANNEL_B}, END_OF_LIST) == 0) 
		die("%s", msgStrings[MSG_OUT_OF_MEMORY]);
}

static void InitBalanceVersical(void)
{
	balanceVersical = ListCreate(sizeof(int));
	if (balanceVersical == 0 ||	ListInsertItem(balanceVersical, &(int){CURRENT_CHANNEL_A}, END_OF_LIST) == 0 ||
			ListInsertItem(balanceVersical, &(int){VOLTAGE_CHANNEL_B}, END_OF_LIST) == 0 ||
					ListInsertItem(balanceVersical, &(int){CURRENT_CHANNEL_B}, END_OF_LIST) == 0 ||
							ListInsertItem(balanceVersical, &(int){KELVIN_ARM_CHANNEL}, END_OF_LIST) == 0)
		die("%s", msgStrings[MSG_OUT_OF_MEMORY]);
}

static void InitBalanceVersicalNoKelvinArm(void)
{
	balanceVersicalNoKelvinArm = ListCreate(sizeof(int));
	if (balanceVersicalNoKelvinArm == 0 || ListInsertItem(balanceVersicalNoKelvinArm, &(int){CURRENT_CHANNEL_A}, END_OF_LIST) == 0 ||
			ListInsertItem(balanceVersicalNoKelvinArm, &(int){VOLTAGE_CHANNEL_B}, END_OF_LIST) == 0 ||
					ListInsertItem(balanceVersicalNoKelvinArm, &(int){CURRENT_CHANNEL_B}, END_OF_LIST) == 0)
		die("%s", msgStrings[MSG_OUT_OF_MEMORY]);
}

static void InitBalanceGiqs(void)
{
	balanceGiqs = ListCreate(sizeof(int));
	if (balanceGiqs == 0 ||	ListInsertItem(balanceGiqs, &(int){CURRENT_CHANNEL_A}, END_OF_LIST) == 0 ||
			ListInsertItem(balanceGiqs, &(int){CURRENT_CHANNEL_B}, END_OF_LIST) == 0 ||
					ListInsertItem(balanceGiqs, &(int){KELVIN_ARM_CHANNEL}, END_OF_LIST) == 0 ||
							ListInsertItem(balanceGiqs, &(int){INJECTION_CHANNEL}, END_OF_LIST) == 0)
		die("%s", msgStrings[MSG_OUT_OF_MEMORY]);
}

static void InitBalanceGiqsNoKelvinArm(void)
{
	balanceGiqsNoKelvinArm = ListCreate(sizeof(int));
	if (balanceGiqsNoKelvinArm == 0 || ListInsertItem(balanceGiqsNoKelvinArm, &(int){CURRENT_CHANNEL_A}, END_OF_LIST) == 0 ||
			ListInsertItem(balanceGiqsNoKelvinArm, &(int){CURRENT_CHANNEL_B}, END_OF_LIST) == 0 ||
					ListInsertItem(balanceGiqsNoKelvinArm, &(int){INJECTION_CHANNEL}, END_OF_LIST) == 0)
		die("%s", msgStrings[MSG_OUT_OF_MEMORY]);
}

int CVICALLBACK Balance (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	ProgramState savedProgramState = programState;
	
	switch (event)
	{
		case EVENT_COMMIT:
			programState = STATE_BALANCING;
			UpdatePanel(panel);
			switch (sourceSettings.balanceTarget)
			{
				case BALANCE_CHANNEL:
					BalanceChannel(panel);
					break;
				case BALANCE_MODE:
					BalanceMode(panel,sourceSettings.balanceProcedure);
					break;
				case BALANCE_DOUBLE_MODE:
					BalanceDoubleMode(panel,sourceSettings.balanceProcedure);
					break;
				case BALANCE_FULL_AB:
					BalanceFullAB(panel, sourceSettings.balanceProcedure);
					break;
				case BALANCE_FULL_ABBA:
					BalanceFullABBA(panel, sourceSettings.balanceProcedure);
					break;
			}
			programState = savedProgramState;
			UpdatePanel(panel);
			break;
		default:
			break;
	} 
	
	return 0;
}

static void BalanceChannel(int panel)
{
	int azPanel;
	double amplitude, phase;
	struct {
		double real;
		double imag;
	} stimulus[MAX_AUTOZERO_STEPS] = {{0}}, response[MAX_AUTOZERO_STEPS] = {{0}},
	deltaStimulus[MAX_AUTOZERO_STEPS-2] = {{0}}, deltaResponse[MAX_AUTOZERO_STEPS-2] = {{0}},
	sensitivity[MAX_AUTOZERO_STEPS-2] = {{0}}, stimulusCorrection[MAX_AUTOZERO_STEPS-2]= {{0}};

	ProgramState savedProgramState = programState;
	
	lockinSettings.isBalanceInterrupted = 0;
	programState = STATE_BALANCING;
	UpdatePanel(panel);
	UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 0));
	
	double graphRange = AUTOZERO_GRAPH_RANGE_FACTOR*modeSettings[0].channelSettings[sourceSettings.activeChannel].balanceThreshold;
	UIERRCHK(azPanel = LoadPanel(panel, panelsFile, PANEL_AZ));
	UIERRCHK(SetAxisScalingMode(azPanel, PANEL_AZ_LOCKIN_READINGS, VAL_BOTTOM_XAXIS, VAL_MANUAL, 
								-graphRange, graphRange));
	UIERRCHK(SetAxisScalingMode(azPanel, PANEL_AZ_LOCKIN_READINGS, VAL_LEFT_YAXIS, VAL_MANUAL,
								-graphRange, graphRange));
	UIERRCHK(SetCtrlVal(azPanel, PANEL_AZ_X_MIN, -graphRange));
	UIERRCHK(SetCtrlVal(azPanel, PANEL_AZ_X_MAX, graphRange));
	UIERRCHK(SetCtrlVal(azPanel, PANEL_AZ_Y_MIN, -graphRange));
	UIERRCHK(SetCtrlVal(azPanel, PANEL_AZ_Y_MAX, graphRange));
	UIERRCHK(DisplayPanel(azPanel));
	
	double maxAmplitude;
	DSSERRCHK(DADSS_GetAmplitudeMax(sourceSettings.activeChannel+1, &maxAmplitude));
	
	if (modeSettings[0].channelSettings[sourceSettings.activeChannel].gainMode == LIANV_GAIN_AUTO) {
		LIAERRCHK(LIANV_SetInputParameters(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings));
	} else {
		LIAERRCHK(LIANV_SetInputParametersNoSensitivity(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings));
	}

	// First data point of the balance algorithm
	stimulus[0].real = modeSettings[0].channelSettings[sourceSettings.activeChannel].real; 
	stimulus[0].imag = modeSettings[0].channelSettings[sourceSettings.activeChannel].imag;
	if (modeSettings[0].channelSettings[sourceSettings.activeChannel].gainMode == LIANV_GAIN_AUTO) {
		UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 1));
		LIAERRCHK(LIANV_GetMeasureXYWithAutoGain(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading));
		UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 0));
	} else {
		LIAERRCHK(LIANV_GetMeasureXY(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading));
	}
	UpdatePanelLockinReading(panel, 
							 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
							 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1]);
	UpdatePanelLockin(panel);
	UIERRCHK(PlotPoint(azPanel, PANEL_AZ_LOCKIN_READINGS, 
					   modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
					   modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1], 
					   VAL_SOLID_CIRCLE, VAL_RED));
	response[0].real = modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0];
	response[0].imag = modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1];

	// Randomly update the stimulus for the second point
	stimulus[1].real = stimulus[0].real+maxAmplitude*Random(-1e-3,1e-3);
	stimulus[1].imag = stimulus[0].imag+maxAmplitude*Random(-1e-3,1e-3);

	ToPolar(stimulus[1].real, stimulus[1].imag, &amplitude, &phase);
	if (amplitude > maxAmplitude) {
		PlaySound("Media/overrange.wav", NULL, SND_FILENAME);
		UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 1));
		UIERRCHK(DiscardPanel(azPanel));
		programState = savedProgramState;
		UpdatePanel(panel);
		lockinSettings.isBalanceInterrupted = 1;
		return; 
	}
	
	DSSERRCHK(DADSS_SetWaveformParametersPolar(sourceSettings.activeChannel+1, amplitude, phase));
	DSSERRCHK(DADSS_UpdateWaveform());
	Delay(DADSS_ADJ_DELAY);
	DSSERRCHK(DADSS_GetWaveformParametersPolar(sourceSettings.activeChannel+1,
					&modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude, 
					&modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
	ToRect(modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude, 
		   modeSettings[0].channelSettings[sourceSettings.activeChannel].phase, 
		   &modeSettings[0].channelSettings[sourceSettings.activeChannel].real, 
		   &modeSettings[0].channelSettings[sourceSettings.activeChannel].imag);
	UpdatePanelWaveformParameters(panel);
	stimulus[1].real = modeSettings[0].channelSettings[sourceSettings.activeChannel].real;
	stimulus[1].imag = modeSettings[0].channelSettings[sourceSettings.activeChannel].imag;
	LIAERRCHK(LIANV_GetTimeConstant(lockinSettings.nvProcessName, &modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.timeConstant));
	DelayWithEventProcessing(AUTOZERO_ADJ_DELAY_FACTOR*LIANV_TimeConstantValues[modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.timeConstant] 
							 + AUTOZERO_ADJ_DELAY_BASE);
	
	if (modeSettings[0].channelSettings[sourceSettings.activeChannel].gainMode == LIANV_GAIN_AUTO) {
		UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 1));
		LIAERRCHK(LIANV_GetMeasureXYWithAutoGain(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading));
		UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 0));
	} else {
		LIAERRCHK(LIANV_GetMeasureXY(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading));
	}
	UpdatePanelLockinReading(panel, 
							 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
							 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1]);
	UpdatePanelLockin(panel);
	UIERRCHK(PlotPoint(azPanel, PANEL_AZ_LOCKIN_READINGS, 
					   modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
					   modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1], 
					   VAL_SOLID_CIRCLE, VAL_RED));
	response[1].real = modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0];
	response[1].imag = modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1];

	int eventHandle, ctrlHandle;
	UIERRCHK(GetUserEvent(0, &eventHandle, &ctrlHandle));
	switch (ctrlHandle) {
		case PANEL_AZ_INTERRUPT:
			lockinSettings.isBalanceInterrupted = 1;
			break;
		default:
			break;
	}
	
	// Start the equilibrium procedure
	int k = 2;
	for ( ; k < MAX_AUTOZERO_STEPS && 
			sqrt(response[k-1].real*response[k-1].real +
				 response[k-1].imag*response[k-1].imag) > modeSettings[0].channelSettings[sourceSettings.activeChannel].balanceThreshold &&
			!lockinSettings.isBalanceInterrupted;
			++k) {
		// Update the source
		CxSub(stimulus[k-1].real, stimulus[k-1].imag, 
			  stimulus[k-2].real, stimulus[k-2].imag, 
			  &deltaStimulus[k-2].real, &deltaStimulus[k-2].imag);
		CxSub(response[k-1].real, response[k-1].imag, 
			  response[k-2].real, response[k-2].imag, 
			  &deltaResponse[k-2].real, &deltaResponse[k-2].imag); 	
		CxDiv(deltaStimulus[k-2].real, deltaStimulus[k-2].imag, 
			  deltaResponse[k-2].real, deltaResponse[k-2].imag,
			  &sensitivity[k-2].real, &sensitivity[k-2].imag);
		CxMul(sensitivity[k-2].real, sensitivity[k-2].imag,
			  response[k-1].real, response[k-1].imag, 
			  &stimulusCorrection[k-2].real, &stimulusCorrection[k-2].imag);
		CxSub(stimulus[k-1].real, stimulus[k-1].imag, 
			  stimulusCorrection[k-2].real, stimulusCorrection[k-2].imag, 
			  &stimulus[k].real, &stimulus[k].imag); 
	
		ToPolar(stimulus[k].real, stimulus[k].imag, &amplitude, &phase);
		if (amplitude > maxAmplitude) {
			PlaySound("Media/overrange.wav", NULL, SND_FILENAME);
			UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 1));
			lockinSettings.isBalanceInterrupted = 1;
			break; 
		}
	
		DSSERRCHK(DADSS_SetWaveformParametersPolar(sourceSettings.activeChannel+1, amplitude, phase));
		DSSERRCHK(DADSS_UpdateWaveform());
		Delay(DADSS_ADJ_DELAY);
		DSSERRCHK(DADSS_GetWaveformParametersPolar(sourceSettings.activeChannel+1, 
				  &modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude, 
				  &modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
		ToRect(modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude, 
			   modeSettings[0].channelSettings[sourceSettings.activeChannel].phase, 
			   &modeSettings[0].channelSettings[sourceSettings.activeChannel].real, 
			   &modeSettings[0].channelSettings[sourceSettings.activeChannel].imag);
		UpdatePanelWaveformParameters(panel);
		stimulus[k].real = modeSettings[0].channelSettings[sourceSettings.activeChannel].real;
		stimulus[k].imag = modeSettings[0].channelSettings[sourceSettings.activeChannel].imag;
		LIAERRCHK(LIANV_GetTimeConstant(lockinSettings.nvProcessName, &modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.timeConstant));
		DelayWithEventProcessing(AUTOZERO_ADJ_DELAY_FACTOR*LIANV_TimeConstantValues[modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.timeConstant] 
								 + AUTOZERO_ADJ_DELAY_BASE);

		if (modeSettings[0].channelSettings[sourceSettings.activeChannel].gainMode == LIANV_GAIN_AUTO) {
			UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 1));
			LIAERRCHK(LIANV_GetMeasureXYWithAutoGain(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading));
			UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 0));
		} else {
			LIAERRCHK(LIANV_GetMeasureXY(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading));
		}
		UpdatePanelLockinReading(panel, 
								 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
								 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1]);
		UpdatePanelLockin(panel);
		UIERRCHK(PlotPoint(azPanel, PANEL_AZ_LOCKIN_READINGS, 
						   modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
						   modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1], 
						   VAL_SOLID_CIRCLE, VAL_RED));
		response[k].real = modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0];
		response[k].imag = modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1];
		
		UIERRCHK(GetUserEvent(0, &eventHandle, &ctrlHandle));
		switch (ctrlHandle) {
			case PANEL_AZ_INTERRUPT:
				lockinSettings.isBalanceInterrupted = 1;
				break;
			default:
				break;
		}
	}
	if (!lockinSettings.isBalanceInterrupted)
		Beep();
	LIAERRCHK(LIANV_GetInputParameters(lockinSettings.nvProcessName, &modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings));
	UIERRCHK(DiscardPanel(azPanel));
	UIERRCHK(ProcessDrawEvents());
	UIERRCHK(ProcessSystemEvents());
	if (k == MAX_AUTOZERO_STEPS) {
		lockinSettings.isBalanceInterrupted = 1;
		Beep();
		warn("%s", msgStrings[MSG_MAX_AUTOZERO_STEPS]);
	}
	programState = savedProgramState;
	sourceSettings.isFileModified = 1;
	UpdatePanel(panel);

	return;
	
Error:
	UIERRCHK(DiscardPanel(azPanel));
	lockinSettings.isBalanceInterrupted = 1;
	UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 0));
	programState = savedProgramState;
	LIANV_GetInputParameters(lockinSettings.nvProcessName, &modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings);
	sourceSettings.isFileModified = 1;
	UpdatePanel(panel);
	return;
}

static void BalanceMode(int panel, ListType balanceProcedure)
{
	int menuBar = GetPanelMenuBar(panel);
	if (menuBar <= 0)
		die(msgStrings[MSG_INTERNAL_ERROR]);
	
	size_t balanceSteps = ListNumItems(balanceProcedure);
	if (balanceSteps == 0)
		die(msgStrings[MSG_INTERNAL_ERROR]);
	
	int isBalanced = 1;
	lockinSettings.isBalanceInterrupted = 0;
	for (size_t i = 1; i <= balanceSteps; ++i) {
		int bridgeChannel;
		ListGetItem(balanceProcedure, &bridgeChannel, i);
		UIERRCHK(SetCtrlVal(panel, PANEL_ACTIVE_CHANNEL, modeSettings[0].bridgeSettings.channelAssignment[bridgeChannel]));
		UIERRCHK(ProcessDrawEvents());
		SetActiveChannel(panel, PANEL_ACTIVE_CHANNEL, EVENT_COMMIT, NULL, 0, 0);
		if (lockinSettings.isBalanceInterrupted) {
			return;
		}
		BalanceChannel(panel);
		if (lockinSettings.isBalanceInterrupted) {
			return;
		}	
	}
	
	if (balanceSteps > 1) {
		int balanceIteration = 1;
		do {
			isBalanced = 1;
			for (size_t i = 1; i <= balanceSteps; ++i) {
				int bridgeChannel;
				ListGetItem(balanceProcedure, &bridgeChannel, i);
				UIERRCHK(SetCtrlVal(panel, PANEL_ACTIVE_CHANNEL, modeSettings[0].bridgeSettings.channelAssignment[bridgeChannel]));
				UIERRCHK(ProcessDrawEvents());
				SetActiveChannel(panel, PANEL_ACTIVE_CHANNEL, EVENT_COMMIT, NULL, 0, 0);
				if (lockinSettings.isBalanceInterrupted) {
					return;
				}

				Delay(LIANV_SettlingTimeMultipliers[modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.filterSlope]*LIANV_TimeConstantValues[modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.timeConstant] + CHECK_BALANCE_DELAY_BASE);
				ReadLockin(panel, PANEL_LOCKIN_READ, EVENT_COMMIT, NULL, 0, 0);
				if (lockinSettings.isBalanceInterrupted) {
					return;
				}
					
				if (sqrt(modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0]*modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0]+
						 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1]*modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1]) > modeSettings[0].channelSettings[sourceSettings.activeChannel].checkThreshold) {
					isBalanced = 0;
					BalanceChannel(panel);
					if (lockinSettings.isBalanceInterrupted) {
						return;
					}
				}
			}
			++balanceIteration;
		} while (!isBalanced && balanceIteration < MAX_FULL_BALANCE_STEPS);
	}

	if (!isBalanced) {
		warn("%s", msgStrings[MSG_MAX_FULL_BALANCE_STEPS]);
		lockinSettings.isBalanceInterrupted = 1;
		return;
	} else {
		if (sourceSettings.dataFileHandle != NULL)
			FileMenuSave(menuBar, MENUBAR_FILE_SAVE, NULL, panel);
	}
}

static void BalanceDoubleMode(int panel, ListType balanceProcedure)
{
	lockinSettings.isBalanceInterrupted = 0;
	
	for (int i = 0; i < 2; ++i) {
		BalanceMode(panel, balanceProcedure);
		if (lockinSettings.isBalanceInterrupted) {
			return;
		}
	}
}

static void BalanceFullAB(int panel, ListType balanceProcedure)
{
	lockinSettings.isBalanceInterrupted = 0;
	
	int balanceRepetitions;
	UIERRCHK(GetCtrlVal(panel, PANEL_BALANCE_REPETITIONS, &balanceRepetitions));
	
	int menuBar = GetPanelMenuBar(panel);
	if (menuBar <= 0)
		die(msgStrings[MSG_INTERNAL_ERROR]);
	if (sourceSettings.dataFileHandle == NULL) {
		FileMenuNew(menuBar, MENUBAR_FILE_NEW, NULL, panel);
		if (lockinSettings.isBalanceInterrupted) {
			return;
		}
	}
	
	for (int i = 1; i <= balanceRepetitions; ++i) {
		UIERRCHK(SetCtrlVal(panel, PANEL_CURRENT_REPETITION, i));
		UIERRCHK(ProcessDrawEvents());
		for (int j = 1; j < sourceSettings.nModes; ++j) {
			UIERRCHK(SetCtrlVal(panel, PANEL_ACTIVE_MODE, j));
			UIERRCHK(ProcessDrawEvents());
			SetActiveMode(panel, PANEL_ACTIVE_MODE, EVENT_COMMIT, NULL, 0, 0);
			if (lockinSettings.isBalanceInterrupted) {
				return;
			}
			BalanceMode(panel, balanceProcedure);
			if (lockinSettings.isBalanceInterrupted) {
				return;
			}
		}
	}
	
	FileMenuClose(menuBar, MENUBAR_FILE_CLOSE, NULL, panel);		
}

static void BalanceFullABBA(int panel, ListType balanceProcedure)
{
	lockinSettings.isBalanceInterrupted = 0;
	
	int balanceRepetitions;
	UIERRCHK(GetCtrlVal(panel, PANEL_BALANCE_REPETITIONS, &balanceRepetitions));
	
	int menuBar = GetPanelMenuBar(panel);
	if (menuBar <= 0)
		die(msgStrings[MSG_INTERNAL_ERROR]);
	if (sourceSettings.dataFileHandle == NULL) {
		FileMenuNew(menuBar, MENUBAR_FILE_NEW, NULL, panel);
		if (lockinSettings.isBalanceInterrupted) {
			return;
		}
	}
	
	for (int i = 1; i <= balanceRepetitions; ++i) {
		UIERRCHK(SetCtrlVal(panel, PANEL_CURRENT_REPETITION, i));
		UIERRCHK(ProcessDrawEvents());
		int j = 1;
		for (int d = 1; d >= -1; d-=2) { 
			for (; d==1 ? j < sourceSettings.nModes : j >=1; j+=d) {
				UIERRCHK(SetCtrlVal(panel, PANEL_ACTIVE_MODE, j));
				UIERRCHK(ProcessDrawEvents());
				SetActiveMode(panel, PANEL_ACTIVE_MODE, EVENT_COMMIT, NULL, 0, 0);
				if (lockinSettings.isBalanceInterrupted) {
					return;
				}
				BalanceMode(panel, balanceProcedure);
				if (lockinSettings.isBalanceInterrupted) {
					return;
				}
			}
			--j;
		}
	}
	
	FileMenuClose(menuBar, MENUBAR_FILE_CLOSE, NULL, panel);
}

