//
// Created by zhao chenyang on 2021/5/16.
//
#include "masterFVM.h"
#include <string.h>

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
    if (trip[1] == 255) { //低位满，需进位
        if (trip[0] == 255) { //trip值达到最大值
            trip[0] = 0;
            trip[1] = 1;
            TripCntLength = 1;
        } else { //trip未达最大值， 高位进位，低位到0
            trip[0] += 1;
            trip[1] = 0;
            TripCntLength = (uint8) 8 + length(trip[0]);
        }
    } else {
        trip[1] += 1;
        TripCntLength = (uint8) 8 + length(trip[0]);
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

uint16 trip_can_id = 0xffff;           //可配置
uint32 jobId = 2333;               // 自定义
ResetCnt_Type current_reset;

void get_value(uint16 can_id, PduInfoType *PduInfoPtr, uint8 tripCntLength, uint8 ResetCntLength) {
    uint8 can_data[8];

    // 声明两个存data的变量
    uint16 data_reset_num;
    uint16 data_trip;

    uint8 data_generate_mac[8];
    memset(data_generate_mac, 0, sizeof(data_generate_mac));
    uint64 generate_mac = 0;
    // 向生成mac的原始数据加入canid
    generate_mac += (uint64) can_id << 48;

    // 左移trip
    data_trip = trip[0];
    data_trip <<= 8;
    data_trip += trip[1];
    // data_trip现在是trip+padding0
    data_trip <<= (16 - tripCntLength);

    //向生成mac的原始数据加入trip
    generate_mac += (uint64) data_trip << 32;

    // 向生成mac的原始数据中加入ResetCnt的值
    if (ResetCntLength == 0) {
        // 设置reset bit
        generate_mac += (uint64) 1 << (48 - 1 - tripCntLength);
    } else {
        uint8 *data_reset = current_reset.resetdata;
        data_reset_num = data_reset[0];
        data_reset_num <<= 8;
        data_reset_num += data_reset[1];
        data_reset_num <<= (16 - ResetCntLength);
        generate_mac += (uint64) data_reset_num << (48 - 16 - tripCntLength);
    }
    for (int i = 0; i < 8; ++i) {
        data_generate_mac[i] = (uint8) (generate_mac >> (8 * (7 - i)));
    }
    // 生成mac值
    uint8 mac[8];
    uint8 mac_length[8];
    memset(mac, 0, sizeof(mac));
    memset(mac_length, 0, sizeof(mac_length));
    /**
     * 生成Mac值存入mac数组
     */
    memcpy(mac, data_generate_mac, sizeof(data_generate_mac));
//	Csm_MacGenerate(jobId, mode, data_generate_mac, 16 + tripCntLength + ResetCntLength, mac, mac_length);
    uint64 result = 0;
    for (int i = 0; i < 8; ++i) {
        result += (uint64) mac[i] << (8 * (7 - i));
    }

    // 数据重排
    if (ResetCntLength == 0) { // trip + 1 + mac
        data_trip += 1 << (16 - tripCntLength - 1);
        result >>= (tripCntLength + 1);
        result += ((uint64) data_trip << 48);
    } else { // reset + mac
        result >>= ResetCntLength;
        result += ((uint64) data_reset_num << 48);
    }
    for (int i = 0; i < 8; ++i) {
        can_data[i] = (uint8) (result >> (8 * (7 - i)));
    }
    PduInfoPtr->SduDataPtr = can_data;
}


FUNC(void, MASTER_CODE)
MasterFVM_getTripValue(P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA)PduInfoPtr) {
    /*
    默认使用数据长度为8的can通信，因此trip同步消息将由trip[],reset[],mac共同使用8字节，并将连接的结果存入pudInfoPtr中。
        trip(TripCntLength)表示 这个数组实际长度由bit长度决定，在构造dataptr时需要修改各比特站位，将trip[]数组前面空位移除
        例如 TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][] 在此场景下 要左移5位改为[1001 1111][1110 0000]
        由于后接1bit reset且值为1 则dataptr = [0x02] [0xbd] [1001 1111][1111 0000] 最后4位为补0
    1.构造生成mac的原始数据 char *dataptr 组成为
    tripcanid[2](tripcanid拆分成两个char)+trip(TripCntLength)+reset(1bit)：值为1 + padding0;
        在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
        例如  TripCntLength=12  [ 0x02 0xbd 0x00 0x14]  此时
    2.调用Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr);
        获得mac值
    3. 数据排版 PduInfoPtr.SduDataPtr =trip(TripCntLength)+1(1bit)+ mac(64bit-TripCntLength-1);
        重排位，在数据生成时不再填充0
    */
    get_value(trip_can_id, PduInfoPtr, TripCntLength, 0);
}

FUNC(void, MASTER_CODE)
MasterFVM_getResetValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId,
                        P2VAR(PduInfoType, AUTOMATIC, SECOC_APPL_DATA)PduInfoPtr) {
    /*
    1.先判断TxPduId 若>=NUM_RESET， 则直接退出;
    默认使用数据长度为8的can通信，因此trip同步消息将由trip[],reset[],mac共同使用8字节，并将连接的结果存入pudInfoPtr中。
        trip(TripCntLength)表示 这个数组实际长度由bit长度决定，在构造dataptr时需要修改各比特站位，将trip[]数组前面空位移除
        例如 TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][] 在此场景下 要左移5位改为[1001 1111][1110 0000]
        由于后接ResetCntLength reset   如  ResetCntLength=11  且reset=0x04 0xff 左移5位并连接到前面trip后
         则dataptr = [0x00] [0x65] [1001 1111][1111 0011] [1111 1100 ]最后2位为补0
    2.构造生成mac的原始数据 char *dataptr 组成为
    resetcanid[2](reset拆分成两个char, 由TxPduId作为索引找到)+trip(TripCntLength)+reset(ResetCntLength) + padding0;
        在trip和reset计数器连接的总bit数构不成1个char时，在后面补0
        reset(ResetCntLength)值在获取后需要将resetData[3*TxPduId]对应值加1 若发生进位需要改变
        若reset达到ResetCntLength规定的最大值，则reset不加值。

    3.调用Csm_MacGenerate(uint32 jobId, Crypto_OperationModeType mode,uint8* dataPtr, uint32 dataLength, uint8* macPtr,uint8 *macLengthPtr);
        获得mac值
    4. 数据排版 PduInfoPtr.SduDataPtr =reset(ResetCntLength)+ mac(64bit-ResetCntLength);
        重排位，在数据生成时不再填充0

    */
    if (TxPduId < NUM_RESET) {
        current_reset = resetCnt[TxPduId];
        // 配置resetData最多2个字节
        uint16 resetData = ((uint16)current_reset.resetdata[0] << 8) + (uint16)current_reset.resetdata[1];
        // resetData + 1
        if(resetData < (1<<current_reset.ResetCntLength)-1)
            resetData++;
        for(int i=0;i<2;++i){
            current_reset.resetdata[i] = (uint8)(resetData>>(8*(1-i)));
        }
        get_value(current_reset.resetcanid, PduInfoPtr, TripCntLength, current_reset.ResetCntLength);
    }
}
