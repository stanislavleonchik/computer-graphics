#ifndef COMPUTER_GRAPHICS_POLYGON_H
#define COMPUTER_GRAPHICS_POLYGON_H

#include "../includes.h"

struct Polygon {
    vector<Point> v = {};
    bool completed = false;
    int thickness = 3;
    Color color = {0, 0, 0};
};

#endif
