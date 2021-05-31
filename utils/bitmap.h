#pragma once

#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<stdbool.h>
#include"../include/Platform_Types.h"

#define min(a, b) (a < b ? a:b)

typedef struct Bitmap {
    char *M; // 地址空间
    int N; // char(8bit)的个数
} bitmap;

// 初始化
bitmap init(int n);
bitmap init_from_uint8(const uint8 *data, int n);
// 扩充
void expand(bitmap b, int k);

// 回收
void destroy(bitmap b);

// 操作
void set(bitmap b, int k); //设置比特位
void clear(bitmap b, int k); //清除比特位
bool test(bitmap b, int k); //测试比特位

// 接口
char *bit2string(bitmap b, int n); // 打印为字符串
uint64 bit2uint64(bitmap b, int start, int n); // 返回
void copy(bitmap b, int start, bitmap data, int n); // 拷贝
//void copyofuint(bitmap b, int start, uint64 data, int n); // 拷贝
