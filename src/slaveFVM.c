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
    bool reset_flag = false;
    if (test(trip_message, TripCntLengthgth))
        reset_flag = true;
    // trip报文的mac部分 长度为(64-TripLengthgth-1)bit
    uint8 macLength = 64 - TripCntLengthgth - 1;
    bitmap mac_bits = init(macLength);
    uint8 *mac = (uint8 *)mac_bits.M;
    for (int i = TripCntLengthgth + 1; i < 64; i++) {
        if (test(trip_message, i))
            set(mac_bits, i);
    }

    // 构造dataptr 长度为128bit
    bitmap dataptr_bits = init(sizeof(char) * 8);
    char *dataptr = dataptr_bits.M;
    // tripcanid
    for (int i = 0; i < 16; i++) {
        if (tripcanid & (1 << (16 - i - 1)))
            set(dataptr_bits, i);
    }
    // reset
    if (reset_flag)
        set(dataptr_bits, 16);
    // trip
    for (int i = 0; i < TripCntLengthgth; i++)
        if (test(trip_bits, i))
            set(dataptr_bits, i + 17);

    // Csm验证
    bitmap verifyPtr_bits = init(TripCntLengthgth + 1);
    uint8 *verifyPtr = (uint8 *)verifyPtr_bits.M;
    // if (Csm_MacVerify(jobId, mode, dataptr, 32, mac, macLength, verifyPtr) == E_NOT_OK)
    //     return E_NOT_OK;
    memset(trip, 0, sizeof(trip));
    for (int i = 0; i < TripCntLengthgth; i++) {
        if (test(verifyPtr_bits, i))
            set(trip_bits, i);
    }
    reset_flag = false;
    if (test(verifyPtr_bits, TripCntLengthgth)) {
        reset_flag = true;
    }
    //(*PduInfoPtr).SduDataPtr = NULL;
    //(*PduInfoPtr).SduLength = 8;

    // CanIf_Transmit(ackid, PduInfoPtr);

    return E_OK;
}

uint16 error_id = 0x100; //报错报文id   可配置

uint8 resetData[3 * NUM_RESET];
uint8 preresetData[3 * NUM_MSG];
uint16 resetcanid[] = {0x65, 0x66};

ResetState_Type resetState[] = {
    {.resetflag = 0,
     .resetloss = 0,
     .resetTag = 0,
     .resetTime = 1000},
    {.resetflag = 0,
     .resetloss = 0,
     .resetTag = 0,
     .resetTime = 2500}};

/**
 * 更新reset报文 
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST) PduInfoType) {
    if (NUM_RESET <= TxPduId) return E_NOT_OK;

    // reset长度
    uint8 ResetCntLengthgth = resetCnt[TxPduId].ResetCntLength;

    // 获取reset报文
    uint8 *data = PduInfoType->SduDataPtr; //64bit
    bitmap reset_message = init_from_uint8(data, 64);

    // 解析reset报文
    // reset报文的reset部分 长度为ResetCntLengthgth
    bitmap reset_bits = init(sizeof(uint8) * 3);
    for (int i = 0; i < ResetCntLengthgth; i++) {
        if (test(reset_message, i))
            set(reset_bits, i);
    }
    // reset报文的mac部分 长度为(64-ResetLengthgth)bit
    uint8 macLength = 64 - ResetCntLengthgth;
    bitmap mac_bits = init(macLength);
    uint8 *mac = (uint8 *)mac_bits.M;
    for (int i = ResetCntLengthgth; i < 64; i++) {
        if (test(reset_message, i))
            set(mac_bits, i);
    }

    // 构造dataptr 长度为128bit
    bitmap dataptr_bits = init(sizeof(char) * 8);
    char *dataptr = dataptr_bits.M;
    // resetcanid
    for (int i = 0; i < 16; i++) {
        if (resetcanid[TxPduId] & (1 << (16 - i - 1)))
            set(dataptr_bits, i);
    }
    // trip
    bitmap trip_bits = init_from_uint8(trip, sizeof(uint8) * 3);
    for (int i = 0; i < TripCntLengthgth; i++)
        if (test(trip_bits, i))
            set(dataptr_bits, i + 16);
    // reset
    for (int i = 0; i < ResetCntLengthgth; i++) {
        if (test(reset_bits, i))
            set(dataptr_bits, i + 16 + TripCntLengthgth);
    }

    // Csm验证
    bitmap verifyPtr_bits = init(TripCntLengthgth + 1);
    uint8 *verifyPtr = (uint8 *)verifyPtr_bits.M;
    // if (Csm_MacVerify(jobId, mode, dataptr, 32, mac, macLength, verifyPtr) == E_NOT_OK)
    //     return E_NOT_OK;
    memset(trip, 0, sizeof(trip));
    for (int i = 0; i < TripCntLengthgth; i++) {
        if (test(verifyPtr_bits, i))
            set(trip_bits, i);
    }
    resetState[TxPduId].resetflag = false;
    if (test(verifyPtr_bits, TripCntLengthgth)) {
        resetState[TxPduId].resetflag = true;
    }
    //(*PduInfoPtr).SduDataPtr = NULL;
    //(*PduInfoPtr).SduLength = 8;

    // CanIf_Transmit(ackid, PduInfoPtr);

    return E_OK;
}
