#include <stdlib.h>
#include <stdint.h>


/* 动态创建char型二维数组 */
char** createCharArray(uint16_t rows, uint16_t cols)
{
    uint16_t i;

    char** arr = (char**)malloc(rows * sizeof(char*));
    for (i = 0; i < rows; i++) {
        arr[i] = (char*)calloc(cols, sizeof(char));
    }

    return arr;
}


/* 释放动态char型二维数组 */
uint8_t freeCharArray(char** arr, uint16_t rows)
{
    uint16_t i;

    for (i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);

    return 0;
}