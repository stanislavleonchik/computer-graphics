#include <iostream>
#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"
#include "supporting_files/imgui_support.h"
#include "views/hsv_sliders.h"
#include "views/editor/editor.h"
#include "views/drawing_tools_view.h"
#include "views/affine_tools.h"
#include "views/fractal_view/fractal_tools.h"
#include "models/Line.h"
#include "models/Tool.h"
#include "models/Polygon.h"
#include "views/fractal_view/fractal_editor.h"
#include "utils/converters/image_to_texture.h"
#include "models/Fractal/Fractal.h"

using matrixf = vector<vector<float>>;
void setup_style(ImGuiStyle& style, ImGuiIO& io);

void setup_imgui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    setup_style(ImGui::GetStyle(), io);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    io.Fonts->AddFontFromFileTTF("../assets/helvetica_regular.otf", 16.0f);
    io.FontDefault = io.Fonts->Fonts.back();

    unsigned char* tex_pixels = NULL;
    int tex_width, tex_height;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_pixels);
    io.Fonts->TexID = (void *)(intptr_t)tex;
}

unsigned char* create_blank_canvas(int width, int height, int channels) {
    size_t size = width * height * channels;
    unsigned char* image = new unsigned char[size];
    for (size_t i = 0; i < size; i += channels) {
        image[i] = 30;     // Red
        image[i + 1] = 30; // Green
        image[i + 2] = 30; // Blue
        if (channels == 4) {
            image[i + 3] = 255; // Alpha
        }
    }
    return image;
}

int main() {
    const int DISPLAY_WIDTH = 1280;
    const int DISPLAY_HEIGHT = 720;
    const int IMAGE_EDITOR_WIDTH = 800;
    const int IMAGE_EDITOR_HEIGHT = 600;
    const int IMAGE_EDITOR_CHANNELS = 4;

    unsigned char* EDITOR_IMAGE = create_blank_canvas(IMAGE_EDITOR_WIDTH, IMAGE_EDITOR_HEIGHT, IMAGE_EDITOR_CHANNELS);
    float hue_adjust = 0.0f;
    float saturation_adjust = 0.0f;
    float brightness_adjust = 0.0f;
    int left_or_right = 2;
    int is_inside = 0;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Lemotech", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (imgl3wInit() != GL3W_OK) {
        std::cerr << "Failed to initialize OpenGL loader." << std::endl;
        return -1;
    }
    const GLubyte* version = glGetString(GL_VERSION);
    if (version) {
        std::cout << "OpenGL Version: " << version << std::endl;
    } else {
        std::cerr << "Failed to retrieve OpenGL version." << std::endl;
    }

    std::vector<Line> lines = {};
    std::vector<Polygon> polygons = {};
    size_t current_polygon = 0;
    ImVec2 intersection_point;
    bool is_drawing = false;
    Tool current_tool = Tool::draw_polygon;
    int current_thickness = 1;
    ImColor current_color = {0, 0, 0};
    AffineMatrix amatrix;

    Fractal fractal;

    setup_imgui(window);

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
    bool is_fractal_tool_shown = true;
    bool is_fractal_edit_view_shown = true;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Clean")) {
                    delete[] EDITOR_IMAGE;
                    EDITOR_IMAGE = create_blank_canvas(IMAGE_EDITOR_WIDTH, IMAGE_EDITOR_HEIGHT, IMAGE_EDITOR_CHANNELS);
                }
                if (ImGui::MenuItem("Save")) {
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Editor", NULL, is_editor_shown)) {
                    is_editor_shown = !is_editor_shown;
                }
                if (ImGui::MenuItem("HSV Sliders", NULL, is_hsv_sliders_shown)) {
                    is_hsv_sliders_shown = !is_hsv_sliders_shown;
                }
                if (ImGui::MenuItem("Drawing Tools", NULL, is_drawing_tools_view_shown)) {
                    is_drawing_tools_view_shown = !is_drawing_tools_view_shown;
                }
                if (ImGui::MenuItem("Affine Tools", NULL, is_affine_tools_shown)) {
                    is_affine_tools_shown = !is_affine_tools_shown;
                }
                if (ImGui::MenuItem("Fractal Tools", NULL, is_fractal_tool_shown)) {
                    is_fractal_tool_shown = !is_fractal_tool_shown;
                }
                if (ImGui::MenuItem("Fractal Editor", NULL, is_fractal_edit_view_shown)) {
                    is_fractal_edit_view_shown = !is_fractal_edit_view_shown;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (is_editor_shown) {
            Editor::create_editor(
                    DISPLAY_WIDTH,
                    DISPLAY_HEIGHT,
                    EDITOR_IMAGE,
                    IMAGE_EDITOR_WIDTH,
                    IMAGE_EDITOR_HEIGHT,
                    IMAGE_EDITOR_CHANNELS,
                    textureID,
                    lines,
                    polygons,
                    current_tool,
                    is_drawing,
                    current_polygon,
                    current_thickness,
                    current_color,
                    intersection_point,
                    left_or_right,
                    is_inside
            );
        }

        if (is_drawing_tools_view_shown) {
            create_drawing_tools_view(
                    current_tool,
                    current_thickness,
                    is_drawing,
                    current_color,
                    polygons,
                    intersection_point
            );
        }

        if (is_affine_tools_shown) {
            create_affine_tools(polygons, current_tool);
        }

        if (is_fractal_tool_shown) {
            fractal_tools(fractal);
        }

        if (is_fractal_edit_view_shown) {
            create_fractal_editor(
                    DISPLAY_WIDTH,
                    DISPLAY_HEIGHT,
                    EDITOR_IMAGE,
                    IMAGE_EDITOR_WIDTH,
                    IMAGE_EDITOR_HEIGHT,
                    IMAGE_EDITOR_CHANNELS,
                    textureID,
                    fractal
                    );
        }

        ImGui::Render();
        glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    delete[] EDITOR_IMAGE;
    glDeleteTextures(1, &textureID);
    cleanup_imgui();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void setup_style(ImGuiStyle& style, ImGuiIO& io) {
    style.FrameRounding = 12.0f;
    style.FrameBorderSize = 1.0f;
    style.WindowRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ChildRounding = 6.0f;
    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 10);
    ImVec4 buttonColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
    ImVec4 buttonHoveredColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    ImVec4 buttonActiveColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    ImVec4 borderColor = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    ImVec4 shadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.2f);
    style.Colors[ImGuiCol_Button] = buttonColor;
    style.Colors[ImGuiCol_ButtonHovered] = buttonHoveredColor;
    style.Colors[ImGuiCol_ButtonActive] = buttonActiveColor;
    style.Colors[ImGuiCol_Border] = borderColor;
    style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
}
