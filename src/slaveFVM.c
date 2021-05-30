#include "slaveFVM.h"

uint8 verifystate = 0; // verifystate确认状态 0表示未确认 1表示确认

uint16 tripcanid = 0x2bd;    //可配置
uint8 trip[3];               // trip报文
uint8 TripCntLength = 16; //可配置
uint16 ackid = 0x2be;        //返回的ack报文  可配置

uint8 preTrip[3 * NUM_MSG];

uint8 msgData[6 * NUM_MSG];
uint8 preMsgData[6 * NUM_MSG];

uint8 resetData[3 * NUM_RESET];
uint8 preresetData[3 * NUM_MSG];

uint8 *verifyPtr;

ResetCntS_Type resetCnt[] = {
        {
                .resetdata = resetData,
                .preresetdata = preresetData,
                .ResetCntLength = 15,

        },
        {
                .resetdata = resetData,
                .preresetdata = preresetData,
                .ResetCntLength = 17,
                .resetcanid = 0x66}
};

/**
 * 更新trip报文
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateTrip(P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr) {
    // 确认接收
    if (verifystate == 1) return E_OK;

    // 获取trip报文
    uint8 *data = PduInfoPtr->SduDataPtr; //64bit
    bitmap trip_message = init_from_uint8(data, 64);

    // 解析trip报文
    // trip报文的trip部分 长度为TripCntLengthgth
    memset(trip, 0, sizeof(uint8) * 3);
    bitmap trip_bits = init_from_uint8(trip, sizeof(trip) * 8);
    for (int i = 0; i < TripCntLength; i++) {
        if (test(trip_message, i))
            set(trip_bits, i);
    }
    // trip报文的reset部分 长度为1bit
    bool reset_flag = false;
    if (test(trip_message, TripCntLength))
        reset_flag = true;
    // trip报文的mac部分 长度为(64-TripLengthgth-1)bit
    uint8 macLength = 64 - TripCntLength - 1;
    bitmap mac_bits = init(macLength);
    uint8 *mac = (uint8 *) mac_bits.M;
    for (int i = TripCntLength + 1; i < 64; i++) {
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
    for (int i = 0; i < TripCntLength; i++)
        if (test(trip_bits, i))
            set(dataptr_bits, i + 17);

    // Csm验证
    bitmap verifyPtr_bits = init(TripCntLength + 1);
    verifyPtr = (uint8 *) verifyPtr_bits.M;

    // if (Csm_MacVerify(jobId, mode, dataptr, 32, mac, macLength, verifyPtr) == E_NOT_OK)
    //     return E_NOT_OK;
    memset(trip, 0, sizeof(trip));
    for (int i = 0; i < TripCntLength; i++) {
        if (test(verifyPtr_bits, i))
            set(trip_bits, i);
    }
    reset_flag = false;
    if (test(verifyPtr_bits, TripCntLength)) {
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
        {
                .resetflag = 0,
                .resetloss = 0,
                .resetTag = 0,
                .resetTime = 1000
        },
        {
                .resetflag = 0,
                .resetloss = 0,
                .resetTag = 0,
                .resetTime = 2500
        }
};

/**
 * 更新reset报文 
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoType) {
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
    uint8 *mac = (uint8 *) mac_bits.M;
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
    for (int i = 0; i < TripCntLength; i++)
        if (test(trip_bits, i))
            set(dataptr_bits, i + 16);
    // reset
    for (int i = 0; i < ResetCntLengthgth; i++) {
        if (test(reset_bits, i))
            set(dataptr_bits, i + 16 + TripCntLength);
    }

    // Csm验证
    bitmap verifyPtr_bits = init(TripCntLength + 1);
    verifyPtr = (uint8 *) verifyPtr_bits.M;
    // if (Csm_MacVerify(jobId, mode, dataptr, 32, mac, macLength, verifyPtr) == E_NOT_OK)
    //     return E_NOT_OK;
    memset(trip, 0, sizeof(trip));
    for (int i = 0; i < TripCntLength; i++) {
        if (test(verifyPtr_bits, i))
            set(trip_bits, i);
    }
    resetState[TxPduId].resetflag = false;
    if (test(verifyPtr_bits, TripCntLength)) {
        resetState[TxPduId].resetflag = true;
    }
    //(*PduInfoPtr).SduDataPtr = NULL;
    //(*PduInfoPtr).SduLength = 8;

    // CanIf_Transmit(ackid, PduInfoPtr);

    return E_OK;
}

MsgCntS_Type msgCnt[] = {
        {
                .msgdata = msgData,
                .premsgdata = preMsgData,
                .MsgCntLength = 16},
        {
                .msgdata = msgData,
                .premsgdata = preMsgData,
                .MsgCntLength = 26
        }
};

/**
 * 更新pretrip prereset premsg
*/
FUNC(void, SLAVE_CODE)
FVM_updatePreValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                   P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue) {
    uint8 SecOCFreshnessValueLength = TripCntLength + resetCnt[TxPduId].ResetCntLength + msgCnt[TxPduId].MsgCntLength;
    bitmap freshness_bits = init_from_uint8((char *) SecOCFreshnessValue, SecOCFreshnessValueLength);
    // 根据新鲜值获取trip reset msg
    bitmap trip_bits = init(TripCntLength);
    for (int i = 0; i < TripCntLength; i++) {
        if (test(freshness_bits, i + 64 - TripCntLength))
            set(trip_bits, i);
    }
    bitmap reset_bits = init(resetCnt[TxPduId].ResetCntLength);
    for (int i = 0; i < resetCnt[TxPduId].ResetCntLength; i++) {
        if (test(freshness_bits, i + 64 - TripCntLength - resetCnt[TxPduId].ResetCntLength))
            set(reset_bits, i);
    }
    bitmap msg_bits = init(msgCnt[TxPduId].MsgCntLength);
    for (int i = 0; i < msgCnt[TxPduId].MsgCntLength; i++) {
        if (test(freshness_bits,
                 i + 64 - TripCntLength - resetCnt[TxPduId].ResetCntLength - msgCnt[TxPduId].MsgCntLength))
            set(msg_bits, i);
    }

    // 根据新鲜值更新trip reset msg
    preTrip[TxPduId] = trip_bits.M;
    resetCnt[TxPduId].preresetdata = reset_bits.M;
    msgCnt[TxPduId].premsgdata = msg_bits.M;
}

