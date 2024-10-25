#pragma once

#include "Point.h"
#include "Color.h"
using std::vector;

struct Polygon {
    vector<Point> v = {};
    bool completed = false;
    int thickness = 3;
    Color color = {0, 0, 0};
};
