#pragma once
#include "imgui.h"
#include "GLFW/glfw3.h"
#include "../models/Polygon.h"
#include "../models/Tool.h"
#include <vector>

using std::vector;

bool NSButton(const char* label, const ImVec2& size_arg = ImVec2(200, 30)) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = size_arg;
    if (size.x == 0) size.x = ImGui::CalcTextSize(label).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    if (size.y == 0) size.y = ImGui::GetFrameHeight();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec4 shadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.15f);
    draw_list->AddRectFilled(ImVec2(pos.x + 2, pos.y + 2), ImVec2(pos.x + size.x + 2, pos.y + size.y + 2),
                             ImColor(shadowColor), ImGui::GetStyle().FrameRounding);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

    bool pressed = ImGui::Button(label, size);
    ImGui::PopStyleColor(4);

    return pressed;
}

auto create_drawing_tools_view(
        Tool& cur_tool,
        int& cur_thickness,
        bool& is_drawing,
        ImColor& current_color,

        vector<Polygon>& polygons,
        ImVec2& intersection_point
        ) -> void {

    bool show_message_box = false;

    int left_or_right;
    int is_inside;

    ImGui::Begin("Tools");
    if (NSButton("Spectate")) {
        is_drawing = false;
        cur_tool = Tool::standby;
    }
    if (NSButton("Bresenham")) {
        is_drawing = false;
        cur_tool = Tool::bresenham;
    }
    if (NSButton("Wu")) {
        is_drawing = false;
        cur_tool = Tool::wu;
    }
    if (NSButton("Check point to edge position")) {
        left_or_right = 2;
        is_drawing = false;
        cur_tool = Tool::point_orientation_to_edge_check;
    }
    if (cur_tool == Tool::point_orientation_to_edge_check) {
        ImGui::BeginDisabled();
        ImGui::RadioButton("Left", &left_or_right, 0);
        ImGui::RadioButton("Right", &left_or_right, 1);
        ImGui::EndDisabled();

    }
    if (NSButton("Check point inside polygon")) {
        is_inside = 0;
        is_drawing = false;
        cur_tool = Tool::point_inside_polygon;
    }
    if (cur_tool == Tool::point_inside_polygon) {
        ImGui::BeginDisabled();
        ImGui::RadioButton("Inside", &is_inside, 1);
        ImGui::EndDisabled();
    }
    if (NSButton("Draw polygon")) {
        is_drawing = false;
        if (!polygons.empty()) {
            polygons[polygons.size() - 1].completed = true;
        }
        cur_tool = Tool::draw_polygon;
    }
    if (NSButton("Clear Polygons")) {
        is_drawing = false;
        if (!polygons.empty()) {
            polygons.clear();
        }
        cur_tool = Tool::draw_polygon;
    }
    if (NSButton("2 lines intersection")) {
        is_drawing = false;
        cur_tool = Tool::find_intersection_point;
        show_message_box = true;
    }
    if(cur_tool == Tool::find_intersection_point){
        std::string res = "Intersection point: (" + std::to_string((int)round(intersection_point.x)) + ", " + std::to_string((int)round(intersection_point.y)) + ')';
        if (show_message_box) {
            ImGui::Begin("Information", &show_message_box, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("%s", res.c_str());

            if (NSButton("OK")) {
                show_message_box = false;
            }

            ImGui::End();
        }
    }

    ImGui::End();
}
