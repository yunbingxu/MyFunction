#include <stdlib.h>
#include <time.h>

#include "usr_public.h"


/* 返回 [min,max) 随机数 */
int randNum(int min, int max)
{
    srand((unsigned)time(NULL) + rand());
    return min + rand() % (max - min);
}


/* 动态创建char型二维数组 */
char** charArray(uint16_t rows, uint16_t cols)
{
    uint16_t i;

    char** arr = (char**)malloc(rows * sizeof(char*));
    for (i = 0; i < rows; i++) {
        arr[i] = (char*)calloc(cols, sizeof(char));
    }

    return arr;
}


/* 释放动态char型二维数组 */
int freeCharArray(char** arr, uint16_t rows)
{
    uint16_t i;

    for (i = 0; i < rows; i++) {
        USR_FREE(arr[i]);
    }
    USR_FREE(arr);

    return 0;
}


/* 字符串数字的倍率转换 */
int numstrScalerDeal(char* szNum, int scaler, int type)
{
    uint8_t i;
    uint8_t len = strlen(szNum);
    uint8_t szTmp[32];

    if (scaler == 0) { /* 无需处理 */
        return 0;
    }

    uint8_t pos = 1;
    for (pos = 0; pos < len; pos++) {
        if (szNum[pos] == '.') {
            break;
        }
    }
    szNum[pos] = '.';

    if (scaler < 0) { /* 小数点左移 */
        scaler *= -1;
        i = pos;
        if (szNum[0] == '-' || szNum[0] == '+') { /* 带符号的数字前面应多补偿一个0 */
            i--;
        }
        for (; scaler + 1 > i; i++, pos++) { /* 数字前补0 */
            strcpy(szTmp, szNum);
            if (szNum[0] == '-') {
                sprintf(szNum, "-0%s", szTmp + 1);
            }
            else if (szNum[0] == '+') {
                sprintf(szNum, "+0%s", szTmp + 1);
            }
            else {
                sprintf(szNum, "0%s", szTmp);
            }
        }
        for (i = 0; i < scaler; i++) {
            USR_SWAP(szNum[pos], szNum[pos - 1]);
            pos--;
        }
    }
    else { /* 小数点右移 */
        for (i = 0; i < scaler; i++) {
            if (szNum[pos + 1] == '\0') {
                szNum[pos + 1] = '0';
                szNum[pos + 2] = '\0';
            }
            USR_SWAP(szNum[pos], szNum[pos + 1]);
            pos++;
        }
    }
    /* 处理末尾多余的零 */
    len = strlen(szNum);
    while (szNum[len - 1] == '0' || szNum[len - 1] == '.') {
        szNum[len - 1] = '\0';
        len--;
    }
    if (len == 0) {
        szNum[0] = '0';
    }

    return 0;
}