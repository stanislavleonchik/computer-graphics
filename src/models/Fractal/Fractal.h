#pragma once
#include <utility>
#include <vector>
#include <imgui.h>
#include "FractalVertex.h"
#include "FractalParams.h"
#include "LSystem.h"

struct Fractal {
    std::vector<std::vector<FractalVertex>> verts = std::vector<std::vector<FractalVertex>>();
    LSystem lsystem;
    FractalParams fractal_params;
    ImVec2 start_pos = {0.5f, 0.750f};
};
