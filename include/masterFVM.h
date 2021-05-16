//
// Created by zhao chenyang on 2021/5/16.
//

#ifndef SECOC_MASTERFVM_H
#define SECOC_MASTERFVM_H

#include "Compiler.h"

// void MasterFVM_Init(void);
FUNC(void, MASTER_CODE)
MasterFVM_Init();

// void MasterFVM_getTripValue(const PduInfoType* PduInfoPtr ); //   获取trip及hash结果
FUNC(void, MASTER_CODE)
MasterFVM_getTripValue(P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr);

// void MasterFVM_getResetValue(PduIdType TxPduId, const PduInfoType* PduInfoPtr); //   获取reset及hash结果
FUNC(void, MASTER_CODE)
MasterFVM_getResetValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr);

// void MasterFVM_changestate(PduIdType TxPduId);
FUNC(void, MASTER_CODE)
MasterFVM_changestate(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId);

// void MasterFVM_MainTx(void);
FUNC(void, MASTER_CODE)
        MasterFVM_MainTx();


#endif //SECOC_MASTERFVM_H
