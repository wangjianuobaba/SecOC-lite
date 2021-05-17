#pragma once

#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<stdbool.h>

#define min(a, b) (a < b ? a:b)

typedef struct Bitmap {
    char *M; // 地址空间
    int N; // char(8bit)的个数
} bitmap;

// 初始化
bitmap init(int n) {
    int len = (n + 8 - 1) / 8; // 需要的空间(8bit)
    bitmap b = {(char *) malloc(sizeof(char) * len), len};
    memset(b.M, 0, b.N);
    return b;
}
bitmap init_from_uint8(uint8* data,int n){
    bitmap b;
    b.M = (char*)data;
    b.N = n;
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