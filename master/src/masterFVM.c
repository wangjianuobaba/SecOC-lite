//
// Created by zhao chenyang on 2021/5/16.
//
#include "masterFVM.h"


uint8 TripCntLength = 16; //可配置
uint8 trip[3];              //初始化时从非易失性存储器中获得并+1后再存回非易失性存储器， 低位先占满8字节 高位再占
//例如  TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][]
// 返回一个uint8的实际位数

FUNC(void, MASTER_CODE)
MasterFVM_Init(void) {
    /*
    1.获取非易失性存储器中的值，存入全局变量trip[3]对应索引内，trip数组使用情况依据TripCntLength决定。优先使用低索引；
    2.对trip[3]+1, 达到char最大值则，发生进位;
        若发生进位导致trip到达TripCntLength规定最大情况，则 trip[3] =1,即高位为0，最低位为1；
        if(trip[-1]==255){   trip[-2]+=1;  依次观察是否进位
    3.将新的trip值更新到非易失性存储器中；

    */

    /**
     * 写入Trip
     */
    // HAL_I2C_Mem_Read(&hi2c1, ADDR_24LCxx_Read, 0, I2C_MEMADD_SIZE_8BIT, trip, 2, 0xff); //读取2字节的trip
    if (TripCntLength <= 8) {
        if (trip[0] == 255) {
            trip[0] = 1;
        } else {
            trip[0] += 1;
        }
    } else if (TripCntLength <= 16) {
        if (trip[1] == 255) { //低位满，需进位
            if (trip[0] == 255) { //trip值达到最大值
                trip[0] = 0;
                trip[1] = 1;
            } else { //trip未达最大值， 高位进位，低位到0
                trip[0] += 1;
                trip[1] = 0;
            }
        } else {
            trip[1] += 1;
        }
    } else {
        if (trip[2] == 255) {
            if (trip[1] == 255) {
                if (trip[0] == 255) {
                    trip[0] = trip[1] = 0;
                    trip[2] = 1;
                } else {
                    trip[1] = trip[2] = 0;
                    trip[0] += 1;
                }
            } else {
                trip[1] += 1;
                trip[2] = 0;
            }
        } else {
            trip[2] += 1;
        }
    }


    /**
     * 写回Trip
     */

    // for (i = 0; i < 2; i++)
    // {
    // 	HAL_I2C_Mem_Write(&hi2c1, ADDR_24LCxx_Write, i, I2C_MEMADD_SIZE_8BIT, &trip[i], 1, 0xff); //使用I2C块读，出错。因此采用此种方式，逐个单字节写入
    // 	HAL_Delay(5);																			  //此处延时必加，与AT24C02写时序有关
    // }
}

uint16 tripCanId = 0xffff;           //可配置
ResetCnt_Type currentReset;

