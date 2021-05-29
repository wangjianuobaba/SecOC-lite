#include "slaveFVM.h"

uint8 trip[3];

void trip_test() {
    bitmap b = init(16);
    //printf(bit2string(b, 16));
    //printf("\n");
    set(b, 0);
    set(b, 8);
    //printf(bit2string(b, 16));

    PduInfoType pduinfo = {(uint8 *)b.M, (uint8 *)malloc(sizeof(uint8) * 1), 0};
    PduInfoType *pduinfo_ptr = &pduinfo;
    FVM_updateTrip(pduinfo_ptr);
    bitmap trip_bits = init_from_uint8(trip, sizeof(trip) * 8);

    printf("%s", bit2string(trip_bits, 24));
}

int main(int argc, char const *argv[]) {
    trip_test();
    return 0;
}
