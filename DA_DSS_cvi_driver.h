#include "cvidef.h"


/* = DA_DSS Include File ===================================== */

//------------- General functions of DA_DSS  ----------------------
void CVIFUNC DADSS_SetNameNVServer (char *nv_server_name);
void CVIFUNC DADSS_SetNameNVProcess (char *nv_process_name);

//------------- Functions of DA_DSS Configuration ------------------
int CVIFUNC DADSS_SetRangeMDAC1 (int channel, unsigned int range);

int CVIFUNC DADSS_GetRangeMDAC1 (int channel, unsigned int *range);

int CVIFUNC DADSS_SetRangeMDAC2 (int channel, unsigned int range);

int CVIFUNC DADSS_GetRangeMDAC2 (int channel, unsigned int *range);

int CVIFUNC DADSS_SetCLKSource (unsigned int  source);

int CVIFUNC DADSS_GetCLKSource (unsigned int  *source);

int CVIFUNC DADSS_SetCLKExport10MHz (unsigned int  export);

int CVIFUNC DADSS_GetCLKExport10MHzStatus (unsigned int  *export);   

int CVIFUNC DADSS_UpdateConfiguration (void);

//------------- Functions of Main Settings of DA_DSS --------------- 

int CVIFUNC DADSS_SetAmplitude (int channel, double amplitude);

int CVIFUNC DADSS_GetAmplitude (int channel, double *amplitude);

int CVIFUNC DADSS_SetPhase (int channel, double phase);

int CVIFUNC DADSS_GetPhase (int channel, double *phase);

int CVIFUNC DADSS_UpdateWaveform (void); 

//---------------- Advanced ----------------------------------------
int CVIFUNC DADSS_SetMDAC1Amplitude (int channel, double amplitude);

int CVIFUNC DADSS_GetMDAC1Amplitude (int channel, double *amplitude);

int CVIFUNC DADSS_SetMDAC1Offset (int channel, int  setting);

int CVIFUNC DADSS_GetMDAC1Offset (int channel, int *setting);

int CVIFUNC DADSS_SetMDAC2 (int channel, unsigned int setting);

int CVIFUNC DADSS_GetMDAC2 (int channel, unsigned int *setting);

int CVIFUNC DADSS_GetAmplitudeMax (int channel, double *amplitudeMax); 

int CVIFUNC DADSS_GetWaveform (int channel, int waveform[], int numSamples); 

int CVIFUNC DADSS_UpdateMDAC2 (void); 

//---------------- NCO Functions ____________________________________

int CVIFUNC DADSS_SetFrequency (double frequency); 

int CVIFUNC DADSS_GetFrequency (double *frequency);

int CVIFUNC DADSS_GetRealFrequency (double *realFrequency); 

int CVIFUNC DADSS_SetCLKFrequency (double frequency);

int CVIFUNC DADSS_GetCLKFrequency (double *frequency);

int CVIFUNC DADSS_SetNumberSamples (int numSamples); 

int CVIFUNC DADSS_GetNumberSamples (int *numSamples);

//---------------Functions of Generation Process 

int CVIFUNC DADSS_StartStop (unsigned char startStop);

int CVIFUNC DADSS_StartStop_Status (unsigned char *startStop);
