#ifndef COMPUTER_GRAPHICS_CREATE_FRACTAL_TOOL_H
#define COMPUTER_GRAPHICS_CREATE_FRACTAL_TOOL_H

#include "imgui.h"
#include "../../src/models/Color.h"

void create_fractal_tool(
        int& lines_per_frame,
        int& iterations,
        int& initial_width,
        int& width_reduce,
        int& length_reduce,
        Color& begin_color,
        Color& end_color
        ) {
    ImGui::Begin("Draw Fractal");

    ImGui::SliderInt("Lines Per Frame", &lines_per_frame, 1, 20);
    ImGui::SliderInt("Iterations", &iterations, 1, 20);
    ImGui::SliderInt("Initial Width", &initial_width, 1, 20);
    ImGui::SliderInt("Width Reduce", &width_reduce, 1, 20);
    ImGui::SliderInt("Length Reduce", &length_reduce, 1, 20);

    static float beg_clr[3] = {0.0f, 0.0f, 0.0f};
    if (ImGui::ColorEdit3("Begin", beg_clr)) {
        begin_color.r = static_cast<unsigned char>(beg_clr[0] * 255);
        begin_color.g = static_cast<unsigned char>(beg_clr[1] * 255);
        begin_color.b = static_cast<unsigned char>(beg_clr[2] * 255);
    }

    static float end_clr[3] = {0.0f, 0.0f, 0.0f};
    if (ImGui::ColorEdit3("Begin", end_clr)) {
        end_color.r = static_cast<unsigned char>(end_clr[0] * 255);
        end_color.g = static_cast<unsigned char>(end_clr[1] * 255);
        end_color.b = static_cast<unsigned char>(end_clr[2] * 255);
    }
    if (ImGui::ColorPicker3("Begin", end_clr)) {
        end_color.r = static_cast<unsigned char>(end_clr[0] * 255);
        end_color.g = static_cast<unsigned char>(end_clr[1] * 255);
        end_color.b = static_cast<unsigned char>(end_clr[2] * 255);
    }

    ImGui::End();
}

#endif
