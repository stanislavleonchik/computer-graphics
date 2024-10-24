
export module find_intersection;

import Line;
#include "imgui.h"

export ImVec2 find_intersection(Line l1, Line l2) {

    ImVec2 a { l1.x0, l1.y0 };
    ImVec2 b { l1.x1, l1.y1 };
    ImVec2 c { l2.x0, l2.y0 };
    ImVec2 d { l2.x1, l2.y1 };
    ImVec2 normal = { d.y - c.y, c.x - d.x };

    // ������ ba = b - a
    ImVec2 ba = { b.x - a.x, b.y - a.y };

    // ������ ac = a - c
    ImVec2 ac = { a.x - c.x, a.y - c.y };

    // ���������� ��������� ������������
    float numerator = -(normal.x * ac.x + normal.y * ac.y);
    float denominator = normal.x * ba.x + normal.y * ba.y;

    if (denominator != 0.0f) {

        float t = numerator / denominator;
        ImVec2 intersection = {
                a.x + t * (b.x - a.x),
                a.y + t * (b.y - a.y)
        };
        return intersection;
    }
    else
        return {};
}
