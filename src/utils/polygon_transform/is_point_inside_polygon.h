#ifndef ISPOINTINSIDEPOLYGON_H
#define ISPOINTINSIDEPOLYGON_H

#include "../../includes.h"

bool is_intersecting(Point p, Point v1, Point v2) {
    // Проверяем, пересекает ли горизонтальный луч от точки p ребро [v1, v2]
    if ((v1.y > p.y) != (v2.y > p.y)) {
        double intersectX = v1.x + (p.y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
        return intersectX > p.x;
    }
    return false;
}

bool is_point_inside_polygon(Point p, const Polygon& polygon) {
    int intersections = 0;

    for (size_t i = 0; i < polygon.v.size(); i++) {
        Point v1 = polygon.v[i];
        Point v2 = polygon.v[(i + 1) % polygon.v.size()]; // Следующая вершина, с замыканием

        if (is_intersecting(p, v1, v2)) {
            intersections++;
            if (p.x == v1.x && p.y == v1.y)
                intersections++;
        }
    }

    return intersections % 2 == 1; // Нечётное число пересечений — точка внутри
}

#endif
