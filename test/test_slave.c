#include <assert.h>
#include "slaveFVM.h"
#include "slaveFVM_Cfg.h"
#include "bitmap.h"

void trip_test() {
    bitmap b = init(16);
    //printf(bit2string(b, 16));
    //printf("\n");
    set(b, 0);
    set(b, 8);
    //printf(bit2string(b, 16));

    PduInfoType pduinfo = {(uint8 *) b.M, (uint8 *) malloc(sizeof(uint8) * 1), 0};
    PduInfoType *pduinfo_ptr = &pduinfo;
    FVM_updateTrip(pduinfo_ptr);
    bitmap trip_bits = init_from_uint8(trip, sizeof(trip) * 8);

    printf("%s", bit2string(trip_bits, 24));
}


void getRxFreshness_test() {
    uint16 id = 0;

    uint8 truncated_freshness_value[8];
    memset(truncated_freshness_value, 0, sizeof(truncated_freshness_value));
    bitmap truncated_freshness_value_bits = init_from_uint8(truncated_freshness_value, 64);
    uint8 freshness_value[8];
    memset(freshness_value, 0, sizeof(freshness_value));
    bitmap freshness_value_bits = init_from_uint8(freshness_value, 64);
    uint32 freshness_value_length = 64;

    resetCnt[0].ResetCntLength = 16;
    uint8 reset_data[2];
    uint8 pre_reset_data[2];
    resetCnt[0].resetdata = reset_data;
    resetCnt[0].preresetdata = pre_reset_data;

    uint8 msg_data[2];
    uint8 pre_msg_data[2];
    msgCnt[0].msgdata = msg_data;
    msgCnt[0].premsgdata = pre_msg_data;

    uint64 result = 0;
    // received flag = 0
    for (int i = 0; i < 2; ++i) {
        switch (i) {
            case 0:
                // 1 latest flag = 0
                // lastest flag:
                resetState[0].resetflag = 0;

                // 1-1 & 1-2
                // latest trip|reset = pre trip|reset = 0
                trip[0] = trip[1] = 0;
                preTrip[0] = preTrip[1] = 0;
                reset_data[0] = reset_data[1] = 0;
                pre_reset_data[0] = pre_reset_data[1] = 0;
                // 1-1  pre msg < latest msg

                memset(freshness_value, 0, sizeof(freshness_value));

                pre_msg_data[0] = pre_msg_data[1] = 0;
                msg_data[0] = 0, msg_data[1] = 1;
                FVM_GetRxFreshness(id, truncated_freshness_value, 64, 0, freshness_value, &freshness_value_length);
                result = bit2uint64(freshness_value_bits, 0, 64);
                assert(result == 1);
                printf("Fomat 1-1 test: success!\n");
                // 1-2  pre msg >= latest msg

                memset(freshness_value, 0, sizeof(freshness_value));

                pre_msg_data[0] = pre_msg_data[1] = 0;
                msg_data[0] = msg_data[1] = 0;
                FVM_GetRxFreshness(id, truncated_freshness_value, 64, 0, freshness_value, &freshness_value_length);
                result = bit2uint64(freshness_value_bits, 0, 64);
                assert(result == (uint64) 1 << 16);
                printf("Fomat 1-2 test: success!\n");

                // 1-3
                // pre trip|reset > receive trip|reset
                trip[0] = trip[1] = 0;
                preTrip[0] = preTrip[1] = 0;
                reset_data[0] = 0, reset_data[1] = 0;
                pre_reset_data[0] = 0, pre_reset_data[1] = 1;

                memset(freshness_value, 0, sizeof(freshness_value));

                pre_msg_data[0] = pre_msg_data[1] = 0;
                msg_data[0] = msg_data[1] = 0;
                FVM_GetRxFreshness(id, truncated_freshness_value, 64, 0, freshness_value, &freshness_value_length);

                result = bit2uint64(freshness_value_bits, 0, 64);
                assert(result == ((uint64) 1 << 32));
                printf("Fomat 1-3 test: success!\n");
                break;
            case 1:
                // 1 latest flag - 1 = 0
                // lastest flag:
                resetState[0].resetflag = 1;

                // 2-1 & 2-2
                // latest - 1  = pre
                trip[0] = trip[1] = 0;
                preTrip[0] = preTrip[1] = 0;
                reset_data[0] = 0, reset_data[1] = 1;
                pre_reset_data[0], pre_reset_data[1] = 0;
                // 2-1
                memset(freshness_value, 0, sizeof(freshness_value));

                pre_msg_data[0] = pre_msg_data[1] = 0;
                msg_data[0] = 0, msg_data[1] = 1;
                FVM_GetRxFreshness(id, truncated_freshness_value, 64, 0, freshness_value, &freshness_value_length);

                result = bit2uint64(freshness_value_bits, 0, 64);
                assert(result == 1);
                printf("Fomat 2-1 test: success!\n");

                // 2-2
                memset(freshness_value, 0, sizeof(freshness_value));

                pre_msg_data[0] = pre_msg_data[1] = 0;
                msg_data[0] = msg_data[1] = 0;
                FVM_GetRxFreshness(id, truncated_freshness_value, 64, 0, freshness_value, &freshness_value_length);

                result = bit2uint64(freshness_value_bits, 0, 64);
                assert(result == ((uint64) 1 << 16));
                printf("Fomat 2-2 test: success!\n");


                // 2-3
                // latest -1 > pre
                trip[0] = trip[1] = 0;
                preTrip[0] = preTrip[1] = 0;
                reset_data[0] = 0, reset_data[1] = 3;
                pre_reset_data[0], pre_reset_data[1] = 0;

                memset(freshness_value, 0, sizeof(freshness_value));

                pre_msg_data[0] = pre_msg_data[1] = 0;
                msg_data[0] = msg_data[1] = 0;
                FVM_GetRxFreshness(id, truncated_freshness_value, 64, 0, freshness_value, &freshness_value_length);

                result = bit2uint64(freshness_value_bits, 0, 64);
                assert(result == ((uint64) 1 << 33));
                printf("Fomat 2-3 test: success!\n");

                break;
        }
    }
}

