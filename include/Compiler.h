//
// Created by zhao chenyang on 2021/5/16.
//

#ifndef SECOC_MASTERFVM_CFG_H
#define SECOC_MASTERFVM_CFG_H

#include "platform_Types.h"

#define FUNC(rettype, memclass)                 rettype memclass
// p2const 未添加memclass（在gcc下，添加了会报错）
#define P2CONST(ptrtype, memclass, ptrclass)    const ptrtype ptrclass *
#define P2VAR(ptrtype, memclass, ptrclass)      ptrtype ptrclass *

#define CONST(type, memclass)                   memclass const type
#define VAR(type, memclass)                     memclass type

//-------------- Master Define --------------
#define MASTER_CODE
#define SECOC_APPL_DATA
#define COMSTACK_TYPES_VAR
#define NUM_RESET 2

typedef uint8 PduLengthType;
typedef uint8 PduIdType;

typedef struct {
    uint8 *SduDataPtr;
    uint8 *MetaDataPtr;
    PduLengthType SduLength;
} PduInfoType; //数据信息  包括数据指针，meta数据指针，pdu长度

typedef struct {
    uint8 resetflag;
    uint8 resetloss;
    uint32 resetTag;
    uint32 resetTime;
} ResetState_Type;

// Csm_Types.h
typedef uint8 Crypto_OperationModeType;

// masterFVM.c
uint32 jobId = 2333;               // 自定义
Crypto_OperationModeType mode = 3; // 自定义

// -------------------------------

// compiler_Cfg.h
#define SLAVE_CODE
#define STD_TYPES_VAR
#define FRESH_VAR
#define SLAVE_APPL_DATA
#define SLAVE_APPL_CONST

#endif //SECOC_MASTERFVM_CFG_H
