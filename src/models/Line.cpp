
export module Line;

import Tool;
import Color;

export struct Line {
    float x0{};
    float y0{};
    float x1{};
    float y1{};
    Tool tool = Tool::wu;
    int thickness = 3;
    Color color = {0, 0, 0};
};
