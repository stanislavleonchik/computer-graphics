#ifndef COMPUTER_GRAPHICS_LINE_H
#define COMPUTER_GRAPHICS_LINE_H

#include "../includes.h"

struct line {
    float x0;
    float y0;
    float x1;
    float y1;
    Tool tool;
    int thickness;
};

#endif
