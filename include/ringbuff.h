#ifndef _RINGBUFF_H_
#define _RINGBUFF_H_

#include <stdint.h>


#define RING_LEN    8 /* 注意：应为2^n */


/* 环形队列结构体 */
typedef struct {
    uint16_t write;
    uint16_t read;
    void*    pData[RING_LEN];
} RingBuff_t;


/* 全局变量 */
extern RingBuff_t g_ringBuff;


/* 函数声明 */
extern RingBuff_t* RingBuff_init(void);
extern int         RingBuff_isFull(RingBuff_t* pRing);
extern int         RingBuff_isEmpty(RingBuff_t* pRing);
extern int         RingBuff_write(RingBuff_t* pRing, void* pData, uint16_t size);
extern void*       RingBuff_read(RingBuff_t* pRing);
extern int         RingBuff_destory(RingBuff_t* pRing);


#endif /* _RINGBUFF_H_ */