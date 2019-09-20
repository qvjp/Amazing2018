/**
 * @file hex2bin.c
 * @author 屈俊平 (qvjunping@gmail.com)
 * @date 2019-9-20
 * @version 1
 * @brief 转换16进制文件到2进制文件
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1
#define FILEPATH 256
#define TARGET_FILE "target.txt"

int factory(int ch)
{
    if (ch >= 0 && ch <= 9)
        ch += 48;
    if (ch >= 10 && ch <= 15)
        ch += 55;
    return ch;
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        printf("Param Error!\n\n");
        printf("You Should:\n");
        printf("bin2hex src.bin [target.hex]\n\n");
        return -1;
    }
    char ch1 = '\0';
    char ch2 = '\0';
    unsigned char buf[BUFSIZE] = {0};
    char file_path1[FILEPATH] = {0};
    char file_path2[FILEPATH] = {0};

    memcpy(file_path1, argv[1], strlen(argv[1]));
    if (argc == 2)
        memcpy(file_path2, TARGET_FILE, strlen(TARGET_FILE));
    else
        memcpy(file_path2, argv[2], strlen(argv[2]));

    FILE *file1 = fopen(file_path1, "r");
    FILE *file2 = fopen(file_path2, "wb");
    if (file1 == NULL)
    {
        printf("Open %s Error\n", file_path1);
        return -1;
    }
    if (file2 == NULL)
    {
        printf("Open %s Error\n", file_path2);
        return -1;
    }
    int num = 0;
    while (fread(buf, 1, 1, file1) > 0)
    {
        num ++;
        ch1 = (*buf >> 4) & 0xF;
        ch2 = *buf & 0xF;
        ch1 = factory((unsigned int)ch1);
        ch2 = factory((unsigned int)ch2);
        fputc(ch1, file2);
        fputc(ch2, file2);
        if (num % 16 == 0)
        {
            num = 0;
            fputc('\r', file2);
            fputc('\n', file2);
        }
        else if (num % 2 == 0)
        {
            fputc(' ', file2);
        }

    }


    fclose(file1);
    fclose(file2);

    return 0;
}

