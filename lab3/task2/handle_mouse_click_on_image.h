#ifndef COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H
#define COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H

#include "../includes.h"
using namespace std;

enum Tool {
    standby,
    bresenham,
    wu
};

struct line {
    int x0;
    int y0;
    int x1;
    int y1;
    Tool tool;
    int thickness;
};

bool isDrawing = false;
int startX = -1, startY = -1, endX = -1, endY = -1;
vector<line> lines;

auto tool = Tool::standby;
int thickness = 1;

void handle_mouse_click_on_image(ImVec2 imagePos, ImVec2 imageSize, int width, int height) {
    if (ImGui::IsItemClicked()) {
        ImVec2 mousePos = ImGui::GetMousePos();

        float relativeX = (mousePos.x - imagePos.x) / imageSize.x;
        float relativeY = (mousePos.y - imagePos.y) / imageSize.y;

        int pixelX = static_cast<int>(relativeX * width);
        int pixelY = static_cast<int>(relativeY * height);

        if (tool != Tool::standby) {
            if (!isDrawing) {
                startX = pixelX;
                startY = pixelY;
                isDrawing = true;
            } else {
                endX = pixelX;
                endY = pixelY;

                lines.emplace_back(startX, startY, endX, endY, tool, thickness);

                isDrawing = false;
            }
        } else {
            isDrawing = false;
        }
    }
}

#endif
