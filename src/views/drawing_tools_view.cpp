
#include "imgui.h"
#include "GLFW/glfw3.h"

export module drawing_tools_view;

import Tool;
import Color;
import Polygon;

using std::vector;

export auto create_drawing_tools_view(
        Tool& cur_tool,
        int& cur_thickness,
        bool& is_drawing,
        Color& current_color,

        vector<Polygon> polygons,
        ImVec2& intersection_point
        ) -> void {

    bool show_message_box = false;

    int left_or_right;
    int is_inside;

    ImGui::Begin("Tools");
    if (ImGui::Button("Spectate")) {
        is_drawing = false;
        cur_tool = Tool::standby;
    }

    // Палитра
    static float color[3] = {0.0f, 0.0f, 0.0f};
    if (ImGui::ColorEdit3("Color", color)) {
        current_color.r = static_cast<unsigned char>(color[0] * 255);
        current_color.g = static_cast<unsigned char>(color[1] * 255);
        current_color.b = static_cast<unsigned char>(color[2] * 255);
    }

    if (ImGui::Button("Bresenham")) {
        is_drawing = false;
        cur_tool = Tool::bresenham;
    }
    if (ImGui::Button("Wu")) {
        is_drawing = false;
        cur_tool = Tool::wu;
    }
    if (ImGui::Button("Check point to edge position")) {
        left_or_right = 2;
        is_drawing = false;
        cur_tool = Tool::point_orientation_to_edge_check;
    }
    if (cur_tool == Tool::point_orientation_to_edge_check) {
        ImGui::BeginDisabled();
        ImGui::RadioButton("Left", &left_or_right, 0); // Если selected == 0, будет выбрана эта кнопка
        ImGui::RadioButton("Right", &left_or_right, 1); // Если selected == 1, будет выбрана эта кнопка
        ImGui::EndDisabled();

    }
    if (ImGui::Button("Check point inside polygon")) {
        is_inside = 0;
        is_drawing = false;
        cur_tool = Tool::point_inside_polygon;
    }
    if (cur_tool == Tool::point_inside_polygon) {
        ImGui::BeginDisabled();
        ImGui::RadioButton("Inside", &is_inside, 1); // Если selected == 0, будет выбрана эта кнопка
        ImGui::EndDisabled();
    }
    if (ImGui::Button("Polygon")) {
        is_drawing = false;
        if (!polygons.empty()) {
            polygons[polygons.size() - 1].completed = true;
        }
        cur_tool = Tool::polygon;
    }
    if (ImGui::Button("Clear Polygons")) {
        is_drawing = false;
        if (!polygons.empty()) {
            polygons.clear();
        }
        cur_tool = Tool::polygon;
    }
    if (ImGui::Button("2 lines intersection")) {
        is_drawing = false;
        cur_tool = Tool::find_intersection_point;
        show_message_box = true;
    }
    if(cur_tool == Tool::find_intersection_point){
        std::string res = "Intersection point: (" + std::to_string((int)round(intersection_point.x)) + ", " + std::to_string((int)round(intersection_point.y)) + ')';
        if (show_message_box) {
            ImGui::Begin("Information", &show_message_box, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("%s", res.c_str());

            if (ImGui::Button("OK")) {
                show_message_box = false;
            }

            ImGui::End();
        }
    }
    ImGui::SliderInt("Thickness", &cur_thickness, 1, 8);

    if (ImGui::Button("Fill")) {
        is_drawing = false;
        cur_tool = Tool::fill;
    }

    ImGui::End();
}
