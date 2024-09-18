#ifndef COMPUTER_GRAPHICS_CREATE_SLIDERS_H
#define COMPUTER_GRAPHICS_CREATE_SLIDERS_H

#include "imgui.h"

void createSliders(float& hue, float& saturation, float& brightness) {
    ImGui::Begin("HSV Controls");

    ImGui::SliderFloat("Hue", &hue, 0.0f, 360.0f);
    ImGui::SliderFloat("Saturation", &saturation, 0.0f, 1.0f);
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f);

    ImGui::End();
}

#endif //COMPUTER_GRAPHICS_CREATE_SLIDERS_H
