#ifndef COMPUTER_GRAPHICS_EDITOR_H
#define COMPUTER_GRAPHICS_EDITOR_H

#include "includes.h"

int width, height, channels;
unsigned char* image = loadImage("../input/input.png", &width, &height, &channels);

std::vector<float> hsvImage(width * height * 3);

void RGBtoHSVImageTransform() {
    for (int i = 0; i < width * height; ++i) {
        float r = image[i * channels] / 255.0f;
        float g = image[i * channels + 1] / 255.0f;
        float b = image[i * channels + 2] / 255.0f;
        float h, s, v;
        RGBtoHSV(r, g, b, h, s, v);
        hsvImage[i * 3] = h;
        hsvImage[i * 3 + 1] = s;
        hsvImage[i * 3 + 2] = v;
    }
}

float hueAdjust = 0.0f, saturationAdjust = 0.0f, brightnessAdjust = 0.0f;

void create_editor(GLuint textureID) {
    applyHSVAdjustments(image, hsvImage, width, height, channels, hueAdjust, saturationAdjust, brightnessAdjust);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoResize
                                    | ImGuiWindowFlags_NoCollapse
                                    | ImGuiWindowFlags_NoTitleBar
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus
                                    | ImGuiWindowFlags_NoScrollbar;
    ImVec2 windowSize = ImGui::GetWindowSize();

    float menuHeight = ImGui::GetFrameHeightWithSpacing();

    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(display_w, display_h - menuHeight));

    ImGui::Begin("Editor", nullptr, window_flags);

    ImVec2 imageSize(width, height);
    ImVec2 imagePos = ImGui::GetCursorScreenPos();
    ImGui::Image((void *) (intptr_t) textureID, imageSize);

    handle_mouse_click_on_image(imagePos, imageSize, width, height);

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
                fill_all(image, width, height);
                break;
        }
    }

    for (Polygon& pol: polygons) {
        if (pol.v.size() < 2)
            draw_wu_line(image, width, height, channels, pol.v[0].x, pol.v[0].y,
                         pol.v[0].x,
                         pol.v[0].y, 3);
        else {
            auto firstP = pol.v[0];
            for (int i = 1; i < pol.v.size(); ++i) {
                draw_wu_line(image, width, height, channels, firstP.x, firstP.y,
                             pol.v[i].x,
                             pol.v[i].y, 3);
                firstP = pol.v[i];
            }
            if (pol.completed) {
                draw_wu_line(image, width, height, channels, firstP.x, firstP.y,
                             pol.v[0].x,
                             pol.v[0].y, 3);
            }
        }
    }

    updateTexture(textureID, image, width, height, channels);

    ImGui::End();
}

#endif
