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

#ifndef CFG_H
#define CFG_H

#ifdef __cplusplus
	extern "C" {
#endif

//==============================================================================
// Include files
		
#include <inifile.h>
		
#include "main.h"
		
//==============================================================================
// Constants

//==============================================================================
// Types
		
typedef struct {
	IniText iniText;
	const char *fileName;
} SettingsFileDescriptor;
		
//==============================================================================
// External variables

extern SourceSettings sourceSettings;
extern ModeSettings modeSettings[];
extern LockinSettings lockinSettings;
extern SwitchSettings switchSettings;
extern ChannelSwitchSettings channelSwitchSettings[];
extern RecordSettings recordSettings;

extern const char defaultSettingsFileName[];
extern char defaultSettingsFile[]; 
extern char defaultSettingsFileDir[];

//==============================================================================
// Global functions

void SetDefaultSettings(void);
void SetDefaultModeSettings(int);
void SetDefaultChannelSwitchSettings(int);
void LoadSettings(const char *);
void SaveSettings(const char *);

#ifdef __cplusplus
	}
#endif

#endif /* CFG_H */
