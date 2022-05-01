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
#include <cvirte.h>     
#include <userint.h>
#include <advanlys.h>
#include <utility.h>

#include "toolbox.h" 
#include "progressbar.h"

#include "main.h"
#include "msg.h" 
#include "cfg.h"
#include "panel.h"
#include "panels.h"
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

static int DIO_SetAllPorts(void);
static int DIO_ResetAllPorts(void);

//==============================================================================
// Global variables

//==============================================================================
// Global functions

int CVICALLBACK Connect (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	int pbPanel;
	
	switch (event) {
		case EVENT_COMMIT:
			if (programState == STATE_IDLE) { 
				programState = STATE_UPDATING;
				UpdatePanel(panel);
				
				UIERRCHK(pbPanel = LoadPanel(panel, panelsFile, PANEL_CON1));
				UIERRCHK(SetPanelAttribute(pbPanel, ATTR_TITLE, msgStrings[MSG_CONNECTING_TITLE]));
				UIERRCHK(ProgressBar_ConvertFromSlide(pbPanel, PANEL_CON1_PROGRESSBAR));
				
				double const progressFraction = 100.0/8.0;
				UIERRCHK(ProgressBar_SetMilestones(pbPanel, PANEL_CON1_PROGRESSBAR, 
												   1.0*progressFraction, 
												   2.0*progressFraction, 
												   3.0*progressFraction, 
												   4.0*progressFraction, 
												   5.0*progressFraction, 
												   6.0*progressFraction, 
												   7.0*progressFraction, 0.0));
				UIERRCHK(DisplayPanel(pbPanel));
				
				if (lockinSettings.isPresent) 
					 LIAERRCHK(LIANV_SetInputParameters(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings));
				UIERRCHK(ProgressBar_AdvanceMilestone(pbPanel, PANEL_CON1_PROGRESSBAR, 0)); // 1
				UIERRCHK(ProcessDrawEvents());
				
				unsigned char isRunning = 0;
				DSSERRCHK(DADSS_StartStop_Status(&isRunning));
				if (isRunning) {
					DSSERRCHK(DADSS_StartStop(0));
					Delay(5.0*DADSS_ADJ_DELAY);
				}
				DSSERRCHK(DADSS_StartStop_Status(&isRunning));
				if (isRunning) {
					warn("%s", msgStrings[MSG_DSS_ALREADY_RUNNING]);
					goto Error;
				}
				UIERRCHK(ProgressBar_AdvanceMilestone(pbPanel, PANEL_CON1_PROGRESSBAR, 0)); // 2
				UIERRCHK(ProcessDrawEvents());
				
				DSSERRCHK(DADSS_SetCLKFrequency(sourceSettings.clockFrequency));
				Delay(DADSS_ADJ_DELAY); 
				DSSERRCHK(DADSS_SetFrequency(sourceSettings.frequency));
				Delay(DADSS_ADJ_DELAY); 
				DSSERRCHK(DADSS_GetRealFrequency(&sourceSettings.realFrequency));
				DSSERRCHK(DADSS_GetNumberSamples(&sourceSettings.nSamples));
				UIERRCHK(ProgressBar_AdvanceMilestone(pbPanel, PANEL_CON1_PROGRESSBAR, 0)); // 3
				UIERRCHK(ProcessDrawEvents());
				
				for (int i = 0; i < DADSS_CHANNELS; ++i)
					DSSERRCHK(DADSS_SetRange(i+1, sourceSettings.range[i])); 
				DSSERRCHK(DADSS_UpdateConfiguration());
				UIERRCHK(ProgressBar_AdvanceMilestone(pbPanel, PANEL_CON1_PROGRESSBAR, 0)); // 4
				UIERRCHK(ProcessDrawEvents());
				
				for (int i = 0; i < DADSS_CHANNELS; ++i)
					DSSERRCHK(DADSS_SetMDAC2(i+1, modeSettings[0].channelSettings[i].mdac2Code));
				DSSERRCHK(DADSS_UpdateMDAC2());
				UIERRCHK(ProgressBar_AdvanceMilestone(pbPanel, PANEL_CON1_PROGRESSBAR, 0)); // 5
				UIERRCHK(ProcessDrawEvents());
				
				UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 0));
				for (int i = 0; i < DADSS_CHANNELS; ++i) {
					DSSERRCHK(DADSS_SetMDAC1Offset(i+1, modeSettings[0].channelSettings[i].mdac1Offset));
					DSSERRCHK(DADSS_SetAmplitude(i+1, modeSettings[0].channelSettings[i].amplitude));
					DSSERRCHK(DADSS_SetPhase(i+1, modeSettings[0].channelSettings[i].phase));
				}
				DSSERRCHK(DADSS_UpdateWaveform());
				UIERRCHK(ProgressBar_AdvanceMilestone(pbPanel, PANEL_CON1_PROGRESSBAR, 0)); // 6
				UIERRCHK(ProcessDrawEvents());
				
				Delay(DADSS_ADJ_DELAY);
				for (int i = 0; i < DADSS_CHANNELS; ++i) {
					DSSERRCHK(DADSS_GetAmplitude(i+1, &modeSettings[0].channelSettings[i].amplitude));
					DSSERRCHK(DADSS_GetPhase(i+1, &modeSettings[0].channelSettings[i].phase));
					ToRect(modeSettings[0].channelSettings[i].amplitude,
						   modeSettings[0].channelSettings[i].phase, 
						   &modeSettings[0].channelSettings[i].real, 
						   &modeSettings[0].channelSettings[i].imag);
				}
				UIERRCHK(ProgressBar_AdvanceMilestone(pbPanel, PANEL_CON1_PROGRESSBAR, 0)); // 7
				UIERRCHK(ProcessDrawEvents());
				
				if (switchSettings.isPresent) {
					DIOERRCHK(DIO_SetAllPorts());
				}
				UIERRCHK(ProgressBar_AdvanceMilestone(pbPanel, PANEL_CON1_PROGRESSBAR, 0)); // 8
				UIERRCHK(ProcessDrawEvents());
				
				UIERRCHK(DiscardPanel(pbPanel));
				programState = STATE_CONNECTED;				
				UpdatePanel(panel);
			} else if (programState == STATE_CONNECTED) { 
				programState = STATE_UPDATING;
				UpdatePanel(panel);
				if (switchSettings.isPresent) {
					DIO_ResetAllPorts();
				}
				programState = STATE_IDLE;
				UpdatePanel(panel);
			} else
				die(msgStrings[MSG_INTERNAL_ERROR]);
			break;
	}
	return 0;
	
Error:
	UIERRCHK(DiscardPanel(pbPanel));
	programState = STATE_IDLE;
	UpdatePanel(panel);
	return 0;
}

int CVICALLBACK CopyWaveformParameters (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char buf[CLIPBOARD_BUF_SZ];
	int samples[DADSS_SAMPLES_MAX] = {0};
	double xReal[DADSS_SAMPLES_MAX] = {0.0};
	double xImag[DADSS_SAMPLES_MAX] = {0.0};
	double dacScale;
	

	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_COPY_REAL_FREQUENCY:
					snprintf(buf, sizeof buf, "f = %.11g;", sourceSettings.realFrequency);
					break;
				case PANEL_COPY_PHASOR:
					snprintf(buf, sizeof buf, "%.15g%+.15gi", modeSettings[0].channelSettings[sourceSettings.activeChannel].real, 
							 modeSettings[0].channelSettings[sourceSettings.activeChannel].imag);
					break;
				case PANEL_COPY_SAMPLES:
					DSSERRCHK(DADSS_GetWaveform(sourceSettings.activeChannel+1,samples,sourceSettings.nSamples));
					char *cur = buf;
					char *end = buf + sizeof buf; 
					for (int i = 0; i < sourceSettings.nSamples && end > cur; ++i) {	
						cur += snprintf(cur, end-cur, "%d\n", samples[i]);
					}
					break;
				case PANEL_COPY_PHASOR_1:
					DSSERRCHK(DADSS_GetWaveform(sourceSettings.activeChannel+1,samples,sourceSettings.nSamples));
					for (int i = 0; i < DADSS_SAMPLES_MAX; ++i)
						xReal[i] = samples[i];
					ALERRCHK(FFT(xReal, xImag, sourceSettings.nSamples)); 
					dacScale = (modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Val) *  
							   (DADSS_RangeMultipliers[sourceSettings.range[sourceSettings.activeChannel]]/DADSS_MDAC1_CODE_RANGE) * 
							   DADSS_REFERENCE_VOLTAGE/sourceSettings.nSamples;
					snprintf(buf, sizeof buf, "%.10g%+.10gi",
							 (xImag[sourceSettings.nSamples-1] - xImag[1]) * dacScale,
							 (xReal[1] + xReal[sourceSettings.nSamples-1]) * dacScale);
			}
			UIERRCHK(ClipboardPutText(buf));
			break;
		default:
			break;
	}
	return 0;

Error: 
	return 0;
}

