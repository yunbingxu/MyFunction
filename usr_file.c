#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>


/* 读取文件，返回字符串，由调用者释放 */
char* getFileContent(const char* path)
{
    uint32_t fileSize = 0;
    char*    content  = NULL;
    FILE*    fp       = fopen(path, "rb");

    if (fp == NULL) {
        printf("open file %s failed!\n", path);
        fp = fopen(path, "rb");
    }
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    content  = (char*)malloc(fileSize + 1);
    memset(content, 0, fileSize + 1);
    fseek(fp, 0, SEEK_SET);
    fread(content, 1, fileSize, fp);
    fclose(fp);
    fp = NULL;

    return content;
}