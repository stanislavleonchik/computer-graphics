#include "imgui.h"

void create_hsv_sliders(float& hue, float& saturation, float& brightness) {
    ImGui::Begin("HSV Controls");

    ImGui::SliderFloat("Hue", &hue, 0.0f, 360.0f);
    ImGui::SliderFloat("Saturation", &saturation, 0.0f, 1.0f);
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f);

    ImGui::End();
}
