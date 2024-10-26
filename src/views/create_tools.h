#ifndef COMPUTER_GRAPHICS_CREATE_TOOLS_H
#define COMPUTER_GRAPHICS_CREATE_TOOLS_H

#include "../includes.h"

void create_tools(Tool& cur_tool, int& cur_thickness) {
    extern int leftOrRight;
    extern int isInside;
    ImGui::Begin("Tools");

    
    if (ImGui::Button("Spectate")) {
        isDrawing = false;
        cur_tool = Tool::standby;
    }
    static float color[3] = {0.0f, 0.0f, 0.0f};

    // Палитра
    if (ImGui::ColorEdit3("Color", color)) {
        currentColor.r = static_cast<unsigned char>(color[0] * 255);
        currentColor.g = static_cast<unsigned char>(color[1] * 255);
        currentColor.b = static_cast<unsigned char>(color[2] * 255);
    }

    if (ImGui::Button("Bresenham")) {
        isDrawing = false;
        cur_tool = Tool::bresenham;
    }
    if (ImGui::Button("Wu")) {
        isDrawing = false;
        cur_tool = Tool::wu;
    }
    if (ImGui::Button("Check point to edge position")) {
        leftOrRight = 2;
        isDrawing = false;
        cur_tool = Tool::point_orientation_to_edge_check;
    }
    if (cur_tool == Tool::point_orientation_to_edge_check) {
        ImGui::BeginDisabled();
        ImGui::RadioButton("Left", &leftOrRight, 0); // Если selected == 0, будет выбрана эта кнопка
        ImGui::RadioButton("Right", &leftOrRight, 1); // Если selected == 1, будет выбрана эта кнопка
        ImGui::EndDisabled();

    }
    if (ImGui::Button("Check point inside polygon")) {
        isInside = 0;
        isDrawing = false;
        cur_tool = Tool::point_inside_polygon;
    }
    if (cur_tool == Tool::point_inside_polygon) {
        ImGui::BeginDisabled();
        ImGui::RadioButton("Inside", &isInside, 1); // Если selected == 0, будет выбрана эта кнопка
        ImGui::EndDisabled();
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
    if (ImGui::Button("Bezier Curve")) {
        if (cur_tool == Tool::B_curve)
            curve.clear();
        isDrawing = true;
        cur_tool = Tool::B_curve;
    }
    ImGui::SliderInt("Thickness", &cur_thickness, 1, 8);

    if (ImGui::Button("Fill")) {
        isDrawing = false;
        cur_tool = Tool::fill;
    }

    ImGui::End();
}

#endif