int CVICALLBACK ManagePanel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_CLOSE: 
			int ret = 0;
			UIERRCHK(ret = ConfirmPopup(msgStrings[MSG_POPUP_CONFIRM_TITLE], msgStrings[MSG_POPUP_QUIT]));
			if (ret == 0)
				return 0;
			if (sourceSettings.dataFileHandle != NULL) {
				int menuBar = GetPanelMenuBar(panel);
				if (menuBar <= 0)
					die(msgStrings[MSG_INTERNAL_ERROR]);
				FileMenuClose(menuBar, MENUBAR_FILE_CLOSE, NULL, panel);
			}
			switch (programState) {
				case STATE_RUNNING:
					StartStop(panel, PANEL_START_STOP, EVENT_COMMIT, NULL, 0, 0);
					if (programState == STATE_RUNNING) {
						return 0;
					}
					Connect(panel, PANEL_CONNECT, EVENT_COMMIT, NULL, 0, 0);
					break;
				case STATE_CONNECTED:
					Connect(panel, PANEL_CONNECT, EVENT_COMMIT, NULL, 0, 0);
					break;
				case STATE_IDLE:
					break;
				default:
					die(msgStrings[MSG_INTERNAL_ERROR]);
			}
			UIERRCHK(QuitUserInterface(0));
			break;
	}
	return 0;
}

int CVICALLBACK ReadLockin (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	ProgramState savedProgramState = programState;
	
	switch (event)	{
		case EVENT_COMMIT:
			programState = STATE_UPDATING;
			UpdatePanel(panel);
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
			char buf[CLIPBOARD_BUF_SZ];
			snprintf(buf, sizeof buf, "%.15g%+.15gi",  modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
							modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1]);
			UIERRCHK(ClipboardPutText(buf));
			
			LIAERRCHK(LIANV_GetInputParameters(lockinSettings.nvProcessName, &modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings)); 
			UpdatePanelLockin(panel);
			programState = savedProgramState;
			UpdatePanel(panel);
			break;
	}
	return 0;

Error:
	UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 0));
	lockinSettings.isBalanceInterrupted = 1;
	programState = savedProgramState;
	UpdatePanel(panel);
	return 0;
}

int CVICALLBACK SetActiveChannel (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	ProgramState savedProgramState = programState;
	
	switch (event) {
		case EVENT_COMMIT:
			UIERRCHK(GetCtrlVal(panel, control, &sourceSettings.activeChannel));
			programState = STATE_UPDATING;
			UpdatePanel(panel);
			if (lockinSettings.isPresent) {
				LIAERRCHK(LIANV_SetInputParameters(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings));
			}
			if (switchSettings.isPresent) {
				DIOERRCHK(DIO_SetAllPorts());
			}
			programState = savedProgramState;
			UpdatePanel(panel);
			break;
	}
	return 0;
	
Error:
	lockinSettings.isBalanceInterrupted = 1;
	programState = savedProgramState;
	UpdatePanel(panel);
	return 0;
}

int CVICALLBACK SetBalance (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_BALANCE_THRESHOLD:
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].balanceThreshold));
					break;
				case PANEL_CHECK_THRESHOLD:
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].checkThreshold));
					break;
				case PANEL_BALANCE_TARGET:
					UIERRCHK(GetCtrlVal(panel, control, (int *) &sourceSettings.balanceTarget));
					break;
			}
			UpdatePanelBalance(panel);
			break;
	}

	return 0;
}

int CVICALLBACK SetConnection (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_CON2_OK:
					UIERRCHK(GetCtrlVal(panel, PANEL_CON2_NV_SERVER_NAME, sourceSettings.nvServerName)); 
					UIERRCHK(GetCtrlVal(panel, PANEL_CON2_SOURCE_PROCESS_NAME, sourceSettings.nvProcessName)); 
					UIERRCHK(GetCtrlVal(panel, PANEL_CON2_LOCKIN_ISPRESENT, &lockinSettings.isPresent));
					UIERRCHK(GetCtrlVal(panel, PANEL_CON2_LOCKIN_PROCESS_NAME, lockinSettings.nvProcessName));
					UIERRCHK(GetCtrlVal(panel, PANEL_CON2_SWITCH_ISPRESENT, &switchSettings.isPresent));
					UIERRCHK(GetCtrlVal(panel, PANEL_CON2_SWITCH_PROCESS_NAME, switchSettings.nvProcessName));
					DADSS_SetNameNVServer(sourceSettings.nvServerName);
					DADSS_SetNameNVProcess(sourceSettings.nvProcessName);
					DIO_SetNameNVServer(sourceSettings.nvServerName);
					DIO_SetNameNVProcess(switchSettings.nvProcessName);
					LIANV_SetNameNVServer(sourceSettings.nvServerName);
					
					if (!switchSettings.isPresent)
						sourceSettings.balanceTarget = BALANCE_CHANNEL;
					
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
				case PANEL_CON2_CANCEL:
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK SetFrequency (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_CLOCKFREQUENCY:
					UIERRCHK(GetCtrlVal(panel, control, &sourceSettings.clockFrequency));
					DSSERRCHK(DADSS_SetCLKFrequency(sourceSettings.clockFrequency));
					Delay(DADSS_ADJ_DELAY);
					UIERRCHK(SetCtrlVal(panel, PANEL_CLOCKFREQUENCY, sourceSettings.clockFrequency));
				case PANEL_FREQUENCY:
					UIERRCHK(GetCtrlVal(panel, PANEL_FREQUENCY, &sourceSettings.frequency));
					DSSERRCHK(DADSS_SetFrequency(sourceSettings.frequency));
					Delay(DADSS_ADJ_DELAY);
					DSSERRCHK(DADSS_GetRealFrequency(&sourceSettings.realFrequency));
					UIERRCHK(SetCtrlVal(panel, PANEL_REAL_FREQUENCY, sourceSettings.realFrequency));
					DSSERRCHK(DADSS_GetNumberSamples(&sourceSettings.nSamples));
					UIERRCHK(SetCtrlVal(panel, PANEL_SAMPLES, sourceSettings.nSamples));
					sourceSettings.isFileModified = 1;
					break;
				case PANEL_SAMPLES:
					UIERRCHK(GetCtrlVal(panel, control, &sourceSettings.nSamples));
					DSSERRCHK(DADSS_SetNumberSamples(sourceSettings.nSamples));
					Delay(DADSS_ADJ_DELAY);
					UIERRCHK(SetCtrlVal(panel, PANEL_SAMPLES, sourceSettings.nSamples));
					DSSERRCHK(DADSS_GetFrequency(&sourceSettings.frequency));
					UIERRCHK(SetCtrlVal(panel, PANEL_FREQUENCY, sourceSettings.frequency));
					DSSERRCHK(DADSS_GetRealFrequency(&sourceSettings.realFrequency));
					UIERRCHK(SetCtrlVal(panel, PANEL_REAL_FREQUENCY, sourceSettings.realFrequency));
					sourceSettings.isFileModified = 1;
			}
			break;
	}
	return 0;

Error:
	return 0;
}

int CVICALLBACK SetLabel (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			UIERRCHK(GetCtrlVal(panel, control, sourceSettings.label[sourceSettings.activeChannel]));
			break;	  
	}
	return 0;
}

int CVICALLBACK SetLockinParameters (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			int valLen, setting;
			GetTableCellValLength(panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, eventData1), &valLen);
			char valBuf[valLen+1];
			GetTableCellVal (panel, PANEL_LOCKIN_PARAMETERS, MakePoint(1, eventData1), valBuf);
			GetTableCellRingIndexFromValue (panel, PANEL_LOCKIN_PARAMETERS, 0, MakePoint(1, eventData1), &setting, valBuf);
			switch (eventData1) {
				case LIANV_GAIN:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].gainMode = setting;
					break;
				case LIANV_SENSITIVITY:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.sensitivity = setting;
					LIAERRCHK(LIANV_SetSensitivity(lockinSettings.nvProcessName, setting));
					break;
				case LIANV_TIME_CONSTANT:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.timeConstant = setting;
					LIAERRCHK(LIANV_SetTimeConstant(lockinSettings.nvProcessName, setting));
					break;
				case LIANV_FILTER_SLOPE:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.filterSlope = setting;
					LIAERRCHK(LIANV_SetLowPassFilterSlope(lockinSettings.nvProcessName, setting));
					break;
				case LIANV_INPUT_CONFIGURATION:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputConfiguration = setting;
					LIAERRCHK(LIANV_SetInputConfiguration(lockinSettings.nvProcessName, setting));
					break;
				case LIANV_INPUT_COUPLING:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputCoupling = setting;
					LIAERRCHK(LIANV_SetInputCoupling(lockinSettings.nvProcessName, setting));
					break;
				case LIANV_INPUT_SHIELD_GROUNDING:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputShieldGrounding = setting;
					LIAERRCHK(LIANV_SetInputShieldGrounding(lockinSettings.nvProcessName, setting));
					break;
				case LIANV_INPUT_RESERVE:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputReserve = setting;
					LIAERRCHK(LIANV_SetReserveMode(lockinSettings.nvProcessName, setting));
					break;
				case LIANV_INPUT_LINE_NOTCH_FILTER:
					modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings.inputLineNotchFilter = setting;
					LIAERRCHK(LIANV_SetInputLineNotchFilter(lockinSettings.nvProcessName, setting));
					break;
				default:
					die(msgStrings[MSG_INTERNAL_ERROR]);
			}
			UpdatePanelLockin(panel); 
			break;
	}
	return 0;

