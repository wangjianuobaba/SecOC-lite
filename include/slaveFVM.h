#ifndef _SLAVEFVM_H
#define _SLAVEFVM_H
#include "Compiler.h"
#include "platform_Types.h"
#include "bitmap.h"

//slaveFVM_Cfg.h
#define NUM_RESET 2
#define NUM_ECU 2
#define NUM_MSG 2

#define SYN_ERROR_SIZE 3

//slaveFVM_Types.h
typedef struct
{
    uint8 *resetdata;
    uint8 *preresetdata;
    uint8 ResetCntLengthgth;
    uint16 resetcanid;
} ResetCntS_Type;

//slaveFVM_Cfg.h

//Std_Types.h
typedef enum
{
    E_OK,
    E_NOT_OK,
} Std_ReturnType;

#define Std_ReturnType uint8

// ComStack_Types.h
typedef uint8 PduIdType;

// **********************

// Std_ReturnType FVM_updateTrip(const PduInfoType* PduInfoPtr );  //更新trip值
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateTrip(P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST) PduInfoPtr);

// Std_ReturnType FVM_updateReset(PduIdType TxPduId, const PduInfoType* PduInfoPtr);  //更新reset值
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST) PduInfoPtr);

// Std_ReturnType FVM_GetTxFreshness (
// 	uint16 SecOCFreshnessValueID,
// 	uint8* SecOCFreshnessValue,
// 	uint32* SecOCFreshnessValueLength
// );
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetTxFreshness(
    VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
    P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA) SecOCFreshnessValue,
    P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA) SecOCFreshnessValueLength);

// Std_ReturnType FvM_GetTxFreshnessTruncData (
// 	uint16 SecOCFreshnessValueID,
// 	uint8* SecOCFreshnessValue,
// 	uint32* SecOCFreshnessValueLength,
// 	uint8* SecOCTruncatedFreshnessValue,
// 	uint32* SecOCTruncatedFreshnessValueLength
// );
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FvM_GetTxFreshnessTruncData(
    VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
    P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA) SecOCFreshnessValue,
    P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA) SecOCFreshnessValueLength,
    P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA) SecOCTruncatedFreshnessValue,
    P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA) SecOCTruncatedFreshnessValueLength);

// Std_ReturnType FVM_GetRxFreshness (uint16 SecOCFreshnessValueID,
//     const uint8* SecOCTruncatedFreshnessValue,
//     uint32 SecOCTruncatedFreshnessValueLength,
//     uint16 SecOCAuthVerifyAttempts,
//     uint8* SecOCFreshnessValue,
//     uint32* SecOCFreshnessValueLength
// );
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetRxFreshness(
    P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST) SecOCTruncatedFreshnessValue,
    VAR(uint32, FRESH_VAR) SecOCTruncatedFreshnessValueLength,
    VAR(uint16, FRESH_VAR) SecOCAuthVerifyAttempts,
    P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA) SecOCFreshnessValue,
    P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA) SecOCFreshnessValueLength);

// Std_ReturnType FVM_GetRxFreshnessAuthData (
// 	uint16 SecOCFreshnessValueID,
// 	const uint8* SecOCTruncatedFreshnessValue,
// 	uint32 SecOCTruncatedFreshnessValueLength,
// 	const uint8* SecOCAuthDataFreshnessValue,
// 	uint16 SecOCAuthDataFreshnessValueLength,
// 	uint16 SecOCAuthVerifyAttempts,
// 	uint8* SecOCFreshnessValue,
// 	uint32* SecOCFreshnessValueLength
// );
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetRxFreshnessAuthData(
    VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
    P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST) SecOCTruncatedFreshnessValue,
    P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA) SecOCTruncatedFreshnessValueLength,
    P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST) SecOCAuthDataFreshnessValue,
    VAR(uint16, FRESH_VAR) SecOCAuthDataFreshnessValueLength,
    VAR(uint16, FRESH_VAR) SecOCAuthVerifyAttempts,
    P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA) SecOCFreshnessValue,
    P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA) SecOCFreshnessValueLength);

// void FVM_updatePreValue(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
FUNC(void, SLAVE_CODE)
FVM_updatePreValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST) PduInfoPtr);

// void updatePreRxValue(PduIdType TxPduId, const PduInfoType* PduInfoPtr);

#endif
