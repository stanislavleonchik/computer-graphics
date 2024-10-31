#pragma once

struct FractalParams {
    int lines_per_frame;
    int iterations;
    float initial_length;
    float initial_width;
    float width_reduce;
    float length_reduce;
    ImColor begin_color;
    ImColor end_color;
    float angle_variation;
    float angle;
};