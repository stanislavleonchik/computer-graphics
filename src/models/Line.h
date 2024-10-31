#pragma once

#include "Tool.h"

struct Line {
    float x0{};
    float y0{};
    float x1{};
    float y1{};
    Tool tool = Tool::wu;
    int thickness = 3;
    ImColor color = {0, 0, 0};
};
