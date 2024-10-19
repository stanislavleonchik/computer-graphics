#ifndef COMPUTER_GRAPHICS_CREATE_TOOLS_H
#define COMPUTER_GRAPHICS_CREATE_TOOLS_H

#include "../includes.h"
#include "../lab4/message_box.h"

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
    if (ImGui::Button("Clear Polygons")) {
        isDrawing = false;
        if (!polygons.empty()) {
            polygons.clear();
        }
        cur_tool = Tool::polygon;
    }
    if (ImGui::Button("2 lines intersection")) {
        isDrawing = false;
        cur_tool = Tool::find_intersection_point;
        show_message_box = true;
    }
    if(cur_tool == Tool::find_intersection_point){
        string res = "Intersection point: (" + to_string((int)round(IntersectionPoint.x)) + ", " + to_string((int)round(IntersectionPoint.y)) + ')';
        ShowInfoWindow(res);
    }
    ImGui::SliderInt("Thickness", &cur_thickness, 1, 8);

    

    ImGui::End();
}

#endif
