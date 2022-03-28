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

#ifndef MSG_H
#define MSG_H

#ifdef __cplusplus
	extern "C" {
#endif
		
//==============================================================================
// Include files

//==============================================================================
// Constants

#define MSG_BUF_SZ 1024

#define UIERRCHK(fCall) {\
	int retTmp;\
	if ((retTmp = (fCall)) < 0) {\
		die("%s\nFile: %s\nFunction: %s\nLine: %d", GetGeneralErrorString(retTmp), __FILE__, __PRETTY_FUNCTION__, __LINE__); \
	}\
}

#define DSSERRCHK(fCall) {\
	int retTmp;\
	if ((retTmp = (fCall)) < 0) {\
		warn("%s: %d", msgStrings[MSG_DSS_ERROR], retTmp);\
		goto Error;\
	}\
}
		
#define LIAERRCHK(fCall) {\
	int retTmp;\
	if ((retTmp = (fCall)) < 0) {\
		warn("%s: %d", msgStrings[MSG_LIANV_ERROR], retTmp);\
		goto Error;\
	}\
}		
		
#define DIOERRCHK(fCall) {\
	int retTmp;\
	if ((retTmp = (fCall)) < 0) {\
		warn("%s: %d", msgStrings[MSG_DIO_ERROR], retTmp);\
		goto Error;\
	}\
}

#define ALERRCHK(fCall) {\
	int retTmp;\
	if ((retTmp = (fCall)) != 0) {\
		warn("%s: %s", msgStrings[MSG_ANALYSIS_ERROR], GetAnalysisErrorString(retTmp)); \
		goto Error;\
	}\
}
		
//==============================================================================
// Types
		
enum MESSAGES {
	MSG_TITLE,
	MSG_VERSION,
	MSG_FATAL_ERROR,
	MSG_INTERNAL_ERROR,
	MSG_WARNING,
	MSG_OUT_OF_MEMORY,
	MSG_MAIN_PANEL_ERROR,
	MSG_LOADING_ERROR,
	MSG_SAVING_ERROR,
	MSG_FILE_ALREADY_EXISTS,
	MSG_SETTINGS_MISSING_PARAMETER,
	MSG_SETTINGS_PARAMETER_OUT_OF_RANGE,
	MSG_SETTINGS_SECTION,
	MSG_POPUP_CONFIRM_TITLE,
	MSG_POPUP_LOAD_SETTINGS_TITLE,
	MSG_POPUP_SAVE_SETTINGS_TITLE,
	MSG_POPUP_RESET_SETTINGS,
	MSG_POPUP_QUIT,
	MSG_DSS_ERROR,
	MSG_DSS_ALREADY_RUNNING,
	MSG_LIANV_ERROR,
	MSG_DIO_ERROR,
	MSG_ANALYSIS_ERROR,
	MSG_UNLOCK_CHANNEL,
	MSG_CONNECTING_TITLE,
	MSG_MAX_AUTOZERO_STEPS,
	MSG_CANNOT_REMOVE_ACTIVE_MODE,
	MSG_POPUP_NEW_FILE_TITLE,
	MSG_POPUP_SAVEAS_FILE_TITLE,
	MSG_EQUAL_CHANNELS,
	MSG_PRESET_OVERRANGE,
	MSG_POPUP_RECORD_DATA_FILE,
	MSG_CANNOT_BALANCE_AUTOMATICALLY,
	MSG_MODE_NOT_FOUND,
	MSG_SWITCH_MODE_NOT_FOUND,
	MSG_MAX_FULL_BALANCE_STEPS,
};

enum TOOLTIPS {
	TOOLTIP_RANGE,
	TOOLTIP_CON2_NV_SERVER_NAME,
	TOOLTIP_CON2_CHECK,
	TOOLTIP_REC_SAMPLING_PERIOD,
	TOOLTIP_REC_DATA_FILE_NAME,
};

//==============================================================================
// External variables

extern const char *msgStrings[];
extern const char *tooltipStrings[];
const char *balanceTargetStrings[];

//==============================================================================
// Global functions

void die(const char *, ...);
void warn(const char *, ...);

#ifdef __cplusplus
	}
#endif

#endif /* MSG_H */ 
