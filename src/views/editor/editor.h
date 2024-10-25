#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>

#include "../../models/Tool.h"
#include "../../models/Line.h"
#include "../../models/Polygon.h"
#include "../../models/Color.h"

#include "../../utils/converters/apply_hsv_adjustments.h"
#include "../../utils/converters/update_texture.h"
#include "../../supporting_files/handle_mouse_click_on_image.h"

#include "../../utils/line_draw/draw_bresenham_line.h"
#include "../../utils/line_draw/draw_wu_line.h"
#include "../../utils/line_draw/fill_all.h"

using std::vector;


auto create_editor(
        const int DISPLAY_WIDTH,
        const int DISPLAY_HEIGHT,

        unsigned char* EDITOR_IMAGE,
        int IMAGE_EDITOR_WIDTH,
        int IMAGE_EDITOR_HEIGHT,
        int IMAGE_EDITOR_CHANNELS,

        GLuint textureID,
        vector<float>& hsv_image,
        float& hue_adjust,
        float& saturation_adjust,
        float& brightness_adjust,

        vector<Line>& lines,
        vector<Polygon>& polygons,

        Tool& tool,
        bool& isDrawing,
        size_t& cur_polygon,
        int thickness,
        Color currentColor,
        ImVec2& intersection_point,
        int& left_or_right,
        int& is_inside
        ) -> void {

    apply_hsv_adjustments(
            EDITOR_IMAGE,
            hsv_image,
            IMAGE_EDITOR_WIDTH,
            IMAGE_EDITOR_HEIGHT,
            IMAGE_EDITOR_CHANNELS,
            hue_adjust,
            saturation_adjust,
            brightness_adjust
            );

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoResize
                                    | ImGuiWindowFlags_NoCollapse
                                    | ImGuiWindowFlags_NoTitleBar
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus
                                    | ImGuiWindowFlags_NoScrollbar;

    float menuHeight = ImGui::GetFrameHeightWithSpacing();

    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2((float)DISPLAY_WIDTH, (float)DISPLAY_HEIGHT - menuHeight));

    ImGui::Begin("Editor", nullptr, window_flags);

    static float zoomLevel = 1.0f;
    static ImVec2 offset(0.0f, 0.0f);

    ImGuiIO& io = ImGui::GetIO();

    // Обработка зума
    if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f) {
        zoomLevel += io.MouseWheel * 0.1f;
        zoomLevel = std::clamp(zoomLevel, 0.1f, 10.0f);
        ImGui::SetScrollY(0);
    }

    // Обработка панорамирования
    if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        offset = ImVec2(io.MouseDelta.x + offset.x, io.MouseDelta.y + offset.y);
    }

    ImVec2 image_size((float)IMAGE_EDITOR_WIDTH * zoomLevel, (float)IMAGE_EDITOR_HEIGHT * zoomLevel);
    ImVec2 image_pos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(ImVec2(image_pos.x + offset.x, image_pos.y + offset.y));

    ImGui::Image((void*)(intptr_t)textureID, image_size);

    // Возвращаем курсор на исходную позицию
    ImGui::SetCursorScreenPos(image_pos);

    handle_mouse_click_on_image(
            image_pos, // ImVec2
            image_size, // ImVec2
            IMAGE_EDITOR_WIDTH, // int
            IMAGE_EDITOR_HEIGHT, // int
            zoomLevel, // float
            offset, // ImVec2
            tool, // Tool
            isDrawing, // bool&
            cur_polygon, // size_t&
            polygons, // vector<Polygon>&
            lines, // vector<Line>&
            thickness, // int
            currentColor, // Color
            intersection_point, // ImVec2&
            left_or_right, // bool&
            is_inside // bool&
            );

    for (auto& line : lines) {
        switch (line.tool) {
            case Tool::bresenham:
                draw_bresenham_line(
                        EDITOR_IMAGE,
                        IMAGE_EDITOR_WIDTH,
                        IMAGE_EDITOR_HEIGHT,
                        IMAGE_EDITOR_CHANNELS,
                        (int)line.x0,
                        (int)line.y0,
                        (int)line.x1,
                        (int)line.y1,
                        line.thickness
                        );
                break;
            case Tool::wu:
                draw_wu_line(
                        EDITOR_IMAGE,
                        IMAGE_EDITOR_WIDTH,
                        IMAGE_EDITOR_HEIGHT,
                        IMAGE_EDITOR_CHANNELS,
                        (int)line.x0,
                        (int)line.y0,
                         (int)line.x1,
                         (int)line.y1,
                         line.thickness
                         );
                break;
            case Tool::fill:
                fill_all(
                        EDITOR_IMAGE,
                        IMAGE_EDITOR_WIDTH,
                        IMAGE_EDITOR_HEIGHT,
                        IMAGE_EDITOR_CHANNELS,
                        currentColor
                        );
                break;
        }
    }

    for (Polygon& pol : polygons) {
        if (pol.v.size() < 2) {
            draw_line_gupta_sproull(EDITOR_IMAGE, IMAGE_EDITOR_WIDTH, IMAGE_EDITOR_HEIGHT, IMAGE_EDITOR_CHANNELS, pol.v[0].x, pol.v[0].y,
                                    pol.v[0].x, pol.v[0].y, pol.thickness,
                                    pol.color.r, pol.color.g, pol.color.b);
        } else {
            auto firstP = pol.v[0];
            for (size_t i = 1; i < pol.v.size(); ++i) {
                draw_line_gupta_sproull(
                        EDITOR_IMAGE,
                        IMAGE_EDITOR_WIDTH,
                        IMAGE_EDITOR_HEIGHT,
                        IMAGE_EDITOR_CHANNELS,
                        static_cast<int>(firstP.x),
                        static_cast<int>(firstP.y),
                        static_cast<int>(pol.v[i].x),
                        static_cast<int>(pol.v[i].y),
                        pol.thickness,
                        pol.color.r,
                        pol.color.g,
                        pol.color.b
                );
                firstP = pol.v[i];
            }
            if (pol.completed) {
                draw_line_gupta_sproull(
                        EDITOR_IMAGE,
                        IMAGE_EDITOR_WIDTH,
                        IMAGE_EDITOR_HEIGHT,
                        IMAGE_EDITOR_CHANNELS,
                        static_cast<int>(firstP.x),
                        static_cast<int>(firstP.y),
                        static_cast<int>(pol.v[0].x),
                        static_cast<int>(pol.v[0].y),
                        pol.thickness,
                        pol.color.r,
                        pol.color.g,
                        pol.color.b
                );
            }
        }
    }

    update_texture(
            textureID,
            EDITOR_IMAGE,
            IMAGE_EDITOR_WIDTH,
            IMAGE_EDITOR_HEIGHT,
            IMAGE_EDITOR_CHANNELS);

    ImGui::End();
}