/**
 * 裁剪新鲜值
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetTxFreshness(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength) {
    // reset & prereset
    ResetCntS_Type current_reset = resetCnt[SecOCFreshnessValueID];
    bitmap reset_bits = init_from_uint8(current_reset.resetdata, current_reset.ResetCntLength);
    bitmap prereset_bits = init_from_uint8(current_reset.preresetdata, current_reset.ResetCntLength);

    // trip & preTrip
    bitmap trip_bits = init_from_uint8(trip[SecOCFreshnessValueID], TripCntLength); // TripCntLengthgth bit
    bitmap pretrip_bits = init_from_uint8(preTrip[SecOCFreshnessValueID], TripCntLength);

    // msg & premsg
    MsgCntS_Type current_msg = msgCnt[SecOCFreshnessValueID];
    bitmap msg_bits = init_from_uint8(current_msg.msgdata, current_msg.MsgCntLength);
    bitmap premsg_bits = init_from_uint8(current_msg.premsgdata, current_msg.MsgCntLength);

    // resetflag
    ResetState_Type current_resetState = resetState[SecOCFreshnessValueID];
    bitmap resetflag_bits = init_from_uint8(current_resetState.resetflag, 2);

    // 拼接trip和resetdata值并判断
    // 比较trip和pretrip
    bool equal_flag = true;
    for (int i = 0; i < TripCntLength; i++) {
        if (test(trip_bits, i) != test(pretrip_bits, i)) {
            equal_flag = false;
            break;
        }
    }

    // 比较reset和prereset
    for (int i = 0; i < current_reset.ResetCntLength; i++) {
        if (test(reset_bits, i) != test(pretrip_bits, i)) {
            equal_flag = false;
            break;
        }
    }

    // 相同 则把messagecounter+1 resetflag使用当前flag
    bitmap can_data_bits = init(sizeof(uint8) * 8); // 64bit
    uint8 *can_data = (uint8 *) can_data_bits.M;
    if (equal_flag) {
        // pretrip: (64-TripCntLength) —— (64-1) bit
        for (int i = 0; i < TripCntLength; i++) {
            if (test(pretrip_bits, i))
                set(can_data_bits, i + 64 - TripCntLength);
        }

        // prereset: (64-TripCntLength-ResetCntLengthgth) —— (64-TripCntLength-1)
        for (int i = 0; i < current_reset.ResetCntLength; i++) {
            if (test(prereset_bits, i)) {
                set(can_data_bits, i + 64 - TripCntLength - current_reset.ResetCntLength);
            }
        }

        // premsg: (64-TripCntLength-ResetCntLength-MsgCntLength) - (64-TripCntLength-ResetCntLengthgth-1)
        for (int i = 0; i < current_msg.MsgCntLength; i++) {
            if (test(premsg_bits, i)) {
                set(can_data_bits, i + 64 - TripCntLength - current_reset.ResetCntLength - current_msg.MsgCntLength);
            }
        }

        // reset_flag
        for (int i = 0; i < 2; i++) {
            if (test(resetflag_bits, i)) {
                set(can_data_bits,
                    i + 64 - TripCntLength - current_reset.ResetCntLength - current_msg.MsgCntLength - 2);
            }
        }
    } else {
        // trip: (64-TripCntLength) —— (64-1) bit
        for (int i = 0; i < TripCntLength; i++) {
            if (test(trip_bits, i))
                set(can_data_bits, i + 64 - TripCntLength);
        }

        // reset: (64-TripCntLength-ResetCntLengthgth) —— (64-TripCntLength-1)
        for (int i = 0; i < current_reset.ResetCntLength; i++) {
            if (test(reset_bits, i)) {
                set(can_data_bits, i + 64 - TripCntLength - current_reset.ResetCntLength);
            }
        }

        // msg: (64-TripCntLength-ResetCntLength-MsgCntLength) - (64-TripCntLength-ResetCntLengthgth-1)
        for (int i = 0; i < current_msg.MsgCntLength; i++) {
            if (test(msg_bits, i)) {
                set(can_data_bits, i + 64 - TripCntLength - current_reset.ResetCntLength - current_msg.MsgCntLength);
            }
        }

        // reset_flag
        for (int i = 0; i < 2; i++) {
            if (test(resetflag_bits, i)) {
                set(can_data_bits,
                    i + 64 - TripCntLength - current_reset.ResetCntLength - current_msg.MsgCntLength - 2);
            }
        }
    }

    SecOCFreshnessValue = can_data;
    return E_OK;
}

/**
 * 根据SecOCTruncatedFreshnessValueLength长度截取
 * 截取msg中后(SecOCTruncatedFreshnessValueLength-2)bit + 2bit(resetflag reset后两位)
 * 构造SecOCTruncatedFreshnessValue
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FvM_GetTxFreshnessTruncData(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCTruncatedFreshnessValueLength) {
    // 获取新鲜值
    bitmap freshness_bits = init_from_uint8((char *) SecOCFreshnessValue, SecOCFreshnessValueLength);
    ResetCntS_Type current_reset = resetCnt[SecOCFreshnessValueID];
    ResetState_Type current_resetState = resetState[SecOCFreshnessValueID];
    MsgCntS_Type current_msg = msgCnt[SecOCFreshnessValueID];

    uint8 length = TripCntLength + current_reset.ResetCntLength + SecOCTruncatedFreshnessValueLength - 2 + 2;
    SecOCTruncatedFreshnessValueLength = &length;
    bitmap truncatedFreshness_bits = init_from_uint8(SecOCTruncatedFreshnessValue, length);

    // 获取trip
    for (int i = 0; i < TripCntLength; i++) {
        if (test(freshness_bits, i + 64 - TripCntLength)) {
            set(truncatedFreshness_bits, i + 64 - TripCntLength);
        }
    }

    // 获取reset
    for (int i = 0; i < current_reset.ResetCntLength; i++) {
        if (test(freshness_bits, i + 64 - TripCntLength - current_reset.ResetCntLength)) {
            set(truncatedFreshness_bits, i + 64 - TripCntLength - current_reset.ResetCntLength);
        }
    }

    // 裁剪msg
    for (int i = 0; i < SecOCTruncatedFreshnessValueLength - 2; i++) {
        if (test(freshness_bits, i + 64 - TripCntLength - current_reset.ResetCntLength - current_msg.MsgCntLength)) {
            set(truncatedFreshness_bits,
                i + 64 - TripCntLength - current_reset.ResetCntLength - current_msg.MsgCntLength + 2);
        }
    }

    // 获取reset_flag
    bitmap resetflag_bits = init(2);
    for (int i = 0; i < 2; i++) {
        if (test(freshness_bits,
                 i + 64 - TripCntLength - current_reset.ResetCntLength - current_msg.MsgCntLength + 2 - 2)) {
            set(truncatedFreshness_bits, i);
        }
    }

    return E_OK;
}


// 构造新鲜值的标志
enum SYMBOL {
    F1_1 = 1, F1_2, F1_3,
    F2_1, F2_2, F2_3,
    F3_1, F3_2, F3_3,
    F4_1, F4_2, F4_3,
    F5_1, F5_2, F5_3,
};

/**
 * 构造新鲜值
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_GetRxFreshness(
        VAR(uint16, FRESH_VAR) SecOCFreshnessValueID,
        P2CONST(uint8, SLAVE_CODE, SLAVE_APPL_CONST)SecOCTruncatedFreshnessValue,
        VAR(uint32, FRESH_VAR) SecOCTruncatedFreshnessValueLength,
        VAR(uint16, FRESH_VAR) SecOCAuthVerifyAttempts,
        P2VAR(uint8, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValue,
        P2VAR(uint32, SLAVE_CODE, SLAVE_APPL_DATA)SecOCFreshnessValueLength) {
    // 获得新鲜值
    bitmap truncatedFreshness_bits = init_from_uint8(SecOCTruncatedFreshnessValue, SecOCTruncatedFreshnessValueLength);

    // 获取reset_flag $ pre_reset_flag
    ResetState_Type current_resetState = resetState[SecOCFreshnessValueID];
    uint8 latest_resetflag = current_resetState.resetflag;
    uint8 received_resetflag = 0;
    for (int i = 0; i < 2; i++) {
        if (test(truncatedFreshness_bits, i))
            received_resetflag += (1 << i);
    }

    // 获取trip & pretrip
    bitmap trip_bits = init_from_uint8(&trip[SecOCFreshnessValueID], TripCntLength); // TripCntLengthgth bit
    bitmap pretrip_bits = init_from_uint8(&preTrip[SecOCFreshnessValueID], TripCntLength);
    // 获取reset & prereset
    ResetCntS_Type current_reset = resetCnt[SecOCFreshnessValueID];
    bitmap reset_bits = init_from_uint8(current_reset.resetdata, current_reset.ResetCntLength);
    bitmap prereset_bits = init_from_uint8(current_reset.preresetdata, current_reset.ResetCntLength);
    // 获取 msg&premsg
    MsgCntS_Type current_msg = msgCnt[SecOCFreshnessValueID];
    bitmap msg_bits = init_from_uint8(current_msg.msgdata, current_msg.MsgCntLength);
    bitmap premsg_bits = init_from_uint8(current_msg.premsgdata, current_msg.MsgCntLength);

    int tr_length = TripCntLength + current_reset.ResetCntLength;
    // trip | reset
    bitmap tripreset_bits = init(tr_length);
    for (int i = 0; i < TripCntLength; i++) {
        if (test(trip_bits, i))
            set(tripreset_bits, i);
    }
    for (int i = 0; i < current_reset.ResetCntLength; i++) {
        if (test(reset_bits, i))
            set(tripreset_bits, i + tr_length - TripCntLength);
    }
    // pretrip | prereset
    bitmap pretripreset_bits = init(tr_length);
    for (int i = 0; i < TripCntLength; i++) {
        if (test(pretrip_bits, i))
            set(pretripreset_bits, i);
    }
    for (int i = 0; i < current_reset.ResetCntLength; i++) {
        if (test(prereset_bits, i))
            set(pretripreset_bits, i + tr_length - TripCntLength);
    }

    // latest_reset value
    uint64 latest_value = bit2uint64(reset_bits, 32, current_reset.ResetCntLength);
    // tripreset value
    uint64 latest_tripreset = bit2uint64(tripreset_bits, 0, tr_length);
    uint64 received_tripreset = bit2uint64(pretripreset_bits, 0, tr_length);
    // lowermsg&uppermsg value
    uint64 uppermsg = bit2uint64(premsg_bits, 16, 16);
    uint64 latest_lowermsg = bit2uint64(premsg_bits, 0, 16);
    uint64 received_lowermsg = bit2uint64(msg_bits, 0, 16);

    // 比较 & 构造新鲜值
    // 计算新鲜值
    uint32 length = *SecOCFreshnessValueLength;
    bitmap freshness_bits = init_from_uint8(SecOCFreshnessValue, length);
    enum SYMBOL flags = F1_1; // 构造标志
    if (received_resetflag == latest_resetflag) {

        if (received_tripreset == latest_tripreset) {

            if (received_lowermsg > latest_lowermsg) {
                flags = F1_1;
            } else {
                flags = F1_2;
            }

        } else if (received_tripreset < latest_tripreset) {
            flags = F1_3;
            copy(freshness_bits, 0, pretripreset_bits, TripCntLength + current_reset.ResetCntLength);
        }

    } else if (received_resetflag == latest_resetflag - 1) {

        if (received_tripreset == latest_tripreset - 1) {

            if (received_lowermsg > latest_lowermsg) {
                flags = F2_1;
            } else {
                flags = F2_2;
            }

        } else if (received_tripreset < latest_tripreset - 1) {
            flags = F2_3;
            latest_value -= 1;
            freshness_bits.M[3] = (uint8) latest_value;
            freshness_bits.M[2] = (uint8) (latest_value >> 8);
        }

    } else if (received_resetflag == latest_resetflag + 1) {

        if (received_tripreset == latest_tripreset + 1) {
            if (received_lowermsg > latest_lowermsg) {
                flags = F3_1;
            } else {
                flags = F3_2;
            }
        } else if (received_tripreset < latest_tripreset + 1) {
            flags = F3_3;
            latest_value += 1;
            freshness_bits.M[3] = (uint8) latest_value;
            freshness_bits.M[2] = (uint8) (latest_value >> 8);
        }

    } else if (received_resetflag == latest_resetflag - 2) {

        if (received_tripreset == latest_tripreset - 2) {
            if (received_lowermsg > latest_lowermsg) {
                flags = F4_1;
            } else {
                flags = F4_2;
            }
        } else if (received_tripreset < latest_tripreset - 2) {
            flags = F4_3;
            latest_value -= 2;
            freshness_bits.M[3] = (uint8) latest_value;
            freshness_bits.M[2] = (uint8) (latest_value >> 8);
        }

    } else if (received_resetflag == latest_resetflag + 2) {

        if (received_tripreset == latest_tripreset + 2) {

            if (received_lowermsg > latest_lowermsg) {
                flags = F5_1;
            } else {
                flags = F5_2;
            }
        } else if (received_tripreset < latest_tripreset + 2) {
            flags = F5_3;
            latest_value += 2;
            freshness_bits.M[3] = (uint8) latest_value;
            freshness_bits.M[2] = (uint8) (latest_value >> 8);
        }

    }

    switch (flags) {
        case F1_1:
        case F2_1:
        case F3_1:
        case F4_1:
        case F5_1: {
            // trip
            copy(freshness_bits, 0, pretrip_bits, TripCntLength);
            // reset
            copy(freshness_bits, TripCntLength, prereset_bits, current_reset.ResetCntLength);
            // upper_msg
            copy(freshness_bits, TripCntLength + current_reset.ResetCntLength, premsg_bits, 16);
        }
            break;

        case F1_2:
        case F2_2:
        case F3_2:
        case F4_2:
        case F5_2: {
            // trip
            copy(freshness_bits, 0, pretrip_bits, TripCntLength);
            // reset
            copy(freshness_bits, TripCntLength, prereset_bits, current_reset.ResetCntLength);
            // upper_msg
            uppermsg++;
            freshness_bits.M[5] = (uint8) uppermsg;
            freshness_bits.M[4] = (uint8) (uppermsg >> 8);
        }
            break;

        default: {
            // trip
            copy(freshness_bits, 0, pretrip_bits, TripCntLength);
            // upper_msg
            freshness_bits.M[4] &= 0;
            freshness_bits.M[5] &= 0;
        }
            break;
    }

    // lower_msg
    copy(freshness_bits, TripCntLength + current_reset.ResetCntLength + 16, msg_bits,
         16);
    return E_OK;
}