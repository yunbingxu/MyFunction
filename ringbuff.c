#include <stdio.h>
#include <stdlib.h>

#include "ringbuff.h"

#include "usr_public.h"


/* 全局变量 */
RingBuff_t g_ringBuff;


/* 环形队列初始化 */
RingBuff_t* RingBuff_init(void)
{
    RingBuff_t* p = (RingBuff_t*)malloc(sizeof(RingBuff_t));
    if (p == NULL) {
        SLOG_INFO("RingBuff_init: pRing is NULL!\n");
        return NULL;
    }
    p->write = 0;
    p->read = 0;

    return p;
}


/* 判断环形队列是否已经满了 */
int RingBuff_isFull(RingBuff_t* pRing)
{
    /* 写位置减去读位置等于队列长度，则环形队列已满 */
    if (RING_LEN == aabs((pRing->write & (RING_LEN - 1)) - (pRing->read & (RING_LEN - 1)))) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


/* 判断环形队列为空 */
int RingBuff_isEmpty(RingBuff_t* pRing)
{
    /* 写位置和读的位置相等，则环形队列为空 */
    if ((pRing->write & (RING_LEN - 1)) == (pRing->read & (RING_LEN - 1))) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


/* 插入数据 */
int RingBuff_write(RingBuff_t* pRing, void* pData, uint16_t size)
{
    if (pRing == NULL) {
        SLOG_INFO("pRing is NULL!\n");
        return (-1);
    }
    if (RingBuff_isFull(pRing)) {
        SLOG_INFO("buff is full\n");
        return (-2);
    }

    memcpy(pRing->pData[pRing->write & (RING_LEN - 1)], pData, size);
    pRing->write++;

    return (0);
}


/* 读取数据 */
void* RingBuff_read(RingBuff_t* pRing)
{
    void* pData;

    if (pRing == NULL) {
        SLOG_INFO("p null\n");
        return NULL;
    }
    if (RingBuff_isEmpty(pRing)) {
        SLOG_INFO("buff is empty\n");
        return NULL;
    }

    pData = pRing->pData[pRing->read & (RING_LEN - 1)];
    pRing->read++;

    return pData;
}


/* 销毁 */
int RingBuff_destory(RingBuff_t* pRing)
{
    uint16_t i;
    if (pRing == NULL) {
        SLOG_INFO("p null\n");
        return (-1);
    }

    for (i = 0; i < RING_LEN; i++) {
        USR_FREE(pRing->pData[i]);
    }
    USR_FREE(pRing);

    return (0);
}
