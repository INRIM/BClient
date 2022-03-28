#include "cvidef.h"
#include "stdint.h"


/* = DIO Include File ===================================== */

//------------- General functions of Digital I/O Module  ----------
void CVIFUNC DIO_SetNameNVServer (char *nv_server_name);
void CVIFUNC DIO_SetNameNVProcess (char *nv_process_name);

//--- Functions of data control of otput port of Digital I/O Module  ---
int CVIFUNC DIO_SetPort (unsigned int port, unsigned char output_data);

int CVIFUNC DIO_GetPortStatus (unsigned int port, uint8_t *output_data_status);

