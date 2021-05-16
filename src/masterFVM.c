//
// Created by zhao chenyang on 2021/5/16.
//
#include "masterFVM.h"
#include "bitmap.h"

uint8 TripCntLength = 16; //可配置
uint8 trip[3];			  //初始化时从非易失性存储器中获得并+1后再存回非易失性存储器， 低位先占满8字节 高位再占
//例如  TripCntLength=11  且trip=0x04 0xff       [0000 0100][1111 1111][]
// 返回一个uint8的实际位数

FUNC(void, MASTER_CODE)
MasterFVM_Init(void)
{
    /*
    1.获取非易失性存储器中的值，存入全局变量trip[3]对应索引内，trip数组使用情况依据TripCntLength决定。优先使用低索引；
    2.对trip[3]+1, 达到char最大值则，发生进位;
        若发生进位导致trip到达TripCntLength规定最大情况，则 trip[3] =1,即高位为0，最低位为1；
        if(trip[-1]==255){   trip[-2]+=1;  依次观察是否进位
    3.将新的trip值更新到非易失性存储器中；

    */

    // HAL_I2C_Mem_Read(&hi2c1, ADDR_24LCxx_Read, 0, I2C_MEMADD_SIZE_8BIT, trip, 2, 0xff); //读取2字节的trip

    if (trip[1] == 255)
    { //低位满，需进位
        if (trip[0] == 255)
        { //trip值达到最大值
            trip[0] = 0;
            trip[1] = 1;
            TripCntLength = 1;
        }
        else
        { //trip未达最大值， 高位进位，低位到0
            trip[0] += 1;
            trip[1] = 0;
            TripCntLength = (uint8)8 + length(trip[0]);
        }
    }
    else
    {
        trip[1] += 1;
        TripCntLength = (uint8)8 + length(trip[0]);
    }

    // for (i = 0; i < 2; i++)
    // {
    // 	HAL_I2C_Mem_Write(&hi2c1, ADDR_24LCxx_Write, i, I2C_MEMADD_SIZE_8BIT, &trip[i], 1, 0xff); //使用I2C块读，出错。因此采用此种方式，逐个单字节写入
    // 	HAL_Delay(5);																			  //此处延时必加，与AT24C02写时序有关
    // }
}

FUNC(void, MASTER_CODE)
MasterFVM_getTripValue(P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr)
{
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
MasterFVM_getResetValue(VAR(PduIdType, COMSTACK_TYPES_VAR) TxPduId, P2CONST(PduInfoType, AUTOMATIC, SECOC_APPL_DATA) PduInfoPtr)
{
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
    if (TxPduId < NUM_RESET)
    {
        current_reset = resetCnt[TxPduId];
        get_value(current_reset.resetcanid, PduInfoPtr, TripCntLength, current_reset.ResetCntLength);
    }
}
