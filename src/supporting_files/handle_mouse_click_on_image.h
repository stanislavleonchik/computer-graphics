#include <iostream>

#include "../models/Point.h"
#include "../models/Line.h"
#include "../models/Tool.h"
#include "../models/Polygon.h"

#include "../utils/polygon_transform/check_point_position_relative_edge.h"
#include "../utils/polygon_transform/find_intersection.h"
#include "../utils/polygon_transform/is_point_inside_polygon.h"
#include "../utils/fractals/fractal_utils.h"

#include "imgui.h"


using std::vector;

auto handle_mouse_click_on_image(
        ImVec2 image_pos,
        ImVec2 image_size,
        int IMAGE_EDITOR_WIDTH,
        int IMAGE_EDITOR_HEIGHT,
        Tool tool,
        bool& isDrawing,
        size_t& cur_polygon,
        vector<Polygon>& polygons,
        vector<Line>& lines,
        int thickness,
        ImColor currentColor,
        ImVec2& intersection_point,
        int& left_or_right,
        int& is_inside
) -> void {
    int start_x = -1,
            start_y = -1,
            end_x = -1,
            end_y = -1;
    if (ImGui::IsItemClicked()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 adjustedImagePos = ImVec2(image_pos.x, image_pos.y);
        if (tool == Tool::standby) return;
        float relativeX = (mousePos.x - adjustedImagePos.x) / image_size.x;
        float relativeY = (mousePos.y - adjustedImagePos.y) / image_size.y;
        if (relativeX >= 0.0f && relativeX <= 1.0f && relativeY >= 0.0f && relativeY <= 1.0f) {
            float pixel_x = relativeX * static_cast<float>(IMAGE_EDITOR_WIDTH);
            float pixel_y = relativeY * static_cast<float>(IMAGE_EDITOR_HEIGHT);
            switch (tool) {
                case Tool::draw_polygon: {
                    if (!isDrawing) {
                        isDrawing = true;
                        cur_polygon = polygons.size();
                        polygons.emplace_back(
                                vector<Point>{{static_cast<float>(pixel_x), static_cast<float>(pixel_y)}},
                                false,
                                thickness,
                                currentColor
                        );
                    } else {
                        if (std::abs(pixel_x - polygons[cur_polygon].v[0].x) < 10 &&
                            std::abs(pixel_y - polygons[cur_polygon].v[0].y) < 10) {
                            isDrawing = false;
                            polygons[cur_polygon].completed = true;
                            polygons[cur_polygon].v.push_back(polygons[cur_polygon].v[0]);
                        } else
                            polygons[cur_polygon].v.emplace_back(pixel_x, pixel_y);
                    }
                    break;
                }
                case Tool::find_intersection_point: {
                    if (!isDrawing) {
                        start_x = static_cast<int>(pixel_x);
                        start_y = static_cast<int>(pixel_y);
                        isDrawing = true;
                    } else {
                        end_x = static_cast<int>(pixel_x);
                        end_y = static_cast<int>(pixel_y);
                        lines.emplace_back(static_cast<float>(start_x), static_cast<float>(start_y),
                                           static_cast<float>(end_x), static_cast<float>(end_y),
                                           Tool::wu, thickness);
                        if (lines.size() >= 2)
                            intersection_point = find_intersection(lines[lines.size() - 1], lines[lines.size() - 2]);
                        isDrawing = false;
                    }
                    break;
                }
                case Tool::point_orientation_to_edge_check: {
                    if (!lines.empty()) {
                        left_or_right = static_cast<int>(check_point_position_relative_edge(lines.back(), pixel_x, pixel_y));
                    }
                    break;
                }
                case Tool::point_inside_polygon: {
                    if (!polygons.empty() && polygons.back().completed) {
                        is_inside = static_cast<int>(is_point_inside_polygon(Point(pixel_x, pixel_y), polygons.back()));
                    }
                    break;
                }
                default: {
                    if (!isDrawing) {
                        start_x = static_cast<int>(pixel_x);
                        start_y = static_cast<int>(pixel_y);
                        isDrawing = true;
                    } else {
                        end_x = static_cast<int>(pixel_x);
                        end_y = static_cast<int>(pixel_y);
                        lines.emplace_back(static_cast<float>(start_x), static_cast<float>(start_y),
                                           static_cast<float>(end_x), static_cast<float>(end_y),
                                           tool, thickness, currentColor);
                        isDrawing = false;
                    }
                }
            }
        }
    }
}
