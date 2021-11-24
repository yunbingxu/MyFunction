#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* 读取文件，返回字符串，由调用者释放 */
static char* getFileContent(const char* path)
{
    uint32_t fileSize = 0;
    char*    content  = NULL;
    FILE*    fp       = fopen(path, "rb");

    while (fp == NULL) {
        SLOG_ERROR("open file %s failed!\n", path);
        fp = fopen(path, "rb");
        Osl_Delay(300);
    }
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    content  = (char*)malloc(fileSize + 1);
    fread(content, 1, fileSize, fp);
    fclose(fp);
    fp = NULL;

    return content;
}