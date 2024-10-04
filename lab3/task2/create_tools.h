#ifndef COMPUTER_GRAPHICS_CREATE_TOOLS_H
#define COMPUTER_GRAPHICS_CREATE_TOOLS_H

#include "../includes.h"

void create_tools(Tool& cur_tool, int& cur_thickness) {
    ImGui::Begin("Tools");

    if (ImGui::Button("Spectate")) {
        cur_tool = Tool::standby;
    }
    if (ImGui::Button("Bresenham")) {
        cur_tool = Tool::bresenham;
    }
    if (ImGui::Button("Wu")) {
        cur_tool = Tool::wu;
    }
    ImGui::SliderInt("Thickness", &cur_thickness, 1, 8);

    ImGui::End();
}

#endif
