#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "rotation_tools.h"
#include "Mesh.h"
#include "Matrix4x4.h"


Mesh createTetrahedron() { // Функция для создания тетраэдра
    Mesh mesh;
    float sqrt2 = std::sqrt(2.0f);

    mesh.vertices = {
            Point3(1, 0, -1 / sqrt2),
            Point3(-1, 0, -1 / sqrt2),
            Point3(0, 1, 1 / sqrt2),
            Point3(0, -1, 1 / sqrt2)
    };

    mesh.polygons = {
            {{0, 1, 2}},
            {{0, 1, 3}},
            {{0, 2, 3}},
            {{1, 2, 3}}
    };

    mesh.indices.clear(); // Создание списка индексов
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
        }
    }

    return mesh;
}

Mesh createHexahedron() { // Функция для создания гексаэдра (куба)
    Mesh mesh;

    mesh.vertices = {
            Point3(-0.5f, -0.5f, -0.5f),
            Point3(0.5f, -0.5f, -0.5f),
            Point3(0.5f,  0.5f, -0.5f),
            Point3(-0.5f,  0.5f, -0.5f),
            Point3(-0.5f, -0.5f,  0.5f),
            Point3(0.5f, -0.5f,  0.5f),
            Point3(0.5f,  0.5f,  0.5f),
            Point3(-0.5f,  0.5f,  0.5f)
    };

    mesh.polygons = {
            {{0, 1, 2, 3}},
            {{4, 5, 6, 7}}, // Задняя грань
            {{0, 1, 5, 4}}, // Передняя грань
            {{2, 3, 7, 6}}, // Нижняя грань
            {{0, 3, 7, 4}}, // Верхняя грань
            {{1, 2, 6, 5}} // Левая грань
    }; // Правая грань

    mesh.indices.clear(); // Создание списка индексов
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
        }
    }

    return mesh;
}

Mesh createOctahedron() { // Функция для создания октаэдра
    Mesh mesh;

    mesh.vertices = {
            Point3(1, 0, 0),
            Point3(-1, 0, 0),
            Point3(0, 1, 0),
            Point3(0, -1, 0),
            Point3(0, 0, 1),
            Point3(0, 0, -1)
    };

    mesh.polygons = {
            {{0, 2, 4}},
            {{2, 1, 4}},
            {{1, 3, 4}},
            {{3, 0, 4}},
            {{0, 2, 5}},
            {{2, 1, 5}},
            {{1, 3, 5}},
            {{3, 0, 5}}
    };

    mesh.indices.clear(); // Создание списка индексов
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
        }
    }

    return mesh;
}

Mesh createIcosahedron() { // Функция для создания икосаэдра
    Mesh mesh;
    const float t = (1.0 + std::sqrt(5.0)) / 2.0;

    mesh.vertices = {
            Point3(-1,  t,  0),
            Point3( 1,  t,  0),
            Point3(-1, -t,  0),
            Point3( 1, -t,  0),
            Point3( 0, -1,  t),
            Point3( 0,  1,  t),
            Point3( 0, -1, -t),
            Point3( 0,  1, -t),
            Point3( t,  0, -1),
            Point3( t,  0,  1),
            Point3(-t,  0, -1),
            Point3(-t,  0,  1)
    };

    mesh.polygons = {
            {{0, 11, 5}}, {{0, 5, 1}}, {{0, 1, 7}}, {{0, 7, 10}}, {{0, 10, 11}},
            {{1, 5, 9}}, {{5, 11, 4}}, {{11, 10, 2}}, {{10, 7, 6}}, {{7, 1, 8}},
            {{3, 9, 4}}, {{3, 4, 2}}, {{3, 2, 6}}, {{3, 6, 8}}, {{3, 8, 9}},
            {{4, 9, 5}}, {{2, 4, 11}}, {{6, 2, 10}}, {{8, 6, 7}}, {{9, 8, 1}}
    };

    for (auto& v : mesh.vertices) { // Нормализация вершин для единичной сферы
        v = v.normalize();
    }

    mesh.indices.clear(); // Создание списка индексов
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
        }
    }

    return mesh;
}

