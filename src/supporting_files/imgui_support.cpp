
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

export module imgui_support;

export void render_imgui(GLuint textureID, int image_width, int image_height) {
    ImGui::Begin("Editor");

    ImVec2 imageSize(image_width, image_height);
    ImGui::Image((void*)(intptr_t)textureID, imageSize);

    ImGui::End();
}

export void cleanup_imgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
