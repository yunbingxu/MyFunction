#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


static int8_t makeStarStr(char* str, uint8_t nb)
{
    int8_t i = 0;
    for (i = 0; i < nb; i++) {
        str[i] = '*';
    }
    return 0;
}


static int8_t makeInfoStr(char* str, uint8_t nb)
{
    int8_t  i   = 0;
    uint8_t len = strlen(str);

    for (i = len; i < nb - 1; i++) {
        str[i] = ' ';
    }
    str[nb - 1] = '*';

    return 0;
}


int8_t printAppInfo(const char* szName, const char* szVersion)
{
    char strStars[128]   = {0};
    char strAppInfo[128] = {0};
    char strAppVer[128]  = {0};
    char strAppDate[128] = {0};

    uint8_t maxLen = 0;

    sprintf(strAppInfo, "* This is \"%s\" APP", szName);
    sprintf(strAppVer,  "* Version: %s", szVersion);
    sprintf(strAppDate, "* Build time: %s, %s", __DATE__, __TIME__);

    if (strlen(strAppInfo) > maxLen) {
        maxLen = strlen(strAppInfo);
    }
    if (strlen(strAppVer) > maxLen) {
        maxLen = strlen(strAppVer);
    }
    if (strlen(strAppDate) > maxLen) {
        maxLen = strlen(strAppDate);
    }

    makeStarStr(strStars,   maxLen + 2);
    makeInfoStr(strAppInfo, maxLen + 2);
    makeInfoStr(strAppVer,  maxLen + 2);
    makeInfoStr(strAppDate, maxLen + 2);

    printf("\n");
    printf("%s\n", strStars);
    printf("%s\n", strAppInfo);
    printf("%s\n", strAppVer);
    printf("%s\n", strAppDate);
    printf("%s\n", strStars);
    printf("\n");

    return 0;
}