Mesh createDodecahedron() { // Функция для создания додекаэдра
    Mesh mesh;
    const float phi = (1.0 + std::sqrt(5.0)) / 2.0;
    const float a = 1.0f; // Золотое сечение
    const float b = 1.0f / phi;
    const float c = 2.0f - phi;

    mesh.vertices = { // Координаты вершин додекаэдра
            { c,  0,  a},
            {-c,  0,  a}, // 1
            {-b,  b,  b}, // 2
            { 0,  a,  c}, // 3
            { b,  b,  b}, // 4
            { b, -b,  b}, // 5
            { 0, -a,  c}, // 6
            {-b, -b,  b}, // 7
            { c,  0, -a}, // 8
            {-c,  0, -a}, // 9
            {-b, -b, -b}, // 10
            { 0, -a, -c}, // 11
            { b, -b, -b}, // 12
            { b,  b, -b}, // 13
            { 0,  a, -c}, // 14
            {-b,  b, -b}, // 15
            { a,  c,  0}, // 16
            {-a,  c,  0}, // 17
            {-a, -c,  0}, // 18
            { a, -c,  0} // 19
    }; // 20

    for (auto& v : mesh.vertices) { // Нормализация вершин
        v = v.normalize();
    }

    mesh.polygons = { // Грани додекаэдра (каждая грань - пятиугольник)
            {{0, 1, 2, 3, 4}},
            {{0, 5, 6, 7, 1}},
            {{0, 4, 16, 19, 5}},
            {{1, 7, 18, 17, 2}},
            {{2, 17, 15, 14, 3}},
            {{3, 14, 13, 16, 4}},
            {{6, 11}},
            {{8, 9, 10, 11, 12}},
            {{8, 13, 14, 15, 9}},
            {{8, 12, 19, 16, 13}},
            {{9, 15, 17, 18, 10}},
    };

    mesh.indices.clear(); // Создание списка индексов
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
        }
    }

    return mesh;
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

double lastX, lastY; // Глобальные переменные для управления камерой
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

Point3 cameraPos(0.0f, 0.0f, 3.0f);
Point3 cameraFront(0.0f, 0.0f, -1.0f);
Point3 cameraUp(0.0f, 1.0f, 0.0f);

void processInput(GLFWwindow* window, float& nearPlaneDistance) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        nearPlaneDistance += 0.02f; // Увеличиваем ближнюю плоскость при движении вперёд
        if (nearPlaneDistance > 99.0f) nearPlaneDistance = 99.0f; // Ограничение для дальности
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        nearPlaneDistance -= 0.02f; // Уменьшаем ближнюю плоскость при движении назад
        if (nearPlaneDistance < -10.0f) nearPlaneDistance = -10.0f; // Ограничение для близости
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    auto xoffset = static_cast<float>(xpos - lastX);
    auto yoffset = static_cast<float>(lastY - ypos);
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) // Ограничение угла обзора по вертикали
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    Point3 front; // Обновление направления камеры
    front.x = std::cos(yaw * M_PI / 180.0f) * std::cos(pitch * M_PI / 180.0f);
    front.y = std::sin(pitch * M_PI / 180.0f);
    front.z = std::sin(yaw * M_PI / 180.0f) * std::cos(pitch * M_PI / 180.0f);
    cameraFront = front.normalize();
}

