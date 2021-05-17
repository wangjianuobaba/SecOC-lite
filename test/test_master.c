//
// Created by zhao chenyang on 2021/5/16.
//
#include <stdio.h>
#include <assert.h>
#include "masterFVM.h"

void testInit() {
    trip[0] = 255;
    trip[1] = 255;
    MasterFVM_Init();
    assert(trip[0] == 0);
    assert(trip[1] == 1);
    assert(TripCntLength == 1);

    trip[1] = 255;
    trip[0] = 133;
    MasterFVM_Init();
    assert(trip[0] == 134);
    assert(trip[1] == 0);
    assert(TripCntLength == 16);

    trip[1] = 2;
    trip[0] = 3;
    MasterFVM_Init();
    assert(trip[0] == 3);
    assert(trip[1] == 3);
    assert(TripCntLength == 10);
    printf("Init test success!!\n");
}

void testGetTripValue() {
    PduInfoType pduInfoPtr;
    TripCntLength = 11;
    trip[0] = 0x04;
    trip[1] = 0xff;
    MasterFVM_getTripValue(&pduInfoPtr);
    // mac的构造采用直接复制的方式
    // mac = dataptr = 0xffff(trip_can_id) + 1001 1111 111(trip) + 1(reset) +  0000
    // SduData: 1001 1111 1111 + mac
    assert(pduInfoPtr.SduDataPtr[0] == 0x9f);
    assert(pduInfoPtr.SduDataPtr[1] == 0xff);
    assert(pduInfoPtr.SduDataPtr[2] == 0xff);
    // mac
    assert(pduInfoPtr.SduDataPtr[3] == 0xf9);
    assert(pduInfoPtr.SduDataPtr[4] == 0xff);
    assert(pduInfoPtr.SduDataPtr[5] == 0x00);
    assert(pduInfoPtr.SduDataPtr[6] == 0x00);
    assert(pduInfoPtr.SduDataPtr[7] == 0x00);

    printf("getTripValue test success!!\n");
}

void testGetResetValue() {
    PduInfoType pduInfoPtr;
    TripCntLength = 11;
    trip[0] = 0x04;
    trip[1] = 0xff;
    resetCnt[1].resetcanid = 0xffff;
    resetCnt[1].ResetCntLength = 11;
    uint8 resetData[2];
    resetCnt[1].resetdata = resetData;
    resetCnt[1].resetdata[0] = 0x04;
    resetCnt[1].resetdata[1] = 0xff;

    MasterFVM_getResetValue(1, &pduInfoPtr);
    // mac的构造采用直接复制的方式
    // mac = dataptr = 0xffff(reset_can_id) + 10011111 111(trip)10011 111111(reset)00
    // SduData: 10011111 111 + mac
    assert(pduInfoPtr.SduDataPtr[0] == 0x9f);
    assert(pduInfoPtr.SduDataPtr[1] == 0xff);
    assert(pduInfoPtr.SduDataPtr[2] == 0xff);
    assert(pduInfoPtr.SduDataPtr[3] == 0xf3);
    assert(pduInfoPtr.SduDataPtr[4] == 0xfe);
    assert(pduInfoPtr.SduDataPtr[5] == 0x7f);
    assert(pduInfoPtr.SduDataPtr[6] == 0x80);
    assert(pduInfoPtr.SduDataPtr[7] == 0x00);

    printf("getResetValue test success!!\n");
}

int main() {
    testInit();
    testGetTripValue();
    testGetResetValue();
}

