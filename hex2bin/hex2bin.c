/**
 * @file hex2bin.c
 * @author 屈俊平 (qvjunping@gmail.com)
 * @date 2019-9-20
 * @version 1
 * @brief 转换16进制文件到二进制文件
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1
#define FILEPATH 256
#define TARGET_FILE "target.bin"

char factory(char ch)
{
    if (ch >= 48 && ch <= 57)
        ch -= 48;
    if (ch >= 65 && ch <= 70)
        ch -= 55;
    if (ch >= 97 && ch <= 102)
        ch -= 87;
    return ch;
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        printf("Param Error!\n\n");
        printf("You Should:\n");
        printf("hex2bin src.hex [target.bin]\n\n");
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

    while ((ch1 = fgetc(file1)) != EOF)
    {
        while (ch1 == ' ' || ch1 == '\r' || ch1 == '\n')
            ch1 = fgetc(file1);
        if ((ch2 = fgetc(file1)) == EOF)
            break;
        while (ch2 == ' ' || ch2 == '\r' || ch2 == '\n')
            ch2 = fgetc(file1);

        ch1 = factory(ch1);
        ch2 = factory(ch2);

        *buf = ch1 << 4 | (ch2 & 0xF);
        *buf = *buf & 0xFF;

        fwrite(buf, 1, 1, file2);
    }

    fclose(file1);
    fclose(file2);

    return 0;
}

