#pragma once

#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "../../models/Tool.h"
#include "../../models/Line.h"
#include "../../models/Polygon.h"
#include "../../utils/converters/update_texture.h"
#include "../../supporting_files/handle_mouse_click_on_image.h"
#include "../../utils/line_draw/draw_bresenham_line.h"
#include "../../utils/line_draw/draw_wu_line.h"
#include "../../utils/line_draw/BezierCurve.h"

namespace Editor {
    auto create_editor(
            const int DISPLAY_WIDTH,
            const int DISPLAY_HEIGHT,
            unsigned char *EDITOR_IMAGE,
            int IMAGE_EDITOR_WIDTH,
            int IMAGE_EDITOR_HEIGHT,
            int IMAGE_EDITOR_CHANNELS,
            GLuint textureID,
            std::vector<Line> &lines,
            std::vector<Polygon> &polygons,
            Tool &tool,
            bool &isDrawing,
            size_t &cur_polygon,
            int thickness,
            ImColor currentColor,
            ImVec2 &intersection_point,
            int &left_or_right,
            int &is_inside
    ) -> void {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove
                                        | ImGuiWindowFlags_NoResize
                                        | ImGuiWindowFlags_NoCollapse
                                        | ImGuiWindowFlags_NoTitleBar
                                        | ImGuiWindowFlags_NoBringToFrontOnFocus
                                        | ImGuiWindowFlags_NoScrollbar;

        float menuHeight = ImGui::GetFrameHeightWithSpacing();
        ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
        ImGui::SetNextWindowSize(
                ImVec2(static_cast<float>(DISPLAY_WIDTH), static_cast<float>(DISPLAY_HEIGHT) - menuHeight));
        ImGui::Begin("Editor", nullptr, window_flags);
        ImVec2 image_size(static_cast<float>(IMAGE_EDITOR_WIDTH), static_cast<float>(IMAGE_EDITOR_HEIGHT));
        ImVec2 image_pos = ImGui::GetCursorScreenPos();
        ImGui::Image((void *) (intptr_t) textureID, image_size);

        handle_mouse_click_on_image(
                image_pos,
                image_size,
                IMAGE_EDITOR_WIDTH,
                IMAGE_EDITOR_HEIGHT,
                tool,
                isDrawing,
                cur_polygon,
                polygons,
                lines,
                thickness,
                currentColor,
                intersection_point,
                left_or_right,
                is_inside
        );

        for (auto &line: lines) {
            switch (line.tool) {
                case Tool::bresenham: {
                    draw_bresenham_line(
                            EDITOR_IMAGE,
                            IMAGE_EDITOR_WIDTH,
                            IMAGE_EDITOR_HEIGHT,
                            IMAGE_EDITOR_CHANNELS,
                            static_cast<int>(line.x0),
                            static_cast<int>(line.y0),
                            static_cast<int>(line.x1),
                            static_cast<int>(line.y1),
                            line.thickness
                    );
                    break;
                }
                case Tool::wu: {
                    draw_wu_line(
                            EDITOR_IMAGE,
                            IMAGE_EDITOR_WIDTH,
                            IMAGE_EDITOR_HEIGHT,
                            IMAGE_EDITOR_CHANNELS,
                            static_cast<int>(line.x0),
                            static_cast<int>(line.y0),
                            static_cast<int>(line.x1),
                            static_cast<int>(line.y1)
                    );
                    break;
                }
                default: {
                }
            }
        }

        for (const Polygon &pol: polygons) {
            if (pol.v.size() < 2) {
                draw_wu_line(EDITOR_IMAGE, IMAGE_EDITOR_WIDTH, IMAGE_EDITOR_HEIGHT, IMAGE_EDITOR_CHANNELS, pol.v[0].x,
                             pol.v[0].y,
                             pol.v[0].x, pol.v[0].y);
            } else {
                auto firstP = pol.v[0];
                for (size_t i = 1; i < pol.v.size(); ++i) {
                    draw_wu_line(
                            EDITOR_IMAGE,
                            IMAGE_EDITOR_WIDTH,
                            IMAGE_EDITOR_HEIGHT,
                            IMAGE_EDITOR_CHANNELS,
                            static_cast<int>(firstP.x),
                            static_cast<int>(firstP.y),
                            static_cast<int>(pol.v[i].x),
                            static_cast<int>(pol.v[i].y)
                    );
                    firstP = pol.v[i];
                }
                if (pol.completed) {
                    draw_wu_line(
                            EDITOR_IMAGE,
                            IMAGE_EDITOR_WIDTH,
                            IMAGE_EDITOR_HEIGHT,
                            IMAGE_EDITOR_CHANNELS,
                            static_cast<int>(firstP.x),
                            static_cast<int>(firstP.y),
                            static_cast<int>(pol.v[0].x),
                            static_cast<int>(pol.v[0].y)
                    );
                }
            }
        }

        BezierCurve curve = BezierCurve();
        curve.draw_points();
        curve.draw_curves(menuHeight);

        update_texture(
                textureID,
                EDITOR_IMAGE,
                IMAGE_EDITOR_WIDTH,
                IMAGE_EDITOR_HEIGHT,
                IMAGE_EDITOR_CHANNELS
        );

        ImGui::End();
    }
}