//GetTripValueTest:
//1:
//0xd,0xe0,0x21,0xa9,0xbd,0xfe,0xca,0x5c,
//2:
//0xd2,0xeb,0x16,0x24,0xa4,0x5b,0xaa,0xf3,
//3:
//0xff,0xdd,0xbd,0x3b,0x5,0x50,0x4e,0x88,
//4:
//0x1,0x23,0x4a,0x16,0x21,0x46,0x3a,0xe8,
//GetResetValueTest:
//1:
//0xe0,0x3,0x59,0xd2,0xa,0xad,0x95,0xab,
//2:
//0xa2,0x1,0x14,0xc3,0xd7,0x32,0xf9,0x41,
//3:
//0x33,0x36,0x0,0x90,0x71,0xad,0x38,0xde,
//4:
//0x5,0xe2,0x75,0x68,0x6d,0xe8,0x6f,0x2b,


void testTrip(){
    printf("TripTest:\n");
    PduInfoType pduInfoPtr;


    printf("1:\n");
    TripCntLength = 5;
    tripcanid = 0x2bd;
    uint8 data1[8] = {0xd,0xe0,0x21,0xa9,0xbd,0xfe,0xca,0x5c};
    pduInfoPtr.SduDataPtr = data1;
    uint8 result = FVM_updateTrip(&pduInfoPtr);
    assert(result == E_OK);
    printf("Success.\n");

    printf("2:\n");
    TripCntLength = 9;
    tripcanid = 0x2bd;
    uint8 data2[8] = {0xd2,0xeb,0x16,0x24,0xa4,0x5b,0xaa,0xf3};
    pduInfoPtr.SduDataPtr = data2;
    result = FVM_updateTrip(&pduInfoPtr);
    assert(result == E_OK);
    printf("Success.\n");


    printf("3:\n");
    TripCntLength = 16;
    tripcanid = 0x2bd;
    uint8 data3[8] = {0xff,0xdd,0xbd,0x3b,0x5,0x50,0x4e,0x88};
    pduInfoPtr.SduDataPtr = data3;
    result = FVM_updateTrip(&pduInfoPtr);
    assert(result == E_OK);
    printf("Success.\n");

    printf("1:\n");
    TripCntLength = 20;
    tripcanid = 0x2bd;
    uint8 data4[8] = {0x1,0x23,0x4a,0x16,0x21,0x46,0x3a,0xe8};
    pduInfoPtr.SduDataPtr = data4;
    result = FVM_updateTrip(&pduInfoPtr);
    assert(result == E_OK);
    printf("Success.\n");

}

void testReset(){
    printf("ResetTest:\n");
    PduInfoType pduInfoPtr;

    printf("1:\n");
    TripCntLength = 5;
    trip[0] = 1;
    resetCnt[0].ResetCntLength = 19;
    resetCnt[0].resetcanid = 0x0065;
    uint8 data1[8] = {0xe0,0x3,0x59,0xd2,0xa,0xad,0x95,0xab};
    pduInfoPtr.SduDataPtr = data1;
    uint8 result = FVM_updateReset(0,&pduInfoPtr);
    assert(result == E_OK);
    printf("Success.\n");

    printf("2:\n");
    TripCntLength = 9;
    trip[0] = 0x01;
    trip[1] = 0xa5;
    resetCnt[0].ResetCntLength = 8;
    resetCnt[0].resetcanid = 0x0065;
    uint8 data2[8] = {0xa2,0x1,0x14,0xc3,0xd7,0x32,0xf9,0x41};
    pduInfoPtr.SduDataPtr = data2;
    result = FVM_updateReset(0,&pduInfoPtr);
    assert(result == E_OK);
    printf("Success.\n");

    printf("3:\n");
    TripCntLength = 16;
    trip[0] = 0xff;
    trip[1] = 0xdd;
    resetCnt[0].ResetCntLength = 10;
    resetCnt[0].resetcanid = 0x0066;
    uint8 data3[8] = {0x33,0x36,0x0,0x90,0x71,0xad,0x38,0xde};
    pduInfoPtr.SduDataPtr = data3;
    result = FVM_updateReset(0,&pduInfoPtr);
    assert(result == E_OK);
    printf("Success.\n");

    printf("4:\n");
    TripCntLength = 20;
    trip[0] = 0x00;
    trip[1] = 0x12;
    trip[2] = 0x34;
    resetCnt[0].ResetCntLength = 7;
    resetCnt[0].resetcanid = 0x0066;
    uint8 data4[8] = {0x5,0xe2,0x75,0x68,0x6d,0xe8,0x6f,0x2b};
    pduInfoPtr.SduDataPtr = data4;
    result = FVM_updateReset(0,&pduInfoPtr);
    assert(result == E_OK);
    printf("Success.\n");

}

int main(int argc, char const *argv[]) {
//    testTrip();
    testReset();
    return 0;
}
