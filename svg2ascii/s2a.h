/**
 *
 * 将SVG格式文件翻译成ASCI字符
 * 当然，SVG中只能包含path, rect,
 */
#ifndef __SVG2ASCII__H__
#define __SVG2ASCII__H__

#define E_ERROR 65

#define S_HEIGHT 25
#define S_WIDTH 60

#define SCALE_Y 16
#define SCALE_X 10


struct s2a_line {
  int y2;
  int x2;
  int y1;
  int x1;
};

struct s2a_rect {
    int x;
    int y;
    int h;
    int w;
};


int read_svg(char **svg, char *path);
int svg_to_map(char *svg, char map[][S_WIDTH]);

int write_to_txt(char map[][S_WIDTH], char *path);
#endif
