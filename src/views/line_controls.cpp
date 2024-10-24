
#include "imgui.h"

export module create_line_controls;

export void create_line_controls(int& x0, int& y0, int& x1, int& y1, int width, int height) {
    ImGui::Begin("Line Controls");

    ImGui::SliderInt("X0", &x0, 0, width - 1);
    ImGui::SliderInt("Y0", &y0, 0, height - 1);
    ImGui::SliderInt("X1", &x1, 0, width - 1);
    ImGui::SliderInt("Y1", &y1, 0, height - 1);

    ImGui::End();
}
