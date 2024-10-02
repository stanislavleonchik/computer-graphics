#include "lab1/draw_graph.h"
#include "lab2/rgb2hsv/rgb2hsv.h"

int main(int argc, char** argv) {
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

    int x0 = 0, y0 = 0, x1 = width - 1, y1 = height - 1;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        applyHSVAdjustments(image, hsvImage, width, height, channels, hueAdjust, saturationAdjust, brightnessAdjust);

        draw_bresenham_line(image, width, height, channels, x0, y0, x1, y1);

        updateTexture(textureID, image, width, height, channels);

        renderImGui(textureID, width, height);

        create_sliders(hueAdjust, saturationAdjust, brightnessAdjust);
        create_line_controls(x0, y0, x1, y1, width, height);

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