
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

void render_imgui(GLuint textureID, int image_width, int image_height) {
    ImGui::Begin("Editor");

    ImVec2 imageSize((float)image_width, (float)image_height);
    ImGui::Image((void*)(intptr_t)textureID, imageSize);

    ImGui::End();
}

void cleanup_imgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
