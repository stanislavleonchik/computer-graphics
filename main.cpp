#include "includes.h"

int main(int argc, char** argv) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(display_w, display_h, "LemotkPaint", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    setupImGui(window);
    RGBtoHSVImageTransform();
    GLuint textureID = createTextureFromImage(image, width, height, channels);

    

    bool showEditor = false;
    bool showSliders = false;
    bool showTools = false;
    bool affine_t = false;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Главное меню
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open")) {
                    // TODO
                }
                if (ImGui::MenuItem("Save")) {
                    saveImage("../output/output.png", image, width, height, channels);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Show Editor", NULL, showEditor)) {
                    showEditor = !showEditor;
                }
                if (ImGui::MenuItem("Show HSV Sliders", NULL, showSliders)) {
                    showSliders = !showSliders;
                }
                if (ImGui::MenuItem("Show Drawing Tools", NULL, showTools)) {
                    showTools = !showTools;
                }
                if (ImGui::MenuItem("Show Affine Tools", NULL, affine_t)) {
                    affine_t = !affine_t;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (showEditor) {
            create_editor(textureID);
        }
        if (showSliders) {
            create_sliders(hueAdjust, saturationAdjust, brightnessAdjust);
        }
        if (showTools) {
            create_tools(tool, thickness);
        }
        if (affine_t) {
            create_affine_tools();
        }

        ImGui::Render();
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &textureID);
    cleanupImGui();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}