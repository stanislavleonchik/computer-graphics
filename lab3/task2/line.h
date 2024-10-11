#ifndef COMPUTER_GRAPHICS_LINE_H
#define COMPUTER_GRAPHICS_LINE_H

#include "../includes.h"

struct line {
    int x0;
    int y0;
    int x1;
    int y1;
    Tool tool;
    int thickness;
};

#endif
