#pragma once
#include <vector>

#include "imgui.h"

#include "../../models/Fractal/Fractal.h"

void fractal_tools(Fractal& fractal) {
    ImGui::Begin("Fractal Tools");
    static int current_fractal = 0;
    std::vector<LSystem> l_systems = get_predefined_l_systems();
    std::vector<const char*> fractal_names;
    for (const auto& lsys : l_systems) {
        fractal_names.push_back(lsys.name.c_str());
    }
    if (ImGui::Combo("Fractal Type", &current_fractal, fractal_names.data(), fractal_names.size())) {
        fractal.fractal_params = l_systems[current_fractal].default_params;
        fractal.lsystem = l_systems[current_fractal];
    }
    ImGui::SliderFloat("X Position", &fractal.start_pos.x, 0.0f, 1.0f);
    ImGui::SliderFloat("Y Position", &fractal.start_pos.y, 0.0f, 10.0f);
//    ImGui::SliderInt("Lines Per Frame", &fractal.fractal_params.lines_per_frame, 1, 20);
    ImGui::SliderInt("Iterations", &fractal.fractal_params.iterations, 1, 20);
    ImGui::SliderFloat("Initial Length", &fractal.fractal_params.initial_length, 0.0f, 40.0f);
    ImGui::SliderFloat("Initial Width", &fractal.fractal_params.initial_width, 0.0f, 10.0f);
    ImGui::SliderFloat("Width Reduce", &fractal.fractal_params.width_reduce, 0.950f, 1.050f);
    ImGui::SliderFloat("Length Reduce", &fractal.fractal_params.length_reduce, 0.950f, 1.050f);
    ImGui::SliderFloat("Angle", &fractal.fractal_params.angle, -180.0f, 180.0f);
    ImGui::SliderFloat("Angle Variation", &fractal.fractal_params.angle_variation, 0.0f, fractal.lsystem.angle / 2.0f);

    // Begin Color Picker
    float begin_color[4] = { fractal.fractal_params.begin_color.Value.x, fractal.fractal_params.begin_color.Value.y, fractal.fractal_params.begin_color.Value.z, fractal.fractal_params.begin_color.Value.w };
    if (ImGui::ColorPicker4("Begin Color", begin_color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueWheel)) {
        fractal.fractal_params.begin_color = ImColor(begin_color[0], begin_color[1], begin_color[2], begin_color[3]);
    }

    // End Color Picker
    float end_color[4] = { fractal.fractal_params.end_color.Value.x, fractal.fractal_params.end_color.Value.y, fractal.fractal_params.end_color.Value.z, fractal.fractal_params.end_color.Value.w };
    if (ImGui::ColorPicker4("End Color", end_color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueWheel)) {
        fractal.fractal_params.end_color = ImColor(end_color[0], end_color[1], end_color[2], end_color[3]);
    }

    if (ImGui::Button("Create Fractal")) {


    }

    ImGui::End();
}
