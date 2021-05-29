#include <assert.h>
#include "slaveFVM.h"

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

int main(int argc, char const *argv[]) {
    getRxFreshness_test();
    return 0;
}
