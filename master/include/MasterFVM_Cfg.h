#ifndef _MASTERFVM_CFG_H_
#define _MASTERFVM_CFG_H_

#include "MasterFVM_Types.h"

#define NUM_RESET 2
#define NUM_ECU 2

uint8 resetData[3 * NUM_RESET];

ConfirmECU_Type confirmECU[NUM_RESET];

ResetCnt_Type resetCnt[NUM_RESET];

#endif