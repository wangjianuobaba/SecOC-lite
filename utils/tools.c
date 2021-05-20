#include "tools.h"

// k=0..8,设置num的第k位为1
uint8 set_k(uint8 num, uint8 k) {
    num &= 1 << k;
    return num;
}

//第k位是否为1
uint8 is_k(uint64 num, uint8 k) {
    if (num & (1 << k)) {
        return 1;
    }
    else {
        return 0;
    }
}

// 返回一个uint8的实际位数
uint8 length(uint8 num) {
    int temp = 0;
    while (num) {
        temp += 1;
        num >>= 1;
    }
    return temp;
}