Error:
	UpdatePanelLockin(panel);
	return 0;
}

int CVICALLBACK SetRange (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			UIERRCHK(GetCtrlVal(panel, control, (int *)&sourceSettings.range[sourceSettings.activeChannel]));
			DSSERRCHK(DADSS_SetRange(sourceSettings.activeChannel+1, 
									 sourceSettings.range[sourceSettings.activeChannel]));
			DSSERRCHK(DADSS_UpdateConfiguration());
			DSSERRCHK(DADSS_UpdateWaveform());
			Delay(DADSS_ADJ_DELAY);
			DSSERRCHK(DADSS_GetWaveformParametersPolar(sourceSettings.activeChannel+1, 
							&modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude,
							&modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
			ToRect(modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude,
					modeSettings[0].channelSettings[sourceSettings.activeChannel].phase, 
				   &modeSettings[0].channelSettings[sourceSettings.activeChannel].real, 
				   &modeSettings[0].channelSettings[sourceSettings.activeChannel].imag);
			UpdatePanelActiveChannel(panel);
			break;
	}
	return 0;

Error:
	UpdatePanelActiveChannel(panel);
	return 0;
}

int CVICALLBACK SetWaveformParameters (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	double maxAmplitude;
	
	switch (event) {
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_AMPLITUDE:
					DSSERRCHK(DADSS_GetAmplitudeMax(sourceSettings.activeChannel+1, &maxAmplitude));
					
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude));
					if (modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude > maxAmplitude) {
						PlaySound("Media/overrange.wav", NULL, SND_FILENAME);
						UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 1));
						modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude = maxAmplitude;
					} else {
						UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 0));
					}
					DSSERRCHK(DADSS_SetAmplitude(sourceSettings.activeChannel+1,
													   modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude));
					DSSERRCHK(DADSS_UpdateWaveform());
					break;
				case PANEL_PHASE:
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
					DSSERRCHK(DADSS_SetPhase(sourceSettings.activeChannel+1,
												   modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
					DSSERRCHK(DADSS_UpdateWaveform());
					break;
				case PANEL_REAL:
					DSSERRCHK(DADSS_GetAmplitudeMax(sourceSettings.activeChannel+1, &maxAmplitude));
					
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].real));
					ToPolar(modeSettings[0].channelSettings[sourceSettings.activeChannel].real, 
							modeSettings[0].channelSettings[sourceSettings.activeChannel].imag,
							&modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude, 
							&modeSettings[0].channelSettings[sourceSettings.activeChannel].phase);
					if (modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude > maxAmplitude) {
						PlaySound("Media/overrange.wav", NULL, SND_FILENAME);
						UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 1));
						modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude = maxAmplitude;
					} else {
						UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 0));
					}
					DSSERRCHK(DADSS_SetWaveformParametersPolar(sourceSettings.activeChannel+1,
									modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude,
									modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
					DSSERRCHK(DADSS_UpdateWaveform());
					break;
				case PANEL_IMAG:
					DSSERRCHK(DADSS_GetAmplitudeMax(sourceSettings.activeChannel+1, &maxAmplitude));
					
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].imag));
					ToPolar(modeSettings[0].channelSettings[sourceSettings.activeChannel].real, 
							modeSettings[0].channelSettings[sourceSettings.activeChannel].imag,
							&modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude, 
							&modeSettings[0].channelSettings[sourceSettings.activeChannel].phase);
					if (modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude > maxAmplitude) {
						PlaySound("Media/overrange.wav", NULL, SND_FILENAME);
						UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 1));
						modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude = maxAmplitude;
					} else {
						UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 0));
					}
					DSSERRCHK(DADSS_SetWaveformParametersPolar(sourceSettings.activeChannel+1,
									modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude,
									modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
					DSSERRCHK(DADSS_UpdateWaveform());
					break;
				case PANEL_MDAC1_OFFSET:
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac1Offset));
					DSSERRCHK(DADSS_SetMDAC1Offset(sourceSettings.activeChannel+1, 
												   modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac1Offset));
					DSSERRCHK(DADSS_UpdateWaveform());
					break;
				case PANEL_MDAC2_CODE:
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Code));
					DSSERRCHK(DADSS_SetMDAC2(sourceSettings.activeChannel+1, 
												 modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Code));
					DSSERRCHK(DADSS_UpdateMDAC2());
					break;
				case PANEL_MDAC2_VAL:
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Val));
					DSSERRCHK(DADSS_Mdac2ValueToCode(modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Val,
														   &modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Code));
					DSSERRCHK(DADSS_SetMDAC2(sourceSettings.activeChannel+1, 
												   modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Code));
					DSSERRCHK(DADSS_UpdateMDAC2());
					break;
				case PANEL_PHASE_ADD_PIHALF:
					if (modeSettings[0].channelSettings[sourceSettings.activeChannel].phase + PI/2 <= DADSS_PHASE_MAX) {
						modeSettings[0].channelSettings[sourceSettings.activeChannel].phase += PI/2;
						DSSERRCHK(DADSS_SetPhase(sourceSettings.activeChannel+1,
													   modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
						DSSERRCHK(DADSS_UpdateWaveform());
					} else {
						return 0;
					}
					break;
				case PANEL_PHASE_ADD_PI:
					if (modeSettings[0].channelSettings[sourceSettings.activeChannel].phase + PI <= DADSS_PHASE_MAX) {
						modeSettings[0].channelSettings[sourceSettings.activeChannel].phase += PI;
						DSSERRCHK(DADSS_SetPhase(sourceSettings.activeChannel+1,
													   modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
						DSSERRCHK(DADSS_UpdateWaveform());
					} else {
						return 0;
					}
					break;
				case PANEL_PHASE_SUBTRACT_PIHALF:
					if (modeSettings[0].channelSettings[sourceSettings.activeChannel].phase - PI/2 >= DADSS_PHASE_MIN) {
						modeSettings[0].channelSettings[sourceSettings.activeChannel].phase -= PI/2;
						DSSERRCHK(DADSS_SetPhase(sourceSettings.activeChannel+1,
													   modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
						DSSERRCHK(DADSS_UpdateWaveform());
					} else {
						return 0;
					}
					break;
				case PANEL_PHASE_SUBTRACT_PI:
					if (modeSettings[0].channelSettings[sourceSettings.activeChannel].phase - PI >= DADSS_PHASE_MIN) {
						modeSettings[0].channelSettings[sourceSettings.activeChannel].phase -= PI;
						DSSERRCHK(DADSS_SetPhase(sourceSettings.activeChannel+1,
													   modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
						DSSERRCHK(DADSS_UpdateWaveform());
					} else {
						return 0;
					}
					break;
				case PANEL_PASTE_PHASOR:
					char *clipboardText;
					ClipboardGetText(&clipboardText, 0);
					if (clipboardText == 0)
						return 0;
					char clipboardTextTrimmed[CLIPBOARD_BUF_SZ];
					char *p = clipboardTextTrimmed;
					char c;
					for (size_t i = 0; 
							p-clipboardTextTrimmed < CLIPBOARD_BUF_SZ && (c = clipboardText[i]) != '\0'; 
							++i) {
						if (!isspace(c)) {
							*p++ = c;
						}
					}
					double realTmp;
					double imagTmp;
					if (sscanf(clipboardTextTrimmed, "%lf %lf", &realTmp, &imagTmp) == 2) {
						DSSERRCHK(DADSS_GetAmplitudeMax(sourceSettings.activeChannel+1, &maxAmplitude));
	
						ToPolar(realTmp,imagTmp,
								&modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude, 
								&modeSettings[0].channelSettings[sourceSettings.activeChannel].phase);
						if (modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude > maxAmplitude) {
							PlaySound("Media/overrange.wav", NULL, SND_FILENAME);
							UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 1));
							modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude = maxAmplitude;
						} else {
							UIERRCHK(SetCtrlVal(panel, PANEL_OUT_OF_RANGE_LED, 0));
						}
						DSSERRCHK(DADSS_SetWaveformParametersPolar(sourceSettings.activeChannel+1,
								  modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude,
								  modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
						DSSERRCHK(DADSS_UpdateWaveform());
					}
					free(clipboardText);
					break;
			}
			Delay(DADSS_ADJ_DELAY);
			DSSERRCHK(DADSS_GetMDAC1Offset(sourceSettings.activeChannel+1,
										   &modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac1Offset));
			DSSERRCHK(DADSS_GetMDAC2(sourceSettings.activeChannel+1,
									 &modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Code));
			DSSERRCHK(DADSS_Mdac2CodeToValue(modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Code,
											 &modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Val));
			DSSERRCHK(DADSS_GetWaveformParametersPolar(sourceSettings.activeChannel+1,
					  &modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude,
					  &modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
			ToRect(modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude, 
				   modeSettings[0].channelSettings[sourceSettings.activeChannel].phase, 
				   &modeSettings[0].channelSettings[sourceSettings.activeChannel].real, 
				   &modeSettings[0].channelSettings[sourceSettings.activeChannel].imag);
			sourceSettings.isFileModified = 1;
			UpdatePanel(panel);
			break;
	}
	return 0;

Error:
	return 0;
}

int CVICALLBACK StartStop (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{	
	switch (event) {
		case EVENT_COMMIT:
			if (programState == STATE_CONNECTED) { // Start
				programState = STATE_UPDATING;
				UpdatePanel(panel);
	
				DSSERRCHK(DADSS_StartStop(1));
					
				programState = STATE_RUNNING;
				UpdatePanel(panel);
			} else if (programState == STATE_RUNNING) { // Stop
				programState = STATE_UPDATING;
				UpdatePanel(panel);
	
				DSSERRCHK(DADSS_StartStop(0));

				programState = STATE_CONNECTED;				
				UpdatePanel(panel);
			} else
				die(msgStrings[MSG_INTERNAL_ERROR]);
			break;
	}
	return 0;
	
Error:
	programState = STATE_IDLE;
	UpdatePanel(panel);
	return 0;
}

int CVICALLBACK ToggleLock (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			if (!modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked) {
				modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked = 1;
			} else {
				int ans = ConfirmPopup(msgStrings[MSG_POPUP_CONFIRM_TITLE], msgStrings[MSG_UNLOCK_CHANNEL]);
				UIERRCHK(ans);
				if (ans == 0) {
					SetCtrlVal(panel, control, 1);
					return 0;
				}
				modeSettings[0].channelSettings[sourceSettings.activeChannel].isLocked = 0;
				SetCtrlVal(panel, control, 0);
			}
			UpdatePanel(panel);
			break;
	}
	return 0;
}

int CVICALLBACK SetModes (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_MOD_ADD:
					if (sourceSettings.nModes < MAX_MODES) {
						UIERRCHK(InsertTableRows(panel, PANEL_MOD_LIST, -1, 1, VAL_CELL_STRING));
						SetDefaultModeSettings(sourceSettings.nModes);
						UIERRCHK(SetTableCellVal(panel, PANEL_MOD_LIST, MakePoint(1,sourceSettings.nModes), modeSettings[sourceSettings.nModes].label));
						sourceSettings.nModes++;
					}
					break;
				case PANEL_MOD_DUPLICATE:
					if (sourceSettings.nModes < MAX_MODES) {
						Point activeTableCell;
						UIERRCHK(GetActiveTableCell (panel, PANEL_MOD_LIST, &activeTableCell));
						UIERRCHK(InsertTableRows(panel, PANEL_MOD_LIST, -1, 1, VAL_CELL_STRING));
						modeSettings[sourceSettings.nModes] = modeSettings[activeTableCell.y];
						UIERRCHK(SetTableCellVal(panel, PANEL_MOD_LIST, MakePoint(1,sourceSettings.nModes), modeSettings[sourceSettings.nModes].label));
						sourceSettings.nModes++;
					}
					break;
				case PANEL_MOD_REMOVE:
					if (sourceSettings.nModes > sourceSettings.activeMode+1) {
						UIERRCHK(DeleteTableRows(panel, PANEL_MOD_LIST, sourceSettings.nModes-1, -1));
						sourceSettings.nModes--;
					} else {
						warn("%s", msgStrings[MSG_CANNOT_REMOVE_ACTIVE_MODE]);
					}
					break;
				case PANEL_MOD_LIST:
					UIERRCHK(GetTableCellVal(panel, PANEL_MOD_LIST, MakePoint(1,eventData1), modeSettings[eventData1].label));
					if (eventData1 == sourceSettings.activeMode) {
						strncpy(modeSettings[0].label, modeSettings[eventData1].label, LABEL_SZ);
					}
					break;
				case PANEL_MOD_OK:
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanelActiveMode(panel);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK SetActiveMode (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	ProgramState savedProgramState = programState;
	
	switch (event) {
		case EVENT_COMMIT:
			UIERRCHK(GetCtrlVal(panel, control, &sourceSettings.activeMode));
			modeSettings[0] = modeSettings[sourceSettings.activeMode];
			programState = STATE_UPDATING;
			UpdatePanel(panel);
			for (int i = 0; i < DADSS_CHANNELS; ++i)
				DSSERRCHK(DADSS_SetMDAC2(i+1, modeSettings[0].channelSettings[i].mdac2Code));
			DSSERRCHK(DADSS_UpdateMDAC2());
			for (int i = 0; i < DADSS_CHANNELS; ++i) {
				DSSERRCHK(DADSS_SetAmplitude(i+1, modeSettings[0].channelSettings[i].amplitude));
				DSSERRCHK(DADSS_SetPhase(i+1, modeSettings[0].channelSettings[i].phase));
			}
			DSSERRCHK(DADSS_UpdateWaveform());
			Delay(DADSS_ADJ_DELAY);
			for (int i = 0; i < DADSS_CHANNELS; ++i) {
				DSSERRCHK(DADSS_GetAmplitude(i+1, &modeSettings[0].channelSettings[i].amplitude));
				DSSERRCHK(DADSS_GetPhase(i+1, &modeSettings[0].channelSettings[i].phase));
			}
			if (lockinSettings.isPresent) { 
				LIAERRCHK(LIANV_SetInputParameters(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinInputSettings)); 
			}
			if (switchSettings.isPresent) {
				DIOERRCHK(DIO_SetAllPorts());
			}
			sourceSettings.isFileModified = 1;
			programState = savedProgramState; 
			UpdatePanel(panel);
			break;
		default:
			break;
	}
	return 0;

Error:
	lockinSettings.isBalanceInterrupted = 1;
	programState = savedProgramState; 
	UpdatePanel(panel);
	return 0;
}

int CVICALLBACK UpdateActiveMode (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			modeSettings[sourceSettings.activeMode] = modeSettings[0];
			break;
		default:
			break;
	}
	return 0;
}

int CVICALLBACK SwapActiveChannel (int panel, int control, int event,
							  		void *callbackData, int eventData1, int eventData2)
{
	int swapActiveChannelPanel; 
	switch (event)
	{
		case EVENT_COMMIT:
			UIERRCHK(swapActiveChannelPanel = LoadPanel(0, panelsFile, PANEL_SWAP));
			UIERRCHK(SetTableColumnAttribute (swapActiveChannelPanel, PANEL_SWAP_LIST, 1, ATTR_MAX_ENTRY_LENGTH, LABEL_SZ-1));
			UIERRCHK(InsertTableRows(swapActiveChannelPanel, PANEL_SWAP_LIST, -1, DADSS_CHANNELS, VAL_CELL_STRING));
			for (int i = 0; i < DADSS_CHANNELS; ++i)
				UIERRCHK(SetTableCellVal(swapActiveChannelPanel, PANEL_SWAP_LIST, MakePoint(1,i+1), sourceSettings.label[i]));
			SetActiveTableCell (swapActiveChannelPanel, PANEL_SWAP_LIST, MakePoint(1, sourceSettings.activeChannel+1));
			UIERRCHK(SetCtrlAttribute(swapActiveChannelPanel, PANEL_SWAP_LIST, ATTR_ENABLE_COLUMN_SIZING, 0));
			UIERRCHK(SetCtrlAttribute(swapActiveChannelPanel, PANEL_SWAP_LIST, ATTR_ENABLE_ROW_SIZING, 0));
			UIERRCHK(InstallPopup(swapActiveChannelPanel));
			break;
		default:
			break;
	}
	return 0;
}

int CVICALLBACK SetSwapDestinationChannel (int panel, int control, int event,
							  				void *callbackData, int eventData1, int eventData2)
{
	Point activeTableCell;
	int swapDestinationChannel;
	ChannelSettings channelSettingsTmp;
	
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_SWAP_OK:
					UIERRCHK(SetPanelAttribute(panel,ATTR_DIMMED, 1));
					UIERRCHK(ProcessDrawEvents()); 
					UIERRCHK(GetActiveTableCell (panel, PANEL_SWAP_LIST, &activeTableCell));
					swapDestinationChannel = activeTableCell.y-1;
					if (swapDestinationChannel != sourceSettings.activeChannel) {
						channelSettingsTmp = modeSettings[0].channelSettings[sourceSettings.activeChannel];
						modeSettings[0].channelSettings[sourceSettings.activeChannel] = modeSettings[0].channelSettings[swapDestinationChannel];
						modeSettings[0].channelSettings[swapDestinationChannel] = channelSettingsTmp;
						DSSERRCHK(DADSS_SetMDAC2(sourceSettings.activeChannel+1, modeSettings[0].channelSettings[sourceSettings.activeChannel].mdac2Code));
						DSSERRCHK(DADSS_SetMDAC2(swapDestinationChannel+1, modeSettings[0].channelSettings[swapDestinationChannel].mdac2Code));
						DSSERRCHK(DADSS_UpdateMDAC2());
						DSSERRCHK(DADSS_SetAmplitude(sourceSettings.activeChannel+1, modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude));
						DSSERRCHK(DADSS_SetPhase(sourceSettings.activeChannel+1, modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
						DSSERRCHK(DADSS_SetAmplitude(swapDestinationChannel+1, modeSettings[0].channelSettings[swapDestinationChannel].amplitude));
						DSSERRCHK(DADSS_SetPhase(swapDestinationChannel+1, modeSettings[0].channelSettings[swapDestinationChannel].phase));
						DSSERRCHK(DADSS_UpdateWaveform());
						Delay(DADSS_ADJ_DELAY);
						DSSERRCHK(DADSS_GetAmplitude(sourceSettings.activeChannel+1, &modeSettings[0].channelSettings[sourceSettings.activeChannel].amplitude));
						DSSERRCHK(DADSS_GetPhase(sourceSettings.activeChannel+1, &modeSettings[0].channelSettings[sourceSettings.activeChannel].phase));
						DSSERRCHK(DADSS_GetAmplitude(swapDestinationChannel+1, &modeSettings[0].channelSettings[swapDestinationChannel].amplitude));
						DSSERRCHK(DADSS_GetPhase(swapDestinationChannel+1, &modeSettings[0].channelSettings[swapDestinationChannel].phase));
					}
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
				case PANEL_SWAP_CANCEL:
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
			}
			break;
	}
	return 0;

Error:
	return 0;
}

int CVICALLBACK PresetBridgeParameters (int panel, int control, int event,
										void *callbackData, int eventData1, int eventData2)
{
	StandardType impedanceTypeA, impedanceTypeB;
	double RA = 0, tauA = 0, RB = 0, tauB = 0;
	double CA = 0, DA = 0, CB = 0, DB = 0;
	double LA = 0, XA = 0, LB = 0, XB = 0; 
	double GA = 0, BA = 0, GB = 0, BB = 0;
	double impedanceAMagnitude, impedanceAPhase, impedanceBMagnitude, impedanceBPhase;
	double impedanceALoadedMagnitude, impedanceALoadedPhase, impedanceBLoadedMagnitude, impedanceBLoadedPhase;
	double rmsCurrent, currentAmplitude, currentPhase;
	SourceSettings sourceSettingsTmp = sourceSettings;
	ModeSettings modeSettingsTmp = modeSettings[0];
	
	switch (event) {
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_PRE_IMPEDANCE_A:
					UIERRCHK(GetCtrlVal(panel, control, (int *) &impedanceTypeA));
					switch (impedanceTypeA)
					{
						case RESISTANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_A, ATTR_LABEL_TEXT, "R ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_LABEL_TEXT, "tau ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_A, "ohm"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_A, "second"));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_MIN_VALUE, -INFINITY));
							break;
						case CAPACITANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_A, ATTR_LABEL_TEXT, "C ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_LABEL_TEXT, "D ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_A, "farad"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_A, ""));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_MIN_VALUE, 0.0));
							break;
						case INDUCTANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_A, ATTR_LABEL_TEXT, "L ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_LABEL_TEXT, "R ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_A, "henry"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_A, "ohm"));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_MIN_VALUE, 0.0));
							break;
						case IMPEDANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_A, ATTR_LABEL_TEXT, "R ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_LABEL_TEXT, "X ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_A, "ohm"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_A, "ohm"));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_MIN_VALUE, -INFINITY));
							break;
						case ADMITTANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_A, ATTR_LABEL_TEXT, "G ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_LABEL_TEXT, "B ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_A, "siemens"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_A, "siemens"));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_A, ATTR_MIN_VALUE, -INFINITY));
							break;
					}
					break;
				case PANEL_PRE_IMPEDANCE_B:
					UIERRCHK(GetCtrlVal(panel, control, (int *) &impedanceTypeB));
					switch (impedanceTypeB)
					{
						case RESISTANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_B, ATTR_LABEL_TEXT, "R ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_LABEL_TEXT, "tau ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_B, "ohm"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_B, "second"));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_MIN_VALUE, -INFINITY));
							break;
						case CAPACITANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_B, ATTR_LABEL_TEXT, "C ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_LABEL_TEXT, "D ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_B, "farad"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_B, ""));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_MIN_VALUE, 0.0));
							break;
						case INDUCTANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_B, ATTR_LABEL_TEXT, "L ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_LABEL_TEXT, "R ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_B, "henry"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_B, "ohm"));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_MIN_VALUE, 0.0));
							break;
						case IMPEDANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_B, ATTR_LABEL_TEXT, "R ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_LABEL_TEXT, "X ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_B, "ohm"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_B, "ohm"));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_MIN_VALUE, -INFINITY));
							break;
						case ADMITTANCE:
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_PRIMARY_B, ATTR_LABEL_TEXT, "G ="));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_LABEL_TEXT, "B ="));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_PRIMARY_B, "siemens"));
							UIERRCHK(SetCtrlVal(panel, PANEL_PRE_UNIT_SECONDARY_B, "siemens"));
							UIERRCHK(SetCtrlAttribute(panel, PANEL_PRE_SECONDARY_B, ATTR_MIN_VALUE, -INFINITY));
							break;
					}
					break;
				case PANEL_PRE_SET:
					UIERRCHK(SetPanelAttribute(panel,ATTR_DIMMED, 1));
					UIERRCHK(ProcessDrawEvents()); 
					UIERRCHK(GetCtrlVal(panel, PANEL_PRE_IMPEDANCE_A, (int *) &impedanceTypeA));
					switch (impedanceTypeA)
					{
						case RESISTANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_A, &RA));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_A, &tauA));
							XA = 2.0*PI*sourceSettings.realFrequency*tauA*RA;
							break;
						case CAPACITANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_A, &CA));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_A, &DA));
							BA = 2.0*PI*sourceSettings.realFrequency*CA;
							GA = BA*DA;
							CxDiv(1.0, 0.0, GA, BA, &RA, &XA);
							break;
						case INDUCTANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_A, &LA));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_A, &RA));
							XA = 2.0*PI*sourceSettings.realFrequency*LA;
							break;
						case IMPEDANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_A, &RA));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_A, &XA));
							break;
						case ADMITTANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_A, &GA));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_A, &BA));
							CxDiv(1.0, 0.0, GA, BA, &RA, &XA);
							break;
					}
					UIERRCHK(GetCtrlVal(panel, PANEL_PRE_IMPEDANCE_B, (int *) &impedanceTypeB));
					switch (impedanceTypeB)
					{
						case RESISTANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_B, &RB));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_B, &tauB));
							XB = 2.0*PI*sourceSettings.realFrequency*tauB*RB;
							break;
						case CAPACITANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_B, &CB));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_B, &DB));
							BB = 2.0*PI*sourceSettings.realFrequency*CB;
							GB = BB*DB;
							CxDiv(1.0, 0.0, GB, BB, &RB, &XB);
							break;
						case INDUCTANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_B, &LB));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_B, &RB));
							XB = 2.0*PI*sourceSettings.realFrequency*LB;
							break;
						case IMPEDANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_B, &RB));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_B, &XB));
							break;
						case ADMITTANCE:
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_PRIMARY_B, &GB));
							UIERRCHK(GetCtrlVal(panel, PANEL_PRE_SECONDARY_B, &BB));
							CxDiv(1.0, 0.0, GB, BB, &RB, &XB);
							break;
					}
					ToPolar(RA, XA, &impedanceAMagnitude, &impedanceAPhase);
					ToPolar(RB, XB, &impedanceBMagnitude, &impedanceBPhase);
					ToPolar(RA+modeSettingsTmp.bridgeSettings.seriesResistance[CURRENT_CHANNEL_A], XA, &impedanceALoadedMagnitude, &impedanceALoadedPhase);
					ToPolar(RB+modeSettingsTmp.bridgeSettings.seriesResistance[CURRENT_CHANNEL_B], XB, &impedanceBLoadedMagnitude, &impedanceBLoadedPhase);
					
					UIERRCHK(GetCtrlVal(panel, PANEL_PRE_RMS_CURRENT, &rmsCurrent));
					currentAmplitude = rmsCurrent*sqrt(2.0);
					
					modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A]].amplitude = \
							impedanceAMagnitude*currentAmplitude;
					modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B]].amplitude = \
							impedanceBMagnitude*currentAmplitude;
					modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]].amplitude = \
							impedanceALoadedMagnitude*currentAmplitude;
					modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]].amplitude = \
							impedanceBLoadedMagnitude*currentAmplitude;
					
					if ((impedanceAPhase-impedanceBPhase) > 0) {
						currentPhase = -(impedanceAPhase+impedanceBPhase+PI)/2;
						modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B]].phase = \
								impedanceBPhase+currentPhase+PI;
						modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]].phase = \
								impedanceBLoadedPhase+currentPhase+PI;
					} else {
						currentPhase = -(impedanceAPhase+impedanceBPhase-PI)/2;
						modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B]].phase = \
								impedanceBPhase+currentPhase-PI;
						modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]].phase = \
								impedanceBLoadedPhase+currentPhase-PI;
					}
										
					modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A]].phase = \
							impedanceAPhase+currentPhase;
					modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]].phase = \
							impedanceALoadedPhase+currentPhase;
					
					DADSS_RangeList tmpRangeA = DADSS_GetMinimumRange(modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A]].amplitude);
					DADSS_RangeList tmpRangeB = DADSS_GetMinimumRange(modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B]].amplitude);
					sourceSettingsTmp.range[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A]] = (tmpRangeA > tmpRangeB) ? tmpRangeA : tmpRangeB;
					sourceSettingsTmp.range[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B]] = sourceSettingsTmp.range[modeSettingsTmp.bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A]];
					
					tmpRangeA = DADSS_GetMinimumRange(modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]].amplitude);
					tmpRangeB = DADSS_GetMinimumRange(modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]].amplitude);
					sourceSettingsTmp.range[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]] = (tmpRangeA > tmpRangeB) ? tmpRangeA : tmpRangeB;
					sourceSettingsTmp.range[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]] = sourceSettingsTmp.range[modeSettingsTmp.bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]];

					for (int i = 0; i <= CURRENT_CHANNEL_B; ++i) {
						if (sourceSettingsTmp.range[modeSettings[0].bridgeSettings.channelAssignment[i]] == DADSS_OVERRANGE) {
							warn("%s", msgStrings[MSG_PRESET_OVERRANGE]);
							UIERRCHK(SetPanelAttribute(panel,ATTR_DIMMED, 0));
							UIERRCHK(ProcessDrawEvents());
							return 0;
						}
					}
					
					for (int i = 0; i <= CURRENT_CHANNEL_B; ++i) {
						ToRect(modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[i]].amplitude, 
						   modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[i]].phase,
						   &modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[i]].real,
						   &modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[i]].imag);
						
						modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[i]].mdac2Code = DADSS_MDAC2_CODE_MAX;
						DADSS_Mdac2CodeToValue(modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[i]].mdac2Code, 
											   &modeSettingsTmp.channelSettings[modeSettingsTmp.bridgeSettings.channelAssignment[i]].mdac2Val);
					}
					
					sourceSettings = sourceSettingsTmp;
					modeSettings[0] = modeSettingsTmp;
					
					for (int j = 1; j < sourceSettings.nModes; j++) {
						for (int i = 0; i <= CURRENT_CHANNEL_B; ++i) {
							modeSettings[j].channelSettings[modeSettings[j].bridgeSettings.channelAssignment[i]].amplitude = 
								modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[i]].amplitude;
							modeSettings[j].channelSettings[modeSettings[j].bridgeSettings.channelAssignment[i]].phase = 
								modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[i]].phase;
							modeSettings[j].channelSettings[modeSettings[j].bridgeSettings.channelAssignment[i]].real = 
								modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[i]].real;
							modeSettings[j].channelSettings[modeSettings[j].bridgeSettings.channelAssignment[i]].imag = 
								modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[i]].imag;
							modeSettings[j].channelSettings[modeSettings[j].bridgeSettings.channelAssignment[i]].mdac2Code = 
								modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[i]].mdac2Code;
							modeSettings[j].channelSettings[modeSettings[j].bridgeSettings.channelAssignment[i]].mdac2Val = 
								modeSettings[0].channelSettings[modeSettings[0].bridgeSettings.channelAssignment[i]].mdac2Val;
						}
						modeSettings[j].channelSettings[modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL]].amplitude = 0.0;
						modeSettings[j].channelSettings[modeSettings[j].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL]].phase = 0.0;
					}
					
					sourceSettings.isFileModified = 1;										
					
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
				case PANEL_PRE_CANCEL:
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK SetBridge (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	int settingsPanel;
	
	switch (event) {
		case EVENT_COMMIT:
			Point activeTableCell;
			UIERRCHK(GetActiveTableCell (panel, PANEL_MOD_LIST, &activeTableCell));
			
			UIERRCHK(settingsPanel = LoadPanel(0, panelsFile, PANEL_BRI));
			
			char buf[BUF_SZ];
			snprintf(buf, BUF_SZ, "%s (%s)", "Bridge Settings", modeSettings[activeTableCell.y].label);
			UIERRCHK(SetPanelAttribute(settingsPanel, ATTR_TITLE, buf));
			
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_VOLTAGE_CHANNEL_A, ATTR_MIN_VALUE, 1));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_VOLTAGE_CHANNEL_A, ATTR_MAX_VALUE, DADSS_CHANNELS));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_CURRENT_CHANNEL_A, ATTR_MIN_VALUE, 1));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_CURRENT_CHANNEL_A, ATTR_MAX_VALUE, DADSS_CHANNELS));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_VOLTAGE_CHANNEL_B, ATTR_MIN_VALUE, 1));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_VOLTAGE_CHANNEL_B, ATTR_MAX_VALUE, DADSS_CHANNELS));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_CURRENT_CHANNEL_B, ATTR_MIN_VALUE, 1));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_CURRENT_CHANNEL_B, ATTR_MAX_VALUE, DADSS_CHANNELS));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_KELVIN_ARM_CHANNEL, ATTR_MIN_VALUE, 1));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_KELVIN_ARM_CHANNEL, ATTR_MAX_VALUE, DADSS_CHANNELS));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_INJECTION_CHANNEL, ATTR_MIN_VALUE, 1));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_BRI_INJECTION_CHANNEL, ATTR_MAX_VALUE, DADSS_CHANNELS));
			
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_VOLTAGE_CHANNEL_A, modeSettings[activeTableCell.y].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_A]+1));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_CURRENT_CHANNEL_A, modeSettings[activeTableCell.y].bridgeSettings.channelAssignment[CURRENT_CHANNEL_A]+1));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_VOLTAGE_CHANNEL_B, modeSettings[activeTableCell.y].bridgeSettings.channelAssignment[VOLTAGE_CHANNEL_B]+1));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_CURRENT_CHANNEL_B, modeSettings[activeTableCell.y].bridgeSettings.channelAssignment[CURRENT_CHANNEL_B]+1));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_KELVIN_ARM_CHANNEL, modeSettings[activeTableCell.y].bridgeSettings.channelAssignment[KELVIN_ARM_CHANNEL]+1));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_INJECTION_CHANNEL, modeSettings[activeTableCell.y].bridgeSettings.channelAssignment[INJECTION_CHANNEL]+1));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_VOLTAGE_RESISTANCE_A, modeSettings[activeTableCell.y].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_A]));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_CURRENT_RESISTANCE_A, modeSettings[activeTableCell.y].bridgeSettings.seriesResistance[CURRENT_CHANNEL_A]));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_VOLTAGE_RESISTANCE_B, modeSettings[activeTableCell.y].bridgeSettings.seriesResistance[VOLTAGE_CHANNEL_B]));
			UIERRCHK(SetCtrlVal(settingsPanel, PANEL_BRI_CURRENT_RESISTANCE_B, modeSettings[activeTableCell.y].bridgeSettings.seriesResistance[CURRENT_CHANNEL_B]));
			
			UIERRCHK(InstallPopup(settingsPanel));
			
			int eventHandle = -1, ctrlHandle = -1;
			while (ctrlHandle != PANEL_BRI_OK || ctrlHandle != PANEL_BRI_CANCEL) {
				UIERRCHK(GetUserEvent(1, &eventHandle, &ctrlHandle));
				switch (ctrlHandle) {
					case PANEL_BRI_OK:
						BridgeSettings bridgeSettingsTmp;
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_VOLTAGE_CHANNEL_A, &bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_A]));
						--bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_A];
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_CURRENT_CHANNEL_A, &bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_A]));
						--bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_A];
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_VOLTAGE_CHANNEL_B, &bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_B]));
						--bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_B];
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_CURRENT_CHANNEL_B, &bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_B]));
						--bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_B];
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_KELVIN_ARM_CHANNEL, &bridgeSettingsTmp.channelAssignment[KELVIN_ARM_CHANNEL]));
						--bridgeSettingsTmp.channelAssignment[KELVIN_ARM_CHANNEL];
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_INJECTION_CHANNEL, &bridgeSettingsTmp.channelAssignment[INJECTION_CHANNEL]));
						--bridgeSettingsTmp.channelAssignment[INJECTION_CHANNEL];
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_VOLTAGE_RESISTANCE_A, &bridgeSettingsTmp.seriesResistance[VOLTAGE_CHANNEL_A]));
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_CURRENT_RESISTANCE_A, &bridgeSettingsTmp.seriesResistance[CURRENT_CHANNEL_A]));
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_VOLTAGE_RESISTANCE_B, &bridgeSettingsTmp.seriesResistance[VOLTAGE_CHANNEL_B]));
						UIERRCHK(GetCtrlVal(settingsPanel, PANEL_BRI_CURRENT_RESISTANCE_B, &bridgeSettingsTmp.seriesResistance[CURRENT_CHANNEL_B]));
						if (bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_A] ||
								bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_B] ||
								bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_B] ||
								bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[KELVIN_ARM_CHANNEL] ||
								bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[INJECTION_CHANNEL] ||
								bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_B] ||
								bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_B] ||
								bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[KELVIN_ARM_CHANNEL] ||
								bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_A] == bridgeSettingsTmp.channelAssignment[INJECTION_CHANNEL] ||
								bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_B] == bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_B] ||
								bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_B] == bridgeSettingsTmp.channelAssignment[KELVIN_ARM_CHANNEL] ||
								bridgeSettingsTmp.channelAssignment[VOLTAGE_CHANNEL_B] == bridgeSettingsTmp.channelAssignment[INJECTION_CHANNEL] ||
								bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_B] == bridgeSettingsTmp.channelAssignment[KELVIN_ARM_CHANNEL] || 
						   		bridgeSettingsTmp.channelAssignment[CURRENT_CHANNEL_B] == bridgeSettingsTmp.channelAssignment[INJECTION_CHANNEL] ||
						   		bridgeSettingsTmp.channelAssignment[KELVIN_ARM_CHANNEL] == bridgeSettingsTmp.channelAssignment[INJECTION_CHANNEL]) {
									warn("%s", msgStrings[MSG_EQUAL_CHANNELS]);
									continue;
						} else {
							modeSettings[activeTableCell.y].bridgeSettings = bridgeSettingsTmp;
							if (activeTableCell.y == sourceSettings.activeMode)
								modeSettings[0].bridgeSettings = bridgeSettingsTmp;
						}
						UIERRCHK(RemovePopup(0));
						UIERRCHK(DiscardPanel(settingsPanel));
						return 0;
					case PANEL_BRI_CANCEL:
						UIERRCHK(RemovePopup(0));
						UIERRCHK(DiscardPanel(settingsPanel));
						return 0;
					default:
						break;
				}
			}
		default:
			break;
	}
	return 0;	
}

