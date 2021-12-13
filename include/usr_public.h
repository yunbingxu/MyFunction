#ifndef USR_PUBLIC_H
#define USR_PUBLIC_H

#include <time.h>
#include <stdint.h>


#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define TIME_STR_LEN    19 /* "YYYY-MM-DD HH:MM:SS" 不含结束符 */

#define USR_FREE(p)     do { if (p) { free(p); p = NULL; } } while (0)
#define USR_SWAP(a, b)  do { typeof(a) tmp = a; a = b; b = tmp; } while (0)


/* 函数声明 */

/* 通用 */
extern int printAppInfo(const char* szName, const char* szVersion);

/* 数学 */
extern int    randNum(int min, int max);
extern char** charArray(uint16_t rows, uint16_t cols);
extern int    freeCharArray(char** arr, uint16_t rows);

/* 时间 */
extern time_t timeStr2Second(const char* str);
extern char*  second2TimeStr(time_t sec);

#endif /* USR_PUBLIC_H */