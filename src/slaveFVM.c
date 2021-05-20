#include "slaveFVM.h"

uint8 verifystate = 0; // verifystate确认状态 0表示未确认 1表示确认

uint16 tripcanid = 0x2bd;    //可配置
extern uint8 trip[3];               // trip报文
uint8 TripCntLengthgth = 16; //可配置
uint16 ackid = 0x2be;        //返回的ack报文  可配置

/**
 * 更新trip报文
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateTrip(P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST) PduInfoPtr) {
    // 确认接收
    if (verifystate == 1) return E_OK;

    // 获取trip报文
    uint8 *data = PduInfoPtr->SduDataPtr; //64bit
    bitmap trip_message = init_from_uint8(data, 64);

    // 解析trip报文
    // trip报文的trip部分 长度为TripCntLengthgth
    memset(trip, 0, sizeof(uint8) * 3);
    bitmap trip_bits = init_from_uint8(trip, sizeof(trip) * 8);
    for (int i = 0; i < TripCntLengthgth; i++) {
        if (test(trip_message, i))
            set(trip_bits, i);
    }
    // trip报文的reset部分 长度为1bit

    // trip报文的mac部分 长度为(64-TripLengthgth-1)bit

    // 构造dataptr 长度为128bit
    char dataptr[8];
}

uint16 error_id = 0x100; //报错报文id   可配置

/**
 * 更新reset报文 
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST) PduInfoType) {}