int CVICALLBACK SetSwitchPorts (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	int settingsPanel;
	
	switch (event)	{
		case EVENT_COMMIT:
			Point activeTableCell;
			UIERRCHK(GetActiveTableCell (panel, PANEL_MOD_LIST, &activeTableCell));
			
			UIERRCHK(settingsPanel = LoadPanel(0, panelsFile, PANEL_SWI));
						
			char buf[BUF_SZ];
			snprintf(buf, BUF_SZ, "%s (%s)", "Switch Ports", modeSettings[activeTableCell.y].label);
			UIERRCHK(SetPanelAttribute(settingsPanel, ATTR_TITLE, buf));
			
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_SWI_PORTS, ATTR_DISABLE_PANEL_THEME, 1));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_SWI_PORTS, ATTR_ENABLE_ROW_SIZING, 0));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_SWI_PORTS, ATTR_ENABLE_COLUMN_SIZING, 0)); 
			
			for (int i = 0; i < DIO_PORTS; ++i) {
				for (int j = 0; j < DIO_PORT_BITS; ++j) {
					if (modeSettings[activeTableCell.y].switchPorts[i] & (1 << j)) {
						UIERRCHK(SetTableCellAttribute(settingsPanel, PANEL_SWI_PORTS, MakePoint(DIO_PORT_BITS-j, i+1), ATTR_CMD_BUTTON_COLOR, VAL_BLACK));						
					} else {
						UIERRCHK(SetTableCellAttribute(settingsPanel, PANEL_SWI_PORTS, MakePoint(DIO_PORT_BITS-j, i+1), ATTR_CMD_BUTTON_COLOR, VAL_PANEL_GRAY));	
					}
				}
			}
			
			unsigned char switchPortsTmp[DIO_PORTS] = {0};
			for (int i = 0; i < DIO_PORTS; ++i) {
				switchPortsTmp[i] = modeSettings[activeTableCell.y].switchPorts[i];
			}

			UIERRCHK(InstallPopup(settingsPanel));
			
			int eventHandle = -1, ctrlHandle = -1;
			while (ctrlHandle != PANEL_SWI_OK || ctrlHandle != PANEL_SWI_CANCEL) {
				UIERRCHK(GetUserEvent(1, &eventHandle, &ctrlHandle));
				switch (ctrlHandle) {
					case PANEL_SWI_PORTS:
						Point activePortCell;
						UIERRCHK(GetActiveTableCell(settingsPanel, ctrlHandle, &activePortCell));
						switchPortsTmp[activePortCell.y-1] ^= (1 << (DIO_PORT_BITS-activePortCell.x));
						if (switchPortsTmp[activePortCell.y-1] & (1 << (DIO_PORT_BITS-activePortCell.x))) {
							UIERRCHK(SetTableCellAttribute(settingsPanel, ctrlHandle, activePortCell, ATTR_CMD_BUTTON_COLOR, VAL_BLACK));						
						} else {
							UIERRCHK(SetTableCellAttribute(settingsPanel, ctrlHandle, activePortCell, ATTR_CMD_BUTTON_COLOR, VAL_PANEL_GRAY));	
						}
						break;
					case PANEL_SWI_OK:
						for (int i = 0; i < DIO_PORTS; ++i) {
							modeSettings[activeTableCell.y].switchPorts[i] = switchPortsTmp[i];
						}
						if (activeTableCell.y == sourceSettings.activeMode) {
							for (int i = 0; i < DIO_PORTS; ++i) {
								modeSettings[0].switchPorts[i] = switchPortsTmp[i];
							}
						}		
						UIERRCHK(RemovePopup(0));
						UIERRCHK(DiscardPanel(settingsPanel));
						return 0;
					case PANEL_SWI_CANCEL:
						UIERRCHK(RemovePopup(0));
						UIERRCHK(DiscardPanel(settingsPanel));
						return 0;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
	return 0;
}

int CVICALLBACK SetChannelSwitchModes (int panel, int control, int event,
									   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_CHSW_ADD:
					if (sourceSettings.nChannelSwitchModes < MAX_CHANNEL_SWITCH_MODES) {
						UIERRCHK(InsertTableRows(panel, PANEL_CHSW_LIST, -1, 1, VAL_CELL_STRING));
						SetDefaultChannelSwitchSettings(sourceSettings.nChannelSwitchModes);
						UIERRCHK(SetTableCellVal(panel, PANEL_CHSW_LIST, MakePoint(1,sourceSettings.nChannelSwitchModes+1), channelSwitchSettings[sourceSettings.nChannelSwitchModes].label));
						++sourceSettings.nChannelSwitchModes;
					}
					break;
				case PANEL_CHSW_DUPLICATE:
					if (sourceSettings.nChannelSwitchModes < MAX_CHANNEL_SWITCH_MODES) {
						Point activeTableCell;
						UIERRCHK(GetActiveTableCell (panel, PANEL_CHSW_LIST, &activeTableCell));
						UIERRCHK(InsertTableRows(panel, PANEL_CHSW_LIST, -1, 1, VAL_CELL_STRING));
						channelSwitchSettings[sourceSettings.nChannelSwitchModes] = channelSwitchSettings[activeTableCell.y-1];
						UIERRCHK(SetTableCellVal(panel, PANEL_CHSW_LIST, MakePoint(1,sourceSettings.nChannelSwitchModes+1), channelSwitchSettings[sourceSettings.nChannelSwitchModes].label));
						++sourceSettings.nChannelSwitchModes;
					}
					break;
				case PANEL_CHSW_REMOVE:
					if (sourceSettings.nChannelSwitchModes > 1) {
						UIERRCHK(DeleteTableRows(panel, PANEL_CHSW_LIST, sourceSettings.nChannelSwitchModes, -1));
						--sourceSettings.nChannelSwitchModes;
					}
					break;
				case PANEL_CHSW_LIST:
					UIERRCHK(GetTableCellVal(panel, PANEL_CHSW_LIST, MakePoint(1,eventData1), channelSwitchSettings[eventData1-1].label));
					break;
				case PANEL_CHSW_OK:
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
				default:
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK SetChannelSwitchPorts (int panel, int control, int event,
									   void *callbackData, int eventData1, int eventData2)
{
	int settingsPanel;
	
	switch (event)	{
		case EVENT_COMMIT:
			Point activeTableCell;
			UIERRCHK(GetActiveTableCell (panel, PANEL_CHSW_LIST, &activeTableCell));
			
			UIERRCHK(settingsPanel = LoadPanel(0, panelsFile, PANEL_SWI));
						
			char buf[BUF_SZ];
			snprintf(buf, BUF_SZ, "%s (%s)", "Channel Switch Ports", channelSwitchSettings[activeTableCell.y-1].label);
			UIERRCHK(SetPanelAttribute(settingsPanel, ATTR_TITLE, buf));
			
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_SWI_PORTS, ATTR_DISABLE_PANEL_THEME, 1));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_SWI_PORTS, ATTR_ENABLE_ROW_SIZING, 0));
			UIERRCHK(SetCtrlAttribute(settingsPanel, PANEL_SWI_PORTS, ATTR_ENABLE_COLUMN_SIZING, 0)); 
			
			for (int i = 0; i < DIO_PORTS; ++i) {
				for (int j = 0; j < DIO_PORT_BITS; ++j) {
					if (channelSwitchSettings[activeTableCell.y-1].switchPorts[i] & (1 << j)) {
						UIERRCHK(SetTableCellAttribute(settingsPanel, PANEL_SWI_PORTS, MakePoint(DIO_PORT_BITS-j, i+1), ATTR_CMD_BUTTON_COLOR, VAL_BLACK));						
					} else {
						UIERRCHK(SetTableCellAttribute(settingsPanel, PANEL_SWI_PORTS, MakePoint(DIO_PORT_BITS-j, i+1), ATTR_CMD_BUTTON_COLOR, VAL_PANEL_GRAY));	
					}
				}
			}
			
			unsigned char switchPortsTmp[DIO_PORTS] = {0};
			for (int i = 0; i < DIO_PORTS; ++i) {
				switchPortsTmp[i] = channelSwitchSettings[activeTableCell.y-1].switchPorts[i];
			}

			UIERRCHK(InstallPopup(settingsPanel));
			
			int eventHandle = -1, ctrlHandle = -1;
			while (ctrlHandle != PANEL_SWI_OK || ctrlHandle != PANEL_SWI_CANCEL) {
				UIERRCHK(GetUserEvent(1, &eventHandle, &ctrlHandle));
				switch (ctrlHandle) {
					case PANEL_SWI_PORTS:
						Point activePortCell;
						UIERRCHK(GetActiveTableCell(settingsPanel, ctrlHandle, &activePortCell));
						switchPortsTmp[activePortCell.y-1] ^= (1 << (DIO_PORT_BITS-activePortCell.x));
						if (switchPortsTmp[activePortCell.y-1] & (1 << (DIO_PORT_BITS-activePortCell.x))) {
							UIERRCHK(SetTableCellAttribute(settingsPanel, ctrlHandle, activePortCell, ATTR_CMD_BUTTON_COLOR, VAL_BLACK));						
						} else {
							UIERRCHK(SetTableCellAttribute(settingsPanel, ctrlHandle, activePortCell, ATTR_CMD_BUTTON_COLOR, VAL_PANEL_GRAY));	
						}
						break;
					case PANEL_SWI_OK:
						for (int i = 0; i < DIO_PORTS; ++i) {
							channelSwitchSettings[activeTableCell.y-1].switchPorts[i] = switchPortsTmp[i];
						}
						UIERRCHK(RemovePopup(0));
						UIERRCHK(DiscardPanel(settingsPanel));
						return 0;
					case PANEL_SWI_CANCEL:
						UIERRCHK(RemovePopup(0));
						UIERRCHK(DiscardPanel(settingsPanel));
						return 0;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
	return 0;
}

int CVICALLBACK SetActiveSwitchMode (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	ProgramState savedProgramState = programState;
	
	switch (event) {
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_ACTIVE_SWITCH_MODE:
					UIERRCHK(GetCtrlVal(panel, control, &modeSettings[0].channelSettings[sourceSettings.activeChannel].activeSwitchMode));
					programState = STATE_UPDATING;
					UpdatePanel(panel);
					DIOERRCHK(DIO_SetAllPorts());
					programState = savedProgramState;
					UpdatePanel(panel);
					break;
				default:
					break;
			}
			break;
	}
	return 0;

Error:
	lockinSettings.isBalanceInterrupted = 1;
	programState = savedProgramState;
	UpdatePanel(panel);
	return 0;
}

