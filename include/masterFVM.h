//
// Created by zhao chenyang on 2021/5/16.
//

#ifndef SECOC_MASTERFVM_H
#define SECOC_MASTERFVM_H

#include "Compiler.h"
#include "tools.h"
#include "bitmap.h"

extern uint8 trip[3];
extern uint8 TripCntLength;

// masterFVM_Types.h
typedef struct
{
    uint8 *resetdata;
    uint8 ResetCntLength;
    uint16 resetcanid;
    uint32 resetSyntag;
    uint32 resetSynTime;
} ResetCnt_Type;

ResetCnt_Type resetCnt[2];

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
