#pragma once
#include <imgui.h>

struct FractalVertex {
    ImVec2 position;
    ImColor im_color;
    int thickness{};
    int depth{};
};