int CVICALLBACK SetRecord (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
		case EVENT_COMMIT:
			switch (control) {
				case PANEL_REC_OK:
					UIERRCHK(GetCtrlVal(panel, PANEL_REC_DATA_FILE_NAME, recordSettings.dataPathName)); 
					UIERRCHK(GetCtrlVal(panel, PANEL_REC_SAMPLES, &recordSettings.nSamples)); 
					UIERRCHK(GetCtrlVal(panel, PANEL_REC_SAMPLING_PERIOD, &recordSettings.samplingPeriod));
					
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
				case PANEL_REC_CANCEL:
					UIERRCHK(RemovePopup(0));
					UIERRCHK(DiscardPanel(panel));
					UIERRCHK(ProcessDrawEvents());
					UIERRCHK(panel = GetActivePanel());
					UpdatePanel(panel);
					break;
				case PANEL_REC_BROWSE:
						char dataPathName[MAX_PATHNAME_LEN];
						int ret = FileSelectPopup("", 
												  "",
												  "*.*",
												  msgStrings[MSG_POPUP_RECORD_DATA_FILE],
												  VAL_OK_BUTTON,
												  0,
												  0,
												  1,
												  1,
												  dataPathName);
						switch (ret) {
							case VAL_NO_FILE_SELECTED:
								break;
							case VAL_EXISTING_FILE_SELECTED:
							case VAL_NEW_FILE_SELECTED:
								UIERRCHK(SetCtrlVal(panel, PANEL_REC_DATA_FILE_NAME, dataPathName));
								break;
						}
			}
			break;
	}
	return 0;
}

