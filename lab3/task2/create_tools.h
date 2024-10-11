#ifndef COMPUTER_GRAPHICS_CREATE_TOOLS_H
#define COMPUTER_GRAPHICS_CREATE_TOOLS_H

#include "../includes.h"

void create_tools(Tool& cur_tool, int& cur_thickness) {
    ImGui::Begin("Tools");

    if (ImGui::Button("Spectate")) {
        isDrawing = false;
        cur_tool = Tool::standby;
    }
    if (ImGui::Button("Bresenham")) {
        isDrawing = false;
        cur_tool = Tool::bresenham;
    }
    if (ImGui::Button("Wu")) {
        isDrawing = false;
        cur_tool = Tool::wu;
    }
    if (ImGui::Button("Polygon")) {
        isDrawing = false;
        if (!polygons.empty()) {
            polygons[polygons.size() - 1].completed = true;
        }
        cur_tool = Tool::polygon;
    }
    ImGui::SliderInt("Thickness", &cur_thickness, 1, 8);

    ImGui::End();
}

#endif
