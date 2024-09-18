#include "../includes.h"
#include "load_image.h"
#include "create_sliders.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "converters.h"

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

    glGenerateMipmap(GL_TEXTURE_2D);

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

int execute_rgb2hsv() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "RGB to HSV Converter", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    setupImGui(window);

    int width, height, channels;
    unsigned char* image = loadImage("/Users/controldata/GitHub/computer-graphics/input/input.png", &width, &height, &channels);

    GLuint textureID = createTextureFromImage(image, width, height, channels);

    std::vector<float> hsvImage(width * height * 3);
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

    float hueAdjust = 0.0f, saturationAdjust = 0.0f, brightnessAdjust = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        applyHSVAdjustments(image, hsvImage, width, height, channels, hueAdjust, saturationAdjust, brightnessAdjust);
        updateTexture(textureID, image, width, height, channels);

        renderImGui(textureID, width, height);

        createSliders(hueAdjust, saturationAdjust, brightnessAdjust);

        ImGui::Render();

        glViewport(0, 0, 1280, 720);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    saveImage("/Users/controldata/GitHub/computer-graphics/output/output.png", image, width, height, channels);

    glDeleteTextures(1, &textureID);
    cleanupImGui();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}