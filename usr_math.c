#include <stdlib.h>
#include <time.h>

#include "usr_public.h"


/* 返回 [min,max) 随机数 */
int randNum(int min, int max)
{
    srand((unsigned)time(NULL) + rand());
    return min + rand() % (max - min);
}


static int mallocArray2D(uint16_t rows, uint16_t cols, uint8_t typeSize)
{
    uint16_t i = 0;

    void** arr = malloc(rows * cols * sizeof(void*));
    if (arr == NULL) {
        SLOG_WARN("mallocArray2D: malloc failed!\n");
        return (-1);
    }

    for (i = 0; i < rows; i++) {
        arr[i] = malloc(cols * typeSize);
        if (arr[i] == NULL) {
            SLOG_WARN("mallocArray2D: malloc failed!\n");
            return (-1);
        }
        memset(arr[i], 0, cols * typeSize);
    }

    return arr;
}


/* 动态创建二维数组,ArrayTypr:0-char 1-int 2-float */
void** array2D(uint16_t rows, uint16_t cols, ArrayType_e type)
{
    void** arr = NULL;

    switch(type)
    {
    case TYPE_CHAR:
        arr = mallocArray2D(rows, cols, sizeof(char));
        break;
    case TYPE_INT:
        arr = mallocArray2D(rows, cols, sizeof(int));
        break;
    case TYPE_FLOAT:
        arr = mallocArray2D(rows, cols, sizeof(float));
        break;
    default:
        break;
    }

    return arr;
}


/* 释放动态二维数组 */
int freeArray2D(void** arr, uint16_t rows)
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