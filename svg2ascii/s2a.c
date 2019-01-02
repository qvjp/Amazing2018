#include <stdio.h>
#include <stdlib.h>
#include "./s2a.h"
#include <string.h>

// 要解析的svg路径
char *svg_path = "./a.svg";
char *txt_path = "./a.txt";

int main(int argc, char* argv[])
{
  char *svg;
  char map[S_HEIGHT][S_WIDTH];
  for (int i = 0; i < S_HEIGHT; i++)
    for (int j = 0; j < S_WIDTH; j++)
      map[i][j] = ' ';

  read_svg(&svg, svg_path);

  svg_to_map(svg, map);

  write_to_txt(map, txt_path);

  return 0;
}


// 将path读到svg中
int read_svg(char **svg, char *path)
{

  FILE *file = fopen(path, "rb");
  long lSize;
  size_t result;
  if (file == NULL)
  {
    printf("文件打开错误\n");
    exit (E_ERROR);
  }

  fseek(file, 0, SEEK_END);
  lSize = ftell(file);
  rewind(file);

  *svg = (char*)malloc(sizeof(char) * lSize);
  if (svg == NULL)
  {
    printf("内存分配错误\n");
    exit (E_ERROR);
  }

  result = fread(*svg, 1, lSize, file);
  if (result != lSize)
  {
    printf("文件读取错误\n");
    exit (E_ERROR);
  }

  fclose(file);
  return 0;
}

// 将map输出到path
int write_to_txt(char map[][S_WIDTH], char *path)
{
  FILE *file = fopen(path, "wb");
  if (file == NULL)
  {
    printf("文件保存错误\n");
    return E_ERROR;
  }
  for (int i = 0; i < S_HEIGHT; i++)
  {
    for (int j = 0; j < S_WIDTH; j++)
    {
      fprintf(file, "%c", map[i][j]);
      // printf("%c", map[i][j]);
    }
    fprintf(file, "\n");
  }
  fclose(file);
  return 0;
}

void copy_str(char *svg, char *svg_tmp)
{
  int i = 0;
  for (i = 0; svg[i] != '\0'; i++)
  {
    svg_tmp[i] = svg[i];
  }
  svg_tmp[i] = '\0';
}

void clear_tmp(char *num_tmp)
{
  for (int i = 0; i < 5; i++)
  {
    num_tmp[i] = '\0';
  }
}


