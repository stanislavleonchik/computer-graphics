#ifndef COMPUTER_GRAPHICS_LINE_H
#define COMPUTER_GRAPHICS_LINE_H

#include "../includes.h"

struct Line {
    float x0;
    float y0;
    float x1;
    float y1;
    Tool tool;
    int thickness = 3;
    Color color = {0, 0, 0};
};

#endif
