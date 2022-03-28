//==============================================================================
// Title:   	LIA_NV_cvi_driver.h
// Purpose:   	A short description of the interface.
//
// Created on:  25.10.2019 at 22:42:17 by Janusz Kaczmarek.
// 				University of Zielona Góra.
//==============================================================================

		
#include "cvidef.h"


/* = LIA_NV_cvi_driver.h Include File ===================================== */

//------------- General functions of LIA_NV  ----------------------
void CVIFUNC LIANV_SetNameNVServer (char  name[]);

//------------- LIA configuration functions ------------------

int CVIFUNC LIANV_SetSensitivity (char *nv_process_name, int sensitivity);
int CVIFUNC LIANV_GetSensitivity (char *nv_process_name, int *sensitivity);

int CVIFUNC LIANV_SetTimeConstant (char *nv_process_name, int time_constant);
int CVIFUNC LIANV_GetTimeConstant (char *nv_process_name, int *time_constant);

int CVIFUNC LIANV_SetLowPassFilterSlope (char *nv_process_name, int low_pass_filter_slope);
int CVIFUNC LIANV_GetLowPassFilterSlope (char *nv_process_name, int *low_pass_filter_slope);

int CVIFUNC LIANV_SetReserveMode (char *nv_process_name, int reserve_mode);
int CVIFUNC LIANV_GetReserveMode (char *nv_process_name, int *reserve_mode);

int CVIFUNC LIANV_SetInputConfiguration (char *nv_process_name, int input_config);
int CVIFUNC LIANV_GetInputConfiguration (char *nv_process_name, int *input_config);

int CVIFUNC LIANV_SetInputShieldGrounding (char *nv_process_name, int input_shield_grounding);
int CVIFUNC LIANV_GetInputShieldGrounding (char *nv_process_name, int *input_shield_grounding);

int CVIFUNC LIANV_SetInputCoupling (char *nv_process_name, int input_coupling);
int CVIFUNC LIANV_GetInputCoupling (char *nv_process_name, int *input_coupling);

int CVIFUNC LIANV_SetInputLineNotchFilter (char *nv_process_name, int input_lnf);
int CVIFUNC LIANV_GetInputLineNotchFilter (char *nv_process_name, int *input_lnf);

//------------- LIA reading measure values  --------------- 
int CVIFUNC LIANV_GetMeasureXY (char *nv_process_name,  double xy_values[]);
int CVIFUNC LIANV_GetXYSamplingRate (char *nv_process_name,  double *sampling_rate);

//------------- LIA reading status  --------------- 
int CVIFUNC LIANV_GetDeviceStatus (char *nv_process_name, char *status_byte_name,  unsigned int *device_status);

