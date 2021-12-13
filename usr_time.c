#include <stdio.h>
#include <stdlib.h>

#include "usr_public.h"


time_t timeStr2Second(const char* str)
{
    struct tm cur_tm;

    if (strlen(str) != TIME_STR_LEN) {
        printf("timestr[%s] error!\n", str); /* "2000-01-01 00:00:00" */
        return 0;
    }
    sscanf(str,
           "%04d-%02d-%02d %02d:%02d:%02d",
           &cur_tm.tm_year,
           &cur_tm.tm_mon,
           &cur_tm.tm_mday,
           &cur_tm.tm_hour,
           &cur_tm.tm_min,
           &cur_tm.tm_sec);
    if (0 == cur_tm.tm_year) {
        printf("timeStr[%s] is invalid!\n", str);
        return 0;
    }
    cur_tm.tm_year -= 1900;
    cur_tm.tm_mon -= 1;

    return mktime(&cur_tm);
}


char* second2TimeStr(time_t sec)
{
    static char timeStr[24];
    struct tm   cur_tm;

    memset(timeStr, 0, sizeof(timeStr));
    memcpy(&cur_tm, localtime(&sec), sizeof(struct tm));

    sprintf(timeStr,
            "%04d-%02d-%02d %02d:%02d:%02d",
            cur_tm.tm_year + 1900,
            cur_tm.tm_mon + 1,
            cur_tm.tm_mday,
            cur_tm.tm_hour,
            cur_tm.tm_min,
            cur_tm.tm_sec);

    return timeStr;
}