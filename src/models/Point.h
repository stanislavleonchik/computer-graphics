#ifndef COMPUTER_GRAPHICS_POINT_H
#define COMPUTER_GRAPHICS_POINT_H

#include"../includes.h"

struct Point {
    float x, y;

    Point(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Point() {
        x = 0;
        y = 0;
    }

    Point(ImVec2 p) {
        x = p.x;
        y = p.y;
    }

    Point operator-(const Point& p) {
        return Point{x-p.x, y-p.y};
    }

    Point operator+(const Point& p) {
        return Point{ x + p.x, y + p.y };
    }

    Point operator*(const Point& p) {
        return Point{ x * p.x, y * p.y };
    }

    Point operator/(const Point& p) {
        return Point{ x / p.x, y / p.y };
    }

    Point& operator = (const Point& p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }
    Point& operator = (const ImVec2& p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }
};

#endif
