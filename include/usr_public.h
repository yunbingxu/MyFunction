#ifndef USR_PUBLIC_H
#define USR_PUBLIC_H

#include <time.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif


#define USR_SWAP(a, b)  do { typeof(a) tmp = a; a = b; b = tmp; } while (0)

/***************************************************************************************/

/* 通用 */
#define USR_FREE(p)     do { if (p) { free(p); p = NULL; } } while (0)

extern int printAppInfo(const char* szName, const char* szVersion);

/***************************************************************************************/

/* 文件 */
extern const char* fileContent(const char* path);

/***************************************************************************************/

/* 数学 */
typedef enum {
    TYPE_CHAR,
    TYPE_INT,
    TYPE_FLOAT
} ArrayType_e;

extern int    randNum(int min, int max);
extern void** array2D(uint16_t rows, uint16_t cols, ArrayType_e type);
extern int    freeArray2D(void** arr, uint16_t rows);

/***************************************************************************************/

/* 时间 */
#define TIME_STR_LEN    19 /* "YYYY-MM-DD HH:MM:SS" 不含结束符 */

typedef struct tm tms_t;

extern time_t timeStr2Second(const char* str);
extern char*  second2TimeStr(time_t sec);
extern tms_t  currentTime(time_t curSec);

/***************************************************************************************/

/* ringbuff.c 环形队列 */
#define RING_LEN 8 /* 注意：应为2^n */

typedef struct {
    uint16_t write;
    uint16_t read;
    void*    pData[RING_LEN];
} RingBuff_t;

extern RingBuff_t* RingBuff_init(void);
extern int         RingBuff_isFull(RingBuff_t* pRing);
extern int         RingBuff_isEmpty(RingBuff_t* pRing);
extern int         RingBuff_write(RingBuff_t* pRing, void* pData, uint16_t size);
extern void*       RingBuff_read(RingBuff_t* pRing);
extern int         RingBuff_destory(RingBuff_t* pRing);

/***************************************************************************************/

/* slog.c 日志管理 */

typedef enum {
    S_TRACE = 1,
    S_DEBUG = 2,
    S_INFO  = 3,
    S_WARN  = 4,
    S_ERROR = 5
} SlogLevel_e;

/* 注意：目录目前只能自动新建一级文件夹 */
int SLOG_init(const char* log_dir, const char* file_name, SlogLevel_e level);

/* 注意：接口一次输出的内容最大长度约为10KB，超出长度会发生截断。通过MAX_LOG_LINE宏可修改最大长度 */
void write_log(SlogLevel_e level, int braw, const char* func_name, int line, const char* fmt, ...);

#define SLOG_ERROR(fmt, ...) write_log(S_ERROR, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_WARN(fmt, ...)  write_log(S_WARN, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_INFO(fmt, ...)  write_log(S_INFO, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG(fmt, ...) write_log(S_DEBUG, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_TRACE(fmt, ...) write_log(S_TRACE, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_ERROR(fmt, ...) write_log(S_ERROR, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define SLOG_ERROR_RAW(fmt, ...) write_log(S_ERROR, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_WARN_RAW(fmt, ...)  write_log(S_WARN, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_INFO_RAW(fmt, ...)  write_log(S_INFO, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG_RAW(fmt, ...) write_log(S_DEBUG, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_TRACE_RAW(fmt, ...) write_log(S_TRACE, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

/* demo
 * SLOG_init("./log", "slog.log", S_TRACE);
 * SLOG_ERROR("SLOG ERROR TEST");
 * SLOG_WARN("SLOG WARN TEST");
 **/

/***************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* USR_PUBLIC_H */