int CVICALLBACK CloseHelp (int panel, int event, void *callbackData,
									 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			UIERRCHK(DiscardPanel(panel));
			break;
	}
	return 0;
}

int CVICALLBACK RecordLockin (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			programState = STATE_RECORDING;
			UpdatePanel(panel);
			
			double fileTimeStamp;
			char fileTimeStampBuf[16]; // YYYYMMDDTHHMMSS
			GetCurrentDateTime(&fileTimeStamp);
			FormatDateTimeString(fileTimeStamp, "%Y%m%dT%H%M%S", fileTimeStampBuf, sizeof fileTimeStampBuf);
			
			char dataPathName[MAX_PATHNAME_LEN];
			strncpy(dataPathName, recordSettings.dataPathName, MAX_PATHNAME_LEN);
			strncat(dataPathName, "_", MAX_PATHNAME_LEN);
			strncat(dataPathName, modeSettings[0].label, MAX_PATHNAME_LEN);
			strncat(dataPathName, "_", MAX_PATHNAME_LEN);
			strncat(dataPathName, channelSwitchSettings[modeSettings[0].channelSettings[sourceSettings.activeChannel].activeSwitchMode].label, MAX_PATHNAME_LEN);
			strncat(dataPathName, "_", MAX_PATHNAME_LEN);
			strncat(dataPathName, fileTimeStampBuf, MAX_PATHNAME_LEN);
			strncat(dataPathName, ".dat", MAX_PATHNAME_LEN);
			recordSettings.dataFileHandle = fopen(dataPathName, "a");
    		if(recordSettings.dataFileHandle == NULL) {
        		warn("%s %s", msgStrings[MSG_SAVING_ERROR], dataPathName);
				programState = STATE_RUNNING;
				UpdatePanel(panel);
        		return 0;
    		}
			
			if (modeSettings[0].channelSettings[sourceSettings.activeChannel].gainMode == LIANV_GAIN_AUTO) {
				UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 1));
				LIAERRCHK(LIANV_GetMeasureXYWithAutoGain(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading));
				UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 0));
			}
			recordSettings.sampleCount = 0;
			double serverSamplingPeriod;
			LIAERRCHK(LIANV_GetXYSamplingRate(lockinSettings.nvProcessName, &serverSamplingPeriod));
			double samplingPeriod = recordSettings.samplingPeriod < serverSamplingPeriod ? serverSamplingPeriod : serverSamplingPeriod*RoundRealToNearestInteger(recordSettings.samplingPeriod/serverSamplingPeriod);
			UIERRCHK(SetCtrlAttribute(panel, PANEL_RECORD_TIMER, ATTR_INTERVAL, samplingPeriod));
			UIERRCHK(SetCtrlAttribute(panel, PANEL_RECORD_TIMER, ATTR_ENABLED, 1));
			break;
	}
	return 0;
	
