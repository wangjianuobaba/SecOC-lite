#ifndef _SLAVEFVM_H
#define _SLAVEFVM_H

#include "slaveFVM_Cfg.h"
#include "Compiler.h"
#include "../../include/Compiler_Cfg.h"

// 构造新鲜值的标志
enum SYMBOL {
    F1_1 = 1, F1_2, F1_3,
    F2_1, F2_2, F2_3,
    F3_1, F3_2, F3_3,
    F4_1, F4_2, F4_3,
    F5_1, F5_2, F5_3,
};

uint16 ackvid; // ecu返回的ack确认报文  可配置
uint8 verifystate; // verifystate确认状态 0表示未确认 1表示确认
uint16 notifyid; // ecu返回的通知报文  可配置
uint8 secocenabled;     //secoc可工作  0表示不行 1表示可以
uint8 errorid; // 出错

uint16 tripcanid; //可配置
uint8 trip[3]; // trip报文
uint8 TripCntLength; //可配置
uint16 ackid; //返回的ack报文  可配置

FUNC(void, SLAVE_CODE)
FVM_changeState(VAR(PduIdType, COMSTACK_TYPES_VAR) RxPduId);


FUNC(void, SLAVE_CODE)
FVM_Syn_Check(void);


FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateTrip(P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr);


FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr);


FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetTxFreshness(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength
);


FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetTxFreshnessTruncData(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValueLength
);

FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetRxFreshness(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST)SecOCTruncatedFreshnessValue,
        VAR(uint32, FRESH_VAR) SecOCTruncatedFreshnessValueLength,
        VAR(uint16, FRESH_VAR) SecOCAuthVerifyAttempts,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength);


FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetRxFreshnessAuthData(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST)SecOCTruncatedFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValueLength,
        P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST)SecOCAuthDataFreshnessValue,
        VAR(uint16, FRESH_VAR) SecOCAuthDataFreshnessValueLength,
        VAR(uint16, FRESH_VAR) SecOCAuthVerifyAttempts,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength
);

FUNC(void, SLAVE_CODE)
FVM_updatePreValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                   P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue);

#endif