// 将svg解析为map
int svg_to_map(char *svg, char map[][S_WIDTH])
{
  char *svg_tmp;
  char num_tmp[5];
  int len = strlen(svg);
  svg_tmp = (char*)malloc(sizeof(char) * len);
  while(*svg)
  {
    switch (*svg)
    {
      case 'r':
        {
          len = strlen(svg);
          copy_str(svg, svg_tmp);
          // strcpy(svg, svg_tmp);
          if(*++svg_tmp == 'e')
            if(*++svg_tmp == 'c')
              if(*++svg_tmp == 't')
                if (*++svg_tmp == ' ')
                {
                  // 处理<rect>
                  // printf("rect\n");
                  struct s2a_rect s;

                  // height
                  svg_tmp = strstr(svg_tmp, "height=");
                  svg_tmp = svg_tmp + 8;
                  for(int i = 0; *svg_tmp >= '0' && *svg_tmp <= '9'; i++)
                  {
                    num_tmp[i] = *svg_tmp++;
                  }
                  if (*svg_tmp == '%')
                    break;
                  s.h=atoi(num_tmp)/SCALE_Y;
                  clear_tmp(num_tmp);
                  // end height
                  // width
                  svg_tmp = strstr(svg_tmp, "width=");
                  svg_tmp = svg_tmp + 7;
                  for(int i = 0; *svg_tmp >= '0' && *svg_tmp <= '9'; i++)
                  {
                    num_tmp[i] = *svg_tmp++;
                  }
                  s.w=atoi(num_tmp)/SCALE_X;
                  clear_tmp(num_tmp);
                  // end width
                  // y
                  svg_tmp = strstr(svg_tmp, "y=");
                  svg_tmp = svg_tmp + 3;
                  for(int i = 0; *svg_tmp >= '0' && *svg_tmp <= '9'; i++)
                  {
                    num_tmp[i] = *svg_tmp++;
                  }
                  s.y=atoi(num_tmp)/SCALE_Y;
                  clear_tmp(num_tmp);
                  // end y
                  // x
                  svg_tmp = strstr(svg_tmp, "x=");
                  svg_tmp = svg_tmp + 3;
                  for(int i = 0; *svg_tmp >= '0' && *svg_tmp <= '9'; i++)
                  {
                    num_tmp[i] = *svg_tmp++;
                  }
                  s.x=atoi(num_tmp)/SCALE_X;
                  clear_tmp(num_tmp);
                  // end x

                  printf("This struct is: s.x: %d s.y: %d s.h: %d s.w: %d\n", s.x, s.y, s.h, s.w);

                  for (int i = s.x; i < s.x+s.w; i++)
                    map[s.y][i] = '-';
                  for (int i = s.x; i < s.x+s.w; i++)
                    map[s.y+s.h][i] = '-';
                  for (int i = s.y; i <= s.y+s.h; i++)
                    map[i][s.x] = '|';
                  for (int i = s.y; i <= s.y+s.h; i++)
                    map[i][s.x+s.w] = '|';

                  len = strlen(svg_tmp);
                  copy_str(svg_tmp, svg);
                }
        }
        break;
      case 'p':
        {

        }
        break;
      case 'l':
        {
          len = strlen(svg);
          copy_str(svg, svg_tmp);
          // strcpy(svg, svg_tmp);
          if(*++svg_tmp == 'i')
            if(*++svg_tmp == 'n')
              if(*++svg_tmp == 'e')
                if (*++svg_tmp == ' ')
                {
                  // 处理<line>

                  struct s2a_line l;

                  // y2
                  svg_tmp = strstr(svg_tmp, "y2=");
                  svg_tmp = svg_tmp + 4;
                  for(int i = 0; *svg_tmp >= '0' && *svg_tmp <= '9'; i++)
                  {
                    num_tmp[i] = *svg_tmp++;
                  }
                  l.y2=atoi(num_tmp)/SCALE_Y;
                  clear_tmp(num_tmp);
                  // end y2

                  // x2
                  svg_tmp = strstr(svg_tmp, "x2=");
                  svg_tmp = svg_tmp + 4;
                  for(int i = 0; *svg_tmp >= '0' && *svg_tmp <= '9'; i++)
                  {
                    num_tmp[i] = *svg_tmp++;
                  }
                  l.x2=atoi(num_tmp)/SCALE_X;
                  clear_tmp(num_tmp);
                  // end x2

                  // y1
                  svg_tmp = strstr(svg_tmp, "y1=");
                  svg_tmp = svg_tmp + 4;
                  for(int i = 0; *svg_tmp >= '0' && *svg_tmp <= '9'; i++)
                  {
                    num_tmp[i] = *svg_tmp++;
                  }
                  l.y1=atoi(num_tmp)/SCALE_Y;
                  clear_tmp(num_tmp);
                  // end y1

                  // x1
                  svg_tmp = strstr(svg_tmp, "x1=");
                  svg_tmp = svg_tmp + 4;
                  for(int i = 0; *svg_tmp >= '0' && *svg_tmp <= '9'; i++)
                  {
                    num_tmp[i] = *svg_tmp++;
                  }
                  l.x1=atoi(num_tmp)/SCALE_X;
                  clear_tmp(num_tmp);
                  // end x1
                  //
                  //printf("%d %d %d %d \n", l.y2, l.x2, l.y1, l.x1);

                  if (l.y1 == l.y2)
                  {
                    for (int i = l.x1; i < l.x2; i++)
                      map[l.y1][i] = '-';
                  }
                  if (l.x1 == l.x2)
                  {
                    for (int i = l.y1; i < l.y2; i++)
                      map[i][l.x1] = '|';
                  }

                  len = strlen(svg_tmp);
                  copy_str(svg_tmp, svg);
                }

        }
        break;
defalut:
        break;
    }
    *svg++;

  }
  /*
  for (int i = 0; i < S_HEIGHT; i++)
    for (int j = 0; j < S_WIDTH; j++)
    {
      map[i][j] = '-';
    }
    */

  return 0;
}