Error:
	UIERRCHK(SetCtrlVal(panel, PANEL_LOCKIN_AUTORANGE_LED, 0));
	fclose(recordSettings.dataFileHandle);
	recordSettings.dataFileHandle = NULL;
	programState = STATE_RUNNING;
	UpdatePanel(panel);
	return 0;
}

int CVICALLBACK RecordLockinOnTimer (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{	
	switch (event)	{
		case EVENT_TIMER_TICK:
			double timeStamp, timeElapsed;
			
			UIERRCHK(GetTimerTickData(&timeStamp, &timeElapsed));
			if (recordSettings.sampleCount == 0) {
				recordSettings.startTime = timeStamp;
			}
			LIAERRCHK(LIANV_GetMeasureXY(lockinSettings.nvProcessName, modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading));
			UpdatePanelLockinReading(panel, 
									 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
									 modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1]);
			if (fprintf(recordSettings.dataFileHandle,"%.15g\t%.15g\t%.15g\n", timeStamp-recordSettings.startTime, 
						modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[0], 
						modeSettings[0].channelSettings[sourceSettings.activeChannel].lockinReading[1]) < 0) {
				warn("%s %s", msgStrings[MSG_SAVING_ERROR], recordSettings.dataPathName);
				goto Error;
			}
			if (++recordSettings.sampleCount == recordSettings.nSamples) {
				UIERRCHK(SetCtrlAttribute(panel, PANEL_RECORD_TIMER, ATTR_ENABLED, 0));
				fclose(recordSettings.dataFileHandle);
				recordSettings.dataFileHandle = NULL;
				Beep();
				programState = STATE_RUNNING;
				UpdatePanel(panel);
			}
			break;
	}
	return 0;
	
Error:
	UIERRCHK(SetCtrlAttribute (panel, PANEL_RECORD_TIMER, ATTR_ENABLED, 0));	
	fclose(recordSettings.dataFileHandle);
	recordSettings.dataFileHandle = NULL;
	programState = STATE_RUNNING;
	UpdatePanel(panel);
	return 0;
}

static int DIO_SetAllPorts(void)
{
	for (int i = 0; i < DIO_PORTS; ++i) {
		int ret;
		if ((ret = DIO_SetPort(i, modeSettings[0].switchPorts[i]^channelSwitchSettings[modeSettings[0].channelSettings[sourceSettings.activeChannel].activeSwitchMode].switchPorts[i])) < 0) {
			return ret;
		}
	}
	return 0;
}

static int DIO_ResetAllPorts(void)
{
	for (int i = 0; i < DIO_PORTS; ++i) {
		int ret;
		if ((ret = DIO_SetPort(i, 0)) < 0) {
			return ret;
		}
	}
	return 0;
}