bool pressed = false;
void processCursorToggle(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS) {
        if (!pressed) {
            pressed = true;
            glfwSetCursorPosCallback(window, mouse_callback);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    } else {
        if (pressed) {
            pressed = false;
            glfwSetCursorPosCallback(window, nullptr);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

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

int main() {
    if (!glfwInit()) { // Инициализация GLFW
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Настройка GLFW для использования современного OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Для Mac OS X

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lemotech 3D", nullptr, nullptr); // Создание окна
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
    int screenWidth, screenHeight; // Задание размера окна просмотра
    float nearPlaneDistance = 0.7f;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

    glEnable(GL_DEPTH_TEST); // Включение глубинного теста

    Mesh mesh = createDodecahedron(); // Переменная для текущего меша

    for (const auto& poly : mesh.polygons) { // Создание списка индексов
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
        }
    }

    GLuint VBO, VAO, EBO; // Создание VBO и VAO
    glGenVertexArrays(1, &VAO); // Vertex Array Object (сохраняет все настройки, которые относятся к вершинным данным (VBO и EBO))
    glGenBuffers(1, &VBO); // Vertex Buffer Object (хранит данные самих вершин)
    glGenBuffers(1, &EBO); // Element Buffer Object (хранит индексы вершин, которые помогают переиспользовать одни и те же вершины)

    glBindVertexArray(VAO); // Привязка VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Вершинный буфер
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Point3), &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Индексный буфер
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)nullptr); // Настройка вершинных атрибутов
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Отвязываем VAO

    std::string vertexShaderSource = R"( // Загрузка и компиляция шейдеров
        #version 410 core
        layout(location = 0) in vec3 aPos;

        uniform mat4 uMVP;

        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 410 core
        out vec4 FragColor;

        void main() {
            FragColor = vec4(1.0);
        } // Белый цвет
    )";

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram(); // Создание шейдерной программы
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success; // Проверка на ошибки линковки
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Ошибка линковки шейдерной программы: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader); // Удаление шейдеров, они больше не нужны
    glDeleteShader(fragmentShader);

    bool is_tools_shown = true;
    bool is_rot_tools_show = false;
    bool is_axe = false;
    static int currentPolyhedron = 4;
    const std::map<std::string, bool> polyhedronNames = {{"Tetrahedron", false}, {"Hexahedron", false},
                                                         {"Octahedron", false}, {"Icosahedron", false},
                                                         {"Dodecahedron", false} };
    while (!glfwWindowShouldClose(window)) { // Основной цикл
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистка буфера цвета и глубины

        glfwPollEvents(); // Обработка событий


        processCursorToggle(window);
        processInput(window, nearPlaneDistance);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Clean")) {}
                if (ImGui::MenuItem("Save")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Tools", NULL, is_tools_shown)) { is_tools_shown = !is_tools_shown; }
                if (ImGui::MenuItem("Rotation tools", NULL, is_rot_tools_show)) { 
                    is_rot_tools_show = !is_rot_tools_show;
                }
                if (ImGui::MenuItem("Tetrahedron", NULL, currentPolyhedron == 0)) { currentPolyhedron = 0; }
                if (ImGui::MenuItem("Hexahedron", NULL, currentPolyhedron == 1)) { currentPolyhedron = 1; }
                if (ImGui::MenuItem("Octahedron", NULL, currentPolyhedron == 2)) { currentPolyhedron = 2; }
                if (ImGui::MenuItem("Icosahedron", NULL, currentPolyhedron == 3)) { currentPolyhedron = 3; }
                if (ImGui::MenuItem("Dodecahedron", NULL, currentPolyhedron == 4)) { currentPolyhedron = 4; }
                switch (currentPolyhedron) {
                    case 0: mesh = createTetrahedron(); break;
                    case 1: mesh = createHexahedron(); break;
                    case 2: mesh = createOctahedron(); break;
                    case 3: mesh = createIcosahedron(); break;
                    case 4: mesh = createDodecahedron(); break;
                }
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Point3), &mesh.vertices[0], GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }


        static float translation[3] = {0.0f, 0.0f, 0.0f};
        static float rotation[3] = {0.0f, 0.0f, 0.0f};
        static float scaling[3] = {1.0f, 1.0f, 1.0f};
        if (is_tools_shown) {
            ImGui::Begin("Affine Transformations", &is_tools_shown); // Начало окна ImGui

            ImGui::Text("Translation"); // Смещение
            ImGui::SliderFloat("X", &translation[0], -5.0f, 5.0f);
            ImGui::SliderFloat("Y", &translation[1], -5.0f, 5.0f);
            ImGui::SliderFloat("Z", &translation[2], -5.0f, 5.0f);

            ImGui::Separator();

            ImGui::Text("Rotation (degrees)"); // Вращение
            ImGui::SliderFloat("Pitch", &rotation[0], -180.0f, 180.0f);
            ImGui::SliderFloat("Yaw", &rotation[1], -180.0f, 180.0f);
            ImGui::SliderFloat("Roll", &rotation[2], -180.0f, 180.0f);

            ImGui::Separator();

            ImGui::Text("Scaling"); // Масштабирование
            ImGui::SliderFloat("Scale X", &scaling[0], 0.1f, 5.0f);
            ImGui::SliderFloat("Scale Y", &scaling[1], 0.1f, 5.0f);
            ImGui::SliderFloat("Scale Z", &scaling[2], 0.1f, 5.0f);

            ImGui::End(); // Конец окна ImGui
        }

        Matrix4x4 rot_transform;
        if (is_rot_tools_show) {
            show_rotation_tools(mesh, rot_transform);
        }


        Matrix4x4 projection = Matrix4x4::perspective(45.0f * M_PI / 180.0f, (float)screenWidth / screenHeight, nearPlaneDistance, 100.0f); // Настройка матриц проекции и вида
        Matrix4x4 view = Matrix4x4::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        Matrix4x4 model; // Создание матрицы модели и применение аффинных преобразований

        model = Matrix4x4::scale(Point3(scaling[0], scaling[1], scaling[2])) * model; // Применение масштабирования

        model = Matrix4x4::rotation(rotation[0] * M_PI / 180.0f, Point3(1.0f, 0.0f, 0.0f)) * model; // Применение вращения (последовательно вокруг осей X, Y, Z)
        model = Matrix4x4::rotation(rotation[1] * M_PI / 180.0f, Point3(0.0f, 1.0f, 0.0f)) * model;
        model = Matrix4x4::rotation(rotation[2] * M_PI / 180.0f, Point3(0.0f, 0.0f, 1.0f)) * model;

        model = Matrix4x4::translate(Point3(translation[0], translation[1], translation[2])) * model; // Применение смещения

        model = rot_transform * model;

        Matrix4x4 mvp = model * projection * view; // Формирование общей матрицы MVP
        ImGui::Render();


        glUseProgram(shaderProgram); // Использование шейдерной программы

        GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP"); // Передача матрицы MVP в шейдер
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp.m[0][0]);

        glBindVertexArray(VAO); // Привязка VAO и отрисовка меша
        glDrawElements(GL_LINES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Обновление окна
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO); // Очистка ресурсов
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}