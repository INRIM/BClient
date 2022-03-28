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

#ifndef __PANEL_H__
#define __PANEL_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

void InitPanelAttributes(int);
void UpdatePanel(int);
void UpdatePanelTitle(int);
void UpdatePanelMenuBar(int);
void UpdatePanelSource(int);
void UpdatePanelActiveMode(int);
void UpdatePanelActiveChannel(int);
void UpdatePanelSwitchActiveMode(int);
void UpdatePanelWaveformParameters(int);
void UpdatePanelLockin(int);
void UpdatePanelLockinReading(int, double, double); 
void UpdatePanelBalance(int);

#ifdef __cplusplus
    }
#endif

#endif 
