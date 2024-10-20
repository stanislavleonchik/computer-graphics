#ifndef CHECKPOINTPOSITIONRELATIVEEDGE_H
#define CHECKPOINTPOSITIONRELATIVEEDGE_H

#include "../../includes.h"

bool check_point_position_relative_edge(const Line &edge, const int x, const int y ) {
    int pointX = x - edge.x0;
    int pointY = y - edge.y0;
    int edgeX = edge.x1 - edge.x0;
    int edgeY = edge.y1 - edge.y0;
    return pointY * edgeX - pointX * edgeY > 0;
}

#endif
