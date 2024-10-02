#ifndef COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H
#define COMPUTER_GRAPHICS_HANDLE_MOUSE_CLICK_ON_IMAGE_H

#include "../includes.h"

bool isDrawing = false;
int startX = -1, startY = -1, endX = -1, endY = -1;

void handleMouseClickOnImage(ImVec2 imagePos, ImVec2 imageSize, int width, int height) {
    if (ImGui::IsItemClicked()) {
        ImVec2 mousePos = ImGui::GetMousePos();

        float relativeX = (mousePos.x - imagePos.x) / imageSize.x;
        float relativeY = (mousePos.y - imagePos.y) / imageSize.y;

        int pixelX = static_cast<int>(relativeX * width);
        int pixelY = static_cast<int>(relativeY * height);

        if (!isDrawing) {
            startX = pixelX;
            startY = pixelY;
            isDrawing = true;
        } else {
            endX = pixelX;
            endY = pixelY;
            isDrawing = false;
        }
    }
}

#endif
