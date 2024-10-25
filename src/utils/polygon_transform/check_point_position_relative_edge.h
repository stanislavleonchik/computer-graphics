#include "../../models/Line.h"

bool check_point_position_relative_edge(const Line &edge, const int x, const int y ) {
    int pointX = x - (int)edge.x0;
    int pointY = y - (int)edge.y0;
    int edgeX = (int)(edge.x1 - edge.x0);
    int edgeY = (int)(edge.y1 - edge.y0);
    return pointY * edgeX - pointX * edgeY > 0;
}
