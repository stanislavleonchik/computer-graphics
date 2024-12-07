#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "Mesh.h"
#include "Matrix4x4.h"
#include "create_polyhedrons.h"
#include "affine_transforms3D.h"
#include "load_obj.h"
#include "save_obj.h"
#include <chrono>

GLuint CompileShader(GLenum type, const std::string& source);

void setup_style(ImGuiStyle& style, ImGuiIO& io);

void setup_imgui(GLFWwindow* window);

int main() {
    if (!glfwInit()) { // Инициализация GLFW
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Настройка GLFW для использования современного OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Для Mac OS X

    GLFWmonitor* monitor = glfwGetPrimaryMonitor(); // Получаем указатель на основной монитор
    const GLFWvidmode* mode = glfwGetVideoMode(monitor); // Получаем текущее разрешение монитора

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Lemotech 3D", nullptr, nullptr); // Создание окна
    if (!window) {
        std::cerr << "Не удалось создать окно GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (imgl3wInit() != GL3W_OK) {
        std::cerr << "Failed to initialize OpenGL loader." << std::endl;
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // Инициализация GLAD
        std::cerr << "Не удалось инициализировать GLAD" << std::endl;
        return -1;
    }

    setup_imgui(window);
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

    glEnable(GL_DEPTH_TEST); // Включение глубинного теста

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    float vertices[] = {
            0.0f,  0.5f, 0.0f, // Верхняя вершина
            -0.5f, -0.5f, 0.0f, // Левая нижняя вершина
            0.5f, -0.5f, 0.0f  // Правая нижняя вершина
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(0);

    std::string vertexShaderSource = R"(
    #version 410 core
    layout(location = 0) in vec3 aPos;

    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
    )";

    std::string fragmentShaderSource = R"(
    #version 410 core

    out vec4 FragColor;

    void main() {
        FragColor = vec4(gl_FragCoord.x / 2800.0, gl_FragCoord.y / 1600.0, 1600.0, 1.0);
    }
    )";

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram(); // Создание шейдерной программы
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success; // Проверка на ошибки линковки
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Ошибка линковки шейдерной программы: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader); // Удаление шейдеров, они больше не нужны
    glDeleteShader(fragmentShader);

    /// Основной цикл
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /// Очистка буфера цвета и глубины
        glUseProgram(shaderProgram); /// Используем шейдерную программу

        glfwPollEvents(); /// Обработка событий


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Render();

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Обновление окна
        glfwSwapBuffers(window);
    }
    /// Конец основного цикла

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    glfwTerminate();
    return 0;
}

GLuint CompileShader(GLenum type, const std::string& source) { // Функция компиляции шейдера
    GLuint shader = glCreateShader(type);
    const char* src_cstr = source.c_str();
    glShaderSource(shader, 1, &src_cstr, nullptr);
    glCompileShader(shader);

    GLint success; // Проверка на ошибки компиляции
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Ошибка компиляции шейдера: " << infoLog << std::endl;
    }
    return shader;
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

void setup_imgui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    setup_style(ImGui::GetStyle(), io);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 410");

    io.Fonts->AddFontFromFileTTF("../assets/helvetica_regular.otf", 16.0f);
    io.FontDefault = io.Fonts->Fonts.back();

    unsigned char* tex_pixels = nullptr;
    int tex_width, tex_height;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_pixels);
    io.Fonts->TexID = (void *)(intptr_t)tex;
}