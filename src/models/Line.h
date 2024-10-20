#ifndef COMPUTER_GRAPHICS_LINE_H
#define COMPUTER_GRAPHICS_LINE_H

#include "../includes.h"

struct Line {
    int x0;
    int y0;
    int x1;
    int y1;
    Tool tool;
    int thickness = 3;
    Color color = {0, 0, 0};
};

#endif
