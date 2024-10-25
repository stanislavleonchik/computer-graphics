#pragma once

#include "imgui.h"

struct Point {
    float x, y;

    Point(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Point operator-(const Point& p) const {
        return Point{x-p.x, y-p.y};
    }

    Point operator+(const Point& p) const {
        return Point{ x + p.x, y + p.y };
    }

    Point operator*(const Point& p) const {
        return Point{ x * p.x, y * p.y };
    }

    Point operator/(const Point& p) const {
        return Point{ x / p.x, y / p.y };
    }

    Point& operator=(const Point& p) = default;

    Point& operator=(const ImVec2& p) {
        x = p.x;
        y = p.y;
        return *this;
    }
};
