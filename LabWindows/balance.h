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


#ifndef __balance_H__
#define __balance_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "toolbox.h" 

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

extern ListType balanceTwoPort;
extern ListType balanceVersical;
extern ListType balanceVersicalNoKelvinArm;
extern ListType balanceGiqs;
extern ListType balanceGiqsNoKelvinArm;

//==============================================================================
// Global functions

void InitBalanceProcedures(void);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __balance_H__ */
