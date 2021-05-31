#include <bitmap.h>
#include "../include/slaveFVM.h"
#include "slaveFVM_Cfg.c"
#include "../include/MacGenerate.h"


uint8 *verifyPtr;

uint16 ackvid = 0x322; // ecu返回的ack确认报文  可配置
uint8 verifystate = 0; // verifystate确认状态 0表示未确认 1表示确认
uint16 notifyid = 0x386; // ecu返回的通知报文  可配置
uint8 secocenabled = 0;     //secoc可工作  0表示不行 1表示可以
uint8 errorid = 0x64; // 出错

uint16 tripcanid = 0xffff; //可配置
uint8 trip[3]; // trip报文
uint8 TripCntLength = 11; //可配置
uint16 ackid = 0x2be; //返回的ack报文  可配置

/**
 * 根据id匹配不同报文
 * 1.ack确认报文表示 master收到自己的ack报文，修改标记在后续收到trip同步报文不返回ack报文
 * 2.通知报文送达则secoc功能可执行。。
 * 3.错误报文表面trip同步失败
*/
FUNC(void, SLAVE_CODE)
FVM_changeState(VAR(PduIdType, COMSTACK_TYPES_VAR) RxPduId) {
    if (RxPduId == ackvid) // ack确认报文
    {
        verifystate = 1; // 状态验证
    } else if (RxPduId == notifyid) // 通知报文
    {
        secocenabled = 1; // 不可工作
    } else if (RxPduId == errorid) // 错误
    {
        // pass
    }
}


/**
 * 更新trip报文
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateTrip(P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoPtr) {
    // 确认接收
    if (verifystate == 1) return E_OK;

    // 获取trip报文
    uint8 *receiveData = PduInfoPtr->SduDataPtr; //64bit
    uint64 receiveDataValue = 0;
    for (int i = 0; i < 8; ++i) {
        receiveDataValue |= (uint64) receiveData[i] << (8 * (7 - i));
    }

    // mac
    uint64 receiveMacValue = receiveDataValue << (TripCntLength + 1);
    receiveMacValue >>= (TripCntLength + 1);
    uint64 receiveTripResetValue = receiveDataValue - receiveMacValue;


    uint64 macGenerateDataValue = 0;
    macGenerateDataValue += (uint64) tripcanid << 48;
    macGenerateDataValue += receiveTripResetValue >> 16;

    uint8 macGenerateData[8];
    memset(macGenerateData, 0, sizeof(macGenerateData));
    for (int i = 0; i < 8; ++i) {
        macGenerateData[i] = (uint8) (macGenerateDataValue >> ((7 - i) * 8));
    }
    uint8 hash[8];
    memset(hash, 0, sizeof(hash));
    uint64 hashValue = 0;
    Mac_Generate(macGenerateData, 8, hash);

    for (int i = 0; i < 8; ++i) {
        hashValue += (uint64) hash[i] << (8 * (7 - i));
    }
    hashValue >>= (TripCntLength + 1);
    if (hashValue != receiveMacValue)
        return E_NOT_OK;

    return E_OK;
}


/**
 * 更新reset报文 
*/
FUNC(VAR(Std_ReturnType, STD_TYPES_VAR), SLAVE_CODE)
FVM_updateReset(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                P2CONST(PduInfoType, SLAVE_CODE, SLAVE_APPL_CONST)PduInfoType) {
    if (NUM_RESET <= TxPduId) return E_NOT_OK;

    // reset长度
    uint8 ResetCntLength = resetCnt[TxPduId].ResetCntLength;

    // 获取reset报文
    uint64 receiveDataValue = 0;
    for (int i = 0; i < 8; ++i) {
        receiveDataValue |= (uint64) receiveData[i] << (8 * (7 - i));
    }

    // mac
    uint64 receiveMacValue = receiveDataValue << ResetCntLength;
    receiveMacValue >>= ResetCntLength;
    uint64 receiveResetValue = receiveDataValue - receiveMacValue;

    uint64 macGenerateDataValue = 0;
    macGenerateDataValue |= (uint64) resetCnt[TxPduId].resetcanid << 48;\

    if(TripCntLength<=8) {
        macGenerateDataValue |= ((uint64)trip[0]<<(8-TripCntLength))>>16;
    } else if(8<TripCntLength&&TripCntLength<=16) {
        macGenerateDataValue |= ((uint64)trip[0]<<(16-TripCntLength))>>16;
        macGenerateDataValue |= (uint64)trip[1]>>(24-(16-TripCntLength));
    } else if(16<TripCntLength&&TripCntLength<=24) {
        macGenerateDataValue |= ((uint64)trip[0]<<(24-TripCntLength))>>16;
        macGenerateDataValue |= (uint64)trip[1]>>(24-(16-TripCntLength));
        macGenerateDataValue |= (uint64)trip[2]>>(32-(16-TripCntLength));
    }
    macGenerateDataValue |=  (receiveResetValue >> (16+TripCntLength));

    uint8 hash[8];
    Mac_Generate((uint8 *) macGenerateDataValue, 8, hash);
    if (*(uint64 *) hash != *(uint64 *) mac) {
        return E_NOT_OK;
    }

    //TODO: 编译器没警告，但我不确定行不行，不行就换memcopy
    *(resetCnt[TxPduId].resetdata) = receiveResetValue;

//    memset(trip, 0, sizeof(trip));
//    for (int i = 0; i < TripCntLength; i++) {
//        if (test(verifyPtr_bits, i))
//            set(trip_bits, i);
//    }
//    resetState[TxPduId].resetflag = false;
//    if (test(verifyPtr_bits, TripCntLength)) {
//        resetState[TxPduId].resetflag = true;
//    }
    //(*PduInfoPtr).SduDataPtr = NULL;
    //(*PduInfoPtr).SduLength = 8;

    // CanIf_Transmit(ackid, PduInfoPtr);

    return E_OK;
}



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
    preTrip[TxPduId] = (uint8) trip_bits.M;
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
FVM_GetTxFreshnessTruncData(
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

    uint8 length = TripCntLength + current_reset.ResetCntLength + *SecOCTruncatedFreshnessValueLength - 2 + 2;
    SecOCTruncatedFreshnessValueLength = (uint32 *) &length;
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