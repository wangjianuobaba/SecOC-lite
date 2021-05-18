#include "slaveFVM.h"

uint16 tripcanid = 0x2bd;    //可配置
uint8 trip[3];               // trip报文
uint8 TripCntLengthgth = 16; //可配置
uint16 ackid = 0x2be;        //返回的ack报文  可配置

/**
 * 更新trip报文
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateTrip(P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST) PduInfoPtr) {}

uint16 error_id = 0x100; //报错报文id   可配置

/**
 * 更新reset报文 
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST) PduInfoType) {}
