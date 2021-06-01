//
// Created by zhao chenyang on 2021/5/16.
//
#include <stdio.h>
#include <assert.h>
#include "masterFVM.h"

void testInit() {
    TripCntLength = 8;
    printf("InitTest(TripCntLength = 8):\n\t");

    trip[0] = 255;
    MasterFVM_Init();
    assert(trip[0] == 1);
    printf("1/2\r\t");

    trip[0] = 133;
    MasterFVM_Init();
    assert(trip[0] == 134);
    printf("2/2\r\t");
    printf("\n\ttest success!!\n");


    TripCntLength = 16;
    printf("\nInitTest(TripCntLength = 16):\n\t");
    trip[0] = 255;
    trip[1] = 255;
    MasterFVM_Init();
    assert(trip[0] == 0);
    assert(trip[1] == 1);
    printf("1/3\r\t");

    trip[1] = 255;
    trip[0] = 133;
    MasterFVM_Init();
    assert(trip[0] == 134);
    assert(trip[1] == 0);
    printf("2/3\r\t");

    trip[1] = 2;
    trip[0] = 3;
    MasterFVM_Init();
    assert(trip[0] == 3);
    assert(trip[1] == 3);
    printf("3/3\r\t");
    printf("\n\tsuccess!!\n");


    TripCntLength = 24;
    printf("\nInitTest(TripCntLength = 24):\n\t");
    trip[0] = 255;
    trip[1] = 255;
    trip[2] = 255;
    MasterFVM_Init();
    assert(trip[0] == 0);
    assert(trip[1] == 0);
    assert(trip[2] == 1);
    printf("1/4\r\t");

    trip[0] = 133;
    trip[1] = 133;
    trip[2] = 255;
    MasterFVM_Init();
    assert(trip[0] == 133);
    assert(trip[1] == 134);
    assert(trip[2] == 0);
    printf("2/4\r\t");

    trip[0] = 133;
    trip[1] = 255;
    trip[2] = 255;
    MasterFVM_Init();
    assert(trip[0] == 134);
    assert(trip[1] == 0);
    assert(trip[2] == 0);
    printf("2/4\r\t");

    trip[0] = 255;
    trip[1] = 255;
    trip[2] = 133;
    MasterFVM_Init();
    assert(trip[0] == 255);
    assert(trip[1] == 255);
    assert(trip[2] == 134);
    printf("4/4\r\t");
    printf("\n\tsuccess!!\n\n");
}

uint8 data[8];

void testGetTripValue() {
    printf("GetTripValueTest:\n");
    PduInfoType pduInfoPtr;
    pduInfoPtr.SduDataPtr = data;

    printf("1:\n");
    TripCntLength = 5;
    trip[0] = 0x01;
    tripCanId = 0x2bd;
    MasterFVM_getTripValue(&pduInfoPtr);
    for (int i = 0; i < 8; ++i) {
        printf("0x%x,", pduInfoPtr.SduDataPtr[i]);
    }
    printf("\n");

    printf("2:\n");
    TripCntLength = 9;
    trip[0] = 0x01;
    trip[1] = 0xa5;
    tripCanId = 0x2bd;
    MasterFVM_getTripValue(&pduInfoPtr);
    for (int i = 0; i < 8; ++i) {
        printf("0x%x,", pduInfoPtr.SduDataPtr[i]);
    }
    printf("\n");

    printf("3:\n");
    TripCntLength = 16;
    trip[0] = 0xff;
    trip[1] = 0xdd;
    tripCanId = 0x2bd;
    MasterFVM_getTripValue(&pduInfoPtr);
    for (int i = 0; i < 8; ++i) {
        printf("0x%x,", pduInfoPtr.SduDataPtr[i]);
    }
    printf("\n");

    printf("4:\n");
    TripCntLength = 20;
    trip[0] = 0x00;
    trip[1] = 0x12;
    trip[2] = 0x34;
    tripCanId = 0x2bd;
    MasterFVM_getTripValue(&pduInfoPtr);
    for (int i = 0; i < 8; ++i) {
        printf("0x%x,", pduInfoPtr.SduDataPtr[i]);
    }
    printf("\n");

}

void testGetResetValue() {
    printf("GetResetValueTest:\n");

    PduInfoType pduInfoPtr;
    pduInfoPtr.SduDataPtr = data;

    printf("1:\n");
    TripCntLength = 5;
    resetCnt[1].ResetCntLength = 19;

    trip[0] = 0x01;

    resetCnt[1].resetdata[0] = 0x07;
    resetCnt[1].resetdata[1] = 0x00;
    resetCnt[1].resetdata[2] = 0x19;

    resetCnt[1].resetcanid = 0x0065;

    MasterFVM_getResetValue(1, &pduInfoPtr);
    for (int i = 0; i < 8; ++i) {
        printf("0x%x,", pduInfoPtr.SduDataPtr[i]);
    }

    printf("\n2:\n");
    TripCntLength = 9;
    resetCnt[1].ResetCntLength = 8;

    trip[0] = 0x01;
    trip[1] = 0xa5;

    resetCnt[1].resetdata[0] = 0xa1;

    resetCnt[1].resetcanid = 0x0065;

    MasterFVM_getResetValue(1, &pduInfoPtr);
    for (int i = 0; i < 8; ++i) {
        printf("0x%x,", pduInfoPtr.SduDataPtr[i]);
    }

    printf("\n3:\n");
    TripCntLength = 16;
    resetCnt[1].ResetCntLength = 10;

    trip[0] = 0xff;
    trip[1] = 0xdd;

    resetCnt[1].resetdata[0] = 0x00;
    resetCnt[1].resetdata[1] = 0xcb;

    resetCnt[1].resetcanid = 0x0066;

    MasterFVM_getResetValue(1, &pduInfoPtr);
    for (int i = 0; i < 8; ++i) {
        printf("0x%x,", pduInfoPtr.SduDataPtr[i]);
    }

    printf("\n4:\n");
    TripCntLength = 20;
    resetCnt[1].ResetCntLength = 7;

    trip[0] = 0x00;
    trip[1] = 0x12;
    trip[2] = 0x34;

    resetCnt[1].resetdata[0] = 0x01;

    resetCnt[1].resetcanid = 0x0066;

    MasterFVM_getResetValue(1, &pduInfoPtr);
    for (int i = 0; i < 8; ++i) {
        printf("0x%x,", pduInfoPtr.SduDataPtr[i]);
    }
    printf("\n");

}

int main() {
//    testInit();
//    testGetTripValue();
    testGetResetValue();
}

