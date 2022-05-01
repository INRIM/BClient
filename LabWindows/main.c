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
#include "balance.h"
#include "DA_DSS_cvi_driver.h" 
#include "LIA_NV_cvi_driver.h"
#include "DIO_cvi_driver.h"

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

const char panelsFile[] = "panels.uir";
ProgramState programState = STATE_IDLE;

//==============================================================================
// Global functions

int main (int argc, char *argv[])
{
	// Initialize
	if (InitCVIRTE(0, argv, 0) == 0)
		return -1; // Out of memory
	
	// Check for duplicate instances
	int isDuplicate;
	if (CheckForDuplicateAppInstance(ACTIVATE_OTHER_INSTANCE, &isDuplicate) < 0)
		return -1; // Out of memory
	if (isDuplicate)
		return 0; // Prevent duplicate instance

	InitBalanceProcedures();
	SetDefaultSettings();

	// Load the configuration file (.ini), if existing
	int settingsPathFound = 0;
	if (GetProjectDir(defaultSettingsFileDir) == 0 &&
			MakePathname(defaultSettingsFileDir, defaultSettingsFileName, defaultSettingsFile) == 0)
		settingsPathFound = 1;
	if (settingsPathFound && FileExists(defaultSettingsFile, 0)) 
		LoadSettings(defaultSettingsFile);
	
	UIERRCHK(SetSystemAttribute(ATTR_REPORT_LOAD_FAILURE, 0)); 
	int panel = LoadPanel(0, panelsFile, PANEL); 
	if (panel < 0)
		die("%s\n%s: %s", msgStrings[MSG_MAIN_PANEL_ERROR], GetUILErrorString(panel), panelsFile);

	// Initialize the panel
	InitPanelAttributes(panel);
	UpdatePanel(panel);
	UpdatePanelLockinReading(panel, 0.0, 0.0);
	
	// Display the panel and run the user interface
	UIERRCHK(DisplayPanel(panel));
	UIERRCHK(SetSleepPolicy(VAL_SLEEP_NONE));
	int status = RunUserInterface();
	UIERRCHK(DiscardPanel(panel));
	
	// Save the configuration file and exit
	if (settingsPathFound)
		SaveSettings(defaultSettingsFile);
	CloseCVIRTE();
	exit(status);
}
