#include "bitmap.h"

// 初始化
bitmap init(int n) {
    int len = (n + 8 - 1) / 8; // 需要的空间(8bit)
    bitmap b = {(char *) malloc(sizeof(char) * len), len};
    memset(b.M, 0, b.N);
    return b;
}

bitmap init_from_uint8(const uint8 *data, int n) {
    int len = (n + 8 - 1) / 8; // 需要的空间(8bit)
    bitmap b = {(char *) data, len};
    return b;
}

// 扩充
void expand(bitmap b, int k) {
    if (k < 8 * b.N)
        return;
    int oldN = b.N;
    char *oldM = b.M;
    b = init(2 * k); // 扩大两倍,可配置
    memcpy_s(b.M, b.N, oldM, oldN);
    free(oldM);
}

// 回收
void destroy(bitmap b) { free(b.M); }

// 操作
void set(bitmap b, int k) {
    expand(b, k);
    b.M[k >> 3] |= (0x80 >> (k & 0x07));
} //设置比特位
void clear(bitmap b, int k) {
    expand(b, k);
    b.M[k >> 3] &= ~(0x80 >> (k & 0x07));
} //清除比特位
bool test(bitmap b, int k) {
    expand(b, k);
    return b.M[k >> 3] & (0x80 >> (k & 0x07));
} //测试比特位

// 打印为字符串
char *bit2string(bitmap b, int n) {
    expand(b, n - 1);
    char *s = (char *) malloc(sizeof(char) * (n + 1));
    s[n] = '\0';
    for (int i = 0; i < n; i++)
        s[i] = test(b, i) ? '1' : '0';
    return s;
}

uint64 bit2uint64(bitmap b, int start, int n) {
    uint8 start_index = (start + 8 - 1) / 8;
    uint8 length = (start + n + 8 - 1) / 8;
    uint64 value = 0;
    for (int i = start_index; i < length; i++) {
        value += (uint64) b.M[length - 1 - i] << ((i - start_index) * 8);
    }
    return value;
}

void copy(bitmap b, int start, bitmap data, int n) {
    //bitmap temp = init_from_uint8(data, n);
    for (int i = 0; i < n; i++) {
        if (test(data, i))
            set(b, start + i);
    }
}

//void copyofuint(bitmap b, uint8 start, uint64 data, int n) {}