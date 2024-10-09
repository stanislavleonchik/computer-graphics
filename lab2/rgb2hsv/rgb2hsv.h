#include "../../includes.h"

void updateTexture(GLuint textureID, unsigned char* image, int width, int height, int channels) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void applyHSVAdjustments(unsigned char* image, std::vector<float>& hsvImage, int width, int height, int channels, float hueAdjust, float saturationAdjust, float brightnessAdjust) {
    for (int i = 0; i < width * height; ++i) {
        float h = hsvImage[i * 3] + hueAdjust;
        float s = std::clamp(hsvImage[i * 3 + 1] + saturationAdjust, 0.0f, 1.0f);
        float v = std::clamp(hsvImage[i * 3 + 2] + brightnessAdjust, 0.0f, 1.0f);

        float r, g, b;
        HSVtoRGB(h, s, v, r, g, b);

        image[i * channels] = static_cast<unsigned char>(r * 255);
        image[i * channels + 1] = static_cast<unsigned char>(g * 255);
        image[i * channels + 2] = static_cast<unsigned char>(b * 255);
    }
}

GLuint createTextureFromImage(unsigned char* image, int width, int height, int channels) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);


    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void setupImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void renderImGui(GLuint textureID, int imageWidth, int imageHeight) {
    ImGui::Begin("Editor");

    ImVec2 imageSize(imageWidth, imageHeight);
    ImGui::Image((void*)(intptr_t)textureID, imageSize);

    ImGui::End();
}

void cleanupImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