void get_value(uint16 canId, PduInfoType *PduInfoPtr, uint8 TripCntLength, uint8 ResetCntLength) {

    // 根据CntLength获得byte length
    uint8 tripByteLength = (TripCntLength + 7) / 8;
    uint8 resetByteLength = (ResetCntLength + 7) / 8;
    // 声明两个存data的变量
    uint32 resetDataValue = 0;
    uint32 tripDataValue = 0;

    uint8 generateMacData[8];
    memset(generateMacData, 0, sizeof(generateMacData));
    uint64 generateMacDataValue = 0;
    // 向生成mac的原始数据加入canid
    generateMacDataValue += (uint64) canId << 48;

    // 将trip的值存入tripDataValue
    switch (tripByteLength) {
        case 1:
            tripDataValue = trip[0];
            break;
        case 2:
            tripDataValue = ((uint32) trip[0] << 8) + (uint32) trip[1];
            break;
        case 3:
            tripDataValue = ((uint32) trip[0] << 16) + ((uint32) trip[1] << 8) + (uint32) trip[2];
            break;
    }
    // data_trip现在是trip+padding0
    tripDataValue <<= (32 - TripCntLength);

    //向生成mac的原始数据加入trip
    generateMacDataValue += (uint64) tripDataValue << 16;

    // 向生成mac的原始数据中加入ResetCnt的值
    if (ResetCntLength == 0) {
        // 设置reset bit
        generateMacDataValue += (uint64) 1 << (48 - 1 - TripCntLength);
    } else {
        // 存入reset data
        uint8 *resetData = currentReset.resetdata;
        // 将reset的值存入resetDataValue
        switch (resetByteLength) {
            case 1:
                resetDataValue = resetData[0];
                break;
            case 2:
                resetDataValue = ((uint32) resetData[0] << 8) + (uint32) resetData[1];
                break;
            case 3:
                resetDataValue = ((uint32) resetData[0] << 16) + ((uint32) resetData[1] << 8) + (uint32) resetData[2];
                break;
        }
        resetDataValue <<= (32 - ResetCntLength);
        if (TripCntLength <= 16)
            generateMacDataValue += (uint64) resetDataValue << (16 - TripCntLength);
        else
            generateMacDataValue += (uint64) resetDataValue >> (TripCntLength - 16);
    }
    for (int i = 0; i < 8; ++i) {
        generateMacData[i] = (uint8) (generateMacDataValue >> (8 * (7 - i)));
    }
    // 生成mac值
    uint8 mac[8];
    memset(mac, 0, sizeof(mac));
    // 测试使用memcpy
//    memcpy(mac, generateMacData, sizeof(generateMacData));
    Mac_Generate(generateMacData, 8, mac);
    uint64 macValue = 0;
    for (int i = 0; i < 8; ++i) {
        macValue += (uint64) mac[i] << (8 * (7 - i));
    }

    // 数据重排
    if (ResetCntLength == 0) { // trip + 1 + mac
        tripDataValue += 1 << (32 - TripCntLength - 1);
        macValue >>= (TripCntLength + 1);
        macValue += ((uint64) tripDataValue << 32);
    } else { // reset + mac
        macValue >>= ResetCntLength;
        macValue += ((uint64) resetDataValue << 32);
    }
    for (int i = 0; i < 8; ++i) {
        PduInfoPtr->SduDataPtr[i] = (uint8) (macValue >> (8 * (7 - i)));
    }
}


FUNC(void, MASTER_CODE)
MasterFVM_getTripValue(P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA)PduInfoPtr) {
    get_value(tripCanId, PduInfoPtr, TripCntLength, 0);
}


FUNC(void, MASTER_CODE)
MasterFVM_getResetValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                        P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA)PduInfoPtr) {
    if (TxPduId < NUM_RESET) {
        currentReset = resetCnt[TxPduId];
        uint8 ResetCntLength = currentReset.ResetCntLength;
        // 配置resetData最多3个字节
        uint32 resetDataValue = 0;
        if (ResetCntLength <= 8) {
            resetDataValue = (uint32) currentReset.resetdata[0];
        } else if (ResetCntLength <= 16) {
            resetDataValue = ((uint32) currentReset.resetdata[0] << 8) + (uint32) currentReset.resetdata[1];
        } else {
            resetDataValue = ((uint32) currentReset.resetdata[0] << 16) + ((uint32) currentReset.resetdata[1] << 8) +
                             (uint32) currentReset.resetdata[2];
        }
        // resetData + 1
        // 看是否达到最大值
        if ((uint64) resetDataValue < ((uint64) 1 << currentReset.ResetCntLength) - 1)
            resetDataValue++;
        int byteLength = (ResetCntLength + 7) / 8;
        for (int i = 0; i < byteLength; ++i) {
            currentReset.resetdata[i] = (uint8) (resetDataValue >> (8 * (byteLength - i - 1)));
        }
        get_value(currentReset.resetcanid, PduInfoPtr, TripCntLength, ResetCntLength);
    }
}
