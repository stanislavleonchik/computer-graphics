#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>


#include "../models/MidpointDisplacementLine.h"
#include "../utils/midpoint_displacement/draw_midpoing_displacement.h"
#include <vector>
#include <deque>
using std::vector, std::deque;

int roughness = 1;

const int canvasWidth = 1024;
const int canvasHeight = 720;
static bool isInitialized = false;

GLuint canvasTexture;
vector<GLubyte> canvasData(canvasWidth * canvasHeight * 4, 255);
deque<MidpointDisplacementLine> midpoint_lines;

void InitCanvasTexture(GLuint& texture, std::vector<GLubyte>& data,int width, int height) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void ClearCanvas() {
    fill(canvasData.begin(), canvasData.end(), 255);
    glBindTexture(GL_TEXTURE_2D, canvasTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, canvasData.data());
}

void create_midpoint_displacement_tools() {
    ImGui::Begin("Midpoint Displacement Tools");

    if (!isInitialized) {
        InitCanvasTexture(canvasTexture, canvasData, canvasWidth, canvasHeight);
        isInitialized = true;
    }

    ImVec2 canvasSize = ImVec2(canvasWidth, canvasHeight);
    ImGui::Image((void*)(intptr_t)canvasTexture, canvasSize);


    ImGui::SliderInt("Roughness", &roughness, 1, 5);

    if (ImGui::Button("New iteration")) {
        ClearCanvas();
        draw_midpoint_displacement(midpoint_lines, roughness, canvasWidth, canvasHeight, canvasData, canvasTexture);

    }
    if (ImGui::Button("Clear")) {
        ClearCanvas();
        midpoint_lines.clear();
    }
    glBindTexture(GL_TEXTURE_2D, canvasTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, canvasData.data());

    ImGui::End();
}
