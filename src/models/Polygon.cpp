
export module Polygon;

import Point;
import Color;
import <vector>;
using std::vector;

export struct Polygon {
    vector<Point> v = {};
    bool completed = false;
    int thickness = 3;
    Color color = {0, 0, 0};
};
