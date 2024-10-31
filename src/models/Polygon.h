#pragma once

#include "Point.h"
using std::vector;

struct Polygon {
    vector<Point> v = {};
    bool completed = false;
    int thickness = 3;
    ImColor color = {0, 0, 0};
};
