#ifndef COMPUTER_GRAPHICS_EDITOR_H
#define COMPUTER_GRAPHICS_EDITOR_H

#include "../../includes.h"

int width, height, channels;
unsigned char* image = loadImage("../assets/blank.png", &width, &height, &channels);

std::vector<float> hsvImage(width * height * 3);

void RGBtoHSVImageTransform() {
    for (int i = 0; i < width * height; ++i) {
        float r = image[i * channels] / 255.0f;
        float g = image[i * channels + 1] / 255.0f;
        float b = image[i * channels + 2] / 255.0f;
        float h, s, v;
        rgb_to_hsv(r, g, b, h, s, v);
        hsvImage[i * 3] = h;
        hsvImage[i * 3 + 1] = s;
        hsvImage[i * 3 + 2] = v;
    }
}

float hueAdjust = 0.0f, saturationAdjust = 0.0f, brightnessAdjust = 0.0f;

void create_editor(GLuint textureID) {
    apply_hsv_adjustments(image, hsvImage, width, height, channels, hueAdjust, saturationAdjust, brightnessAdjust);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoResize
                                    | ImGuiWindowFlags_NoCollapse
                                    | ImGuiWindowFlags_NoTitleBar
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus
                                    | ImGuiWindowFlags_NoScrollbar;

    float menuHeight = ImGui::GetFrameHeightWithSpacing();

    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(display_w, display_h - menuHeight));

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

    ImVec2 imageSize(width * zoomLevel, height * zoomLevel);
    ImVec2 imagePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(ImVec2(imagePos.x + offset.x, imagePos.y + offset.y));

    ImGui::Image((void*)(intptr_t)textureID, imageSize);

    // Возвращаем курсор на исходную позицию
    ImGui::SetCursorScreenPos(imagePos);

    handle_mouse_click_on_image(imagePos, imageSize, width, height, zoomLevel, offset);

    for (size_t i = 0; i < lines.size(); ++i) {
        switch (lines[i].tool) {
            case Tool::bresenham:
                draw_bresenham_line(image, width, height, channels, lines[i].x0, lines[i].y0,
                                    lines[i].x1,
                                    lines[i].y1, lines[i].thickness);
                break;
            case Tool::wu:
                draw_wu_line(image, width, height, channels, lines[i].x0, lines[i].y0,
                             lines[i].x1,
                             lines[i].y1, lines[i].thickness);
                break;
            case Tool::fill:
                fill_all(image, width, height, channels, currentColor);
                break;
        }
    }

    for (Polygon& pol : polygons) {
        if (pol.v.size() < 2) {
            draw_line_gupta_sproull(image, width, height, channels, pol.v[0].x, pol.v[0].y,
                            pol.v[0].x, pol.v[0].y, pol.thickness,
                            pol.color.r, pol.color.g, pol.color.b);
        } else {
            auto firstP = pol.v[0];
            for (size_t i = 1; i < pol.v.size(); ++i) {
                draw_line_gupta_sproull(
                        image,
                        width,
                        height,
                        channels,
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
                        image,
                        width,
                        height,
                        channels,
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

    curve.draw_points();
    curve.draw_curves(menuHeight);

    update_texture(textureID, image, width, height, channels);

    ImGui::End();
}

#endif
