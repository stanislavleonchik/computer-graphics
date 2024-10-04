#ifndef COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H
#define COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H

#include "../includes.h"
using namespace std;

enum Tool {
    standby,
    bresenham,
    wu
};

bool isDrawing = false;
int startX = -1, startY = -1, endX = -1, endY = -1;
vector<pair<pair<int, int>, Tool>> lines;

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

                lines.push_back({{startX, startY}, tool});
                lines.push_back({{endX, endY}, tool});

                isDrawing = false;
            }
        } else {
            isDrawing = false;
        }
    }
}

#endif
