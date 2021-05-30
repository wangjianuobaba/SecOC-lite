//
// Created by zhao chenyang on 2021/5/16.
//

#ifndef _SECOC_MASTERFVM_H
#define _SECOC_MASTERFVM_H

#include "MasterFVM_Cfg.h"
#include "Compiler.h"
#include "tools.h"
#include "bitmap.h"
#include "MacGenerate.h"
#include <string.h>


uint8 TripCntLength; //可配置
uint8 trip[3];

// void MasterFVM_Init(void);
FUNC(void, MASTER_CODE)
MasterFVM_Init();

// void MasterFVM_getTripValue(const PduInfoType* PduInfoPtr ); //   获取trip及hash结果
FUNC(void, MASTER_CODE)
MasterFVM_getTripValue(P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr);

// void MasterFVM_getResetValue(PduIdType TxPduId, const PduInfoType* PduInfoPtr); //   获取reset及hash结果
FUNC(void, MASTER_CODE)
MasterFVM_getResetValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr);


#endif //SECOC_MASTERFVM_H
