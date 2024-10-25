#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"

#include "supporting_files/imgui_support.h"

#include "utils/converters/rgb_to_hsv_image_transform.h"
#include "utils/converters/image_to_texture.h"
#include "supporting_files/load_image.h"

#include "views/hsv_sliders.h"
#include "views/editor/editor.h"
#include "views/drawing_tools_view.h"
#include "views/affine_tools.h"

#include "models/Line.h"
#include "models/Tool.h"
#include "models/Color.h"
#include "models/Polygon.h"

using std::vector;
using matrixf = vector<vector<float>>;

void setup_imgui(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}

int main() {
    const int DISPLAY_WIDTH = 1280;
    const int DISPLAY_HEIGHT = 720;
    int IMAGE_EDITOR_WIDTH;
    int IMAGE_EDITOR_HEIGHT;
    int IMAGE_EDITOR_CHANNELS;
    unsigned char* EDITOR_IMAGE;
    EDITOR_IMAGE = load_image(
            "../assets/blank.png",
            &IMAGE_EDITOR_WIDTH,
            &IMAGE_EDITOR_HEIGHT,
            &IMAGE_EDITOR_CHANNELS
    );

    float hue_adjust;
    float saturation_adjust;
    float brightness_adjust;
    int left_or_right = 2;
    int is_inside = 0;
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL major version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); // OpenGL minor version
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS

    GLFWwindow* window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Lemotech", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (imgl3wInit() != GL3W_OK) { // Use the correct function here
        std::cerr << "Failed to initialize OpenGL loader." << std::endl;
        return -1;
    }

    // Check OpenGL version
    const GLubyte* version = glGetString(GL_VERSION);
    if (version) {
        std::cout << "OpenGL Version: " << version << std::endl;
    } else {
        std::cerr << "Failed to retrieve OpenGL version." << std::endl;
    }

    vector<Line> lines = {};
    vector<Polygon> polygons = {};
    size_t current_polygon = 0;
    ImVec2 intersection_point;

    bool is_drawing = false;

    Tool current_tool = Tool::standby;

    int current_thickness = 1;
    Color current_color = {0, 0, 0};

    AffineMatrix amatrix;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    setup_imgui(window);

    auto hsv_image = rgv_to_hsv_image_transform(
            EDITOR_IMAGE,
            IMAGE_EDITOR_WIDTH,
            IMAGE_EDITOR_HEIGHT,
            IMAGE_EDITOR_CHANNELS
    );

    GLuint textureID = image_to_texture(
            EDITOR_IMAGE,
            IMAGE_EDITOR_WIDTH,
            IMAGE_EDITOR_HEIGHT,
            IMAGE_EDITOR_CHANNELS
    );

    bool is_editor_shown = false;
    bool is_hsv_sliders_shown = false;
    bool is_drawing_tools_view_shown = false;
    bool is_affine_tools_shown = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Главное меню
        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("Open")) {
                    EDITOR_IMAGE = load_image(
                            "../assets/blank.png",
                            &IMAGE_EDITOR_WIDTH,
                            &IMAGE_EDITOR_HEIGHT,
                            &IMAGE_EDITOR_CHANNELS
                    );
                }

                if (ImGui::MenuItem("Save")) {
                    save_image(
                            "../output/output.png",
                            EDITOR_IMAGE,
                            IMAGE_EDITOR_WIDTH,
                            IMAGE_EDITOR_HEIGHT,
                            IMAGE_EDITOR_CHANNELS
                    );
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Show Editor", NULL, is_editor_shown)) {
                    is_editor_shown = !is_editor_shown;
                }
                if (ImGui::MenuItem("Show HSV Sliders", NULL, is_hsv_sliders_shown)) {
                    is_hsv_sliders_shown = !is_hsv_sliders_shown;
                }
                if (ImGui::MenuItem("Show Drawing Tools", NULL, is_drawing_tools_view_shown)) {
                    is_drawing_tools_view_shown = !is_drawing_tools_view_shown;
                }
                if (ImGui::MenuItem("Show Affine Tools", NULL, is_affine_tools_shown)) {
                    is_affine_tools_shown = !is_affine_tools_shown;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (is_editor_shown) {
            create_editor(
                    DISPLAY_WIDTH, // const int
                    DISPLAY_HEIGHT, // const int

                    EDITOR_IMAGE, // unsigned char*
                    IMAGE_EDITOR_WIDTH, // int
                    IMAGE_EDITOR_HEIGHT, // int
                    IMAGE_EDITOR_CHANNELS, // int

                    textureID, // GLuint
                    hsv_image, // vector<float>&
                    hue_adjust, // float&
                    saturation_adjust, // float&
                    brightness_adjust, // float&

                    lines, // vector<Line>&
                    polygons, // vector<Polygon>&

                    current_tool, // Tool&
                    is_drawing, // bool&
                    current_polygon, // size_t&
                    current_thickness, // int
                    current_color, // Color
                    intersection_point, // ImVec2&
                    left_or_right, // int&
                    is_inside // int&
            );
        }
        if (is_hsv_sliders_shown) {
            create_hsv_sliders(hue_adjust, saturation_adjust, brightness_adjust);
        }
        if (is_drawing_tools_view_shown) {
            create_drawing_tools_view(
                    current_tool, // Tool&
                    current_thickness, // int&
                    is_drawing, // bool&
                    current_color, // Color&
                    polygons, // vector<Polygon>
                    intersection_point // ImVec2&
            );
        }
        if (is_affine_tools_shown) {
            create_affine_tools(polygons, current_tool);
        }

        ImGui::Render();
        glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &textureID);
    cleanup_imgui();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}