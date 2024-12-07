#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "Mesh.h"
#include "Matrix4x4.h"
#include "affine_transforms3D.h"
#include "load_obj.h"
#include "save_obj.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

GLuint CompileShader(GLenum type, const std::string& source);

double lastX, lastY; // Глобальные переменные для управления камерой
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
bool pressed = false;

Point3 cameraPos(0.0f, 0.0f, 3.0f);
Point3 cameraFront(0.0f, 0.0f, -1.0f);
Point3 cameraUp(0.0f, 1.0f, 0.0f);

static Point3 camObjPos(-2.133f, 1.0f, 2.434f);
static Point3 camObjRot(0.0f, 0.0f, 0.0f);
static float camObjScale = 0.1f;
static float cameraRadius = 7.0f;
float angle = 0.0f;
float angularSpeed = 1.0f;
static bool CCTV = false;

void updateCameraPosition(float deltaTime) {
    angle += angularSpeed * deltaTime; // Увеличиваем угол
    if (angle > 2.0f * M_PI) {
        angle -= 2.0f * M_PI; // Ограничиваем угол в пределах от 0 до 2π
    }

    // Вычисляем координаты камеры
    camObjPos.x = cameraRadius * std::cos(angle);
    camObjPos.z = cameraRadius * std::sin(angle);

    // Центр взгляда остаётся в начале координат
    camObjRot.y = -cameraPos.normalize().y;
}

void updateCameraFront() {
    Point3 center(0.0f, 0.0f, 0.0f); // Центр координат
    Point3 direction = center - Point3(camObjPos.x, camObjPos.y, camObjPos.z); // Вектор направления
    Point3 normalizedDirection = direction.normalize(); // Нормализация

    // Преобразуем нормализованный вектор в углы поворота
    camObjRot.y = (std::atan2(normalizedDirection.x, normalizedDirection.z) - 90.0f) * 180.0f / M_PI; // Угол вокруг оси Y

}

void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void processCursorToggle(GLFWwindow* window);

void setup_style(ImGuiStyle& style, ImGuiIO& io);

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void char_callback(GLFWwindow* window, unsigned int codepoint);

void setup_imgui(GLFWwindow* window);

void show_camera_object_tools(bool& is_camera_tools_shown);


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
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);
    int screenWidth, screenHeight; // Задание размера окна просмотра
    float nearPlaneDistance = 0.7f;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

    glEnable(GL_DEPTH_TEST); // Включение глубинного теста

    Mesh mesh = loadOBJ("../assets/dodecahedron.obj");
    Mesh cameraMesh = loadOBJ("../assets/camera.obj");
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.edgeIndices.push_back(idx0);
            mesh.edgeIndices.push_back(idx1);
        }
        for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
            mesh.faceIndices.push_back(poly.vertex_indices[0]);
            mesh.faceIndices.push_back(poly.vertex_indices[i]);
            mesh.faceIndices.push_back(poly.vertex_indices[i + 1]);
        }
    }

    for (const auto& poly : cameraMesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            cameraMesh.edgeIndices.push_back(idx0);
            cameraMesh.edgeIndices.push_back(idx1);
        }
        for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
            cameraMesh.faceIndices.push_back(poly.vertex_indices[0]);
            cameraMesh.faceIndices.push_back(poly.vertex_indices[i]);
            cameraMesh.faceIndices.push_back(poly.vertex_indices[i + 1]);
        }
    }

    GLuint VBO;
    GLuint faceVAO, faceEBO;
    GLuint edgeVAO, edgeEBO;

    glGenVertexArrays(1, &faceVAO);
    glGenVertexArrays(1, &edgeVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &faceEBO);
    glGenBuffers(1, &edgeEBO);

// Set up face VAO
    glBindVertexArray(faceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Point3), &mesh.vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faceIndices.size() * sizeof(unsigned int), &mesh.faceIndices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)nullptr);
    glEnableVertexAttribArray(0);

// Set up edge VAO
    glBindVertexArray(edgeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO is already filled
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.edgeIndices.size() * sizeof(unsigned int), &mesh.edgeIndices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // отдельный VBO/VAO для cameraMesh
    GLuint cameraVBO, cameraVAO, cameraEBO;
    GLuint cameraEdgeVAO, cameraEdgeEBO;
    glGenVertexArrays(1, &cameraVAO);
    glGenVertexArrays(1, &cameraEdgeVAO);
    glGenBuffers(1, &cameraVBO);
    glGenBuffers(1, &cameraEBO);
    glGenBuffers(1, &cameraEdgeEBO);

    glBindVertexArray(cameraVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cameraVBO);
    glBufferData(GL_ARRAY_BUFFER, cameraMesh.vertices.size() * sizeof(Point3), &cameraMesh.vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cameraEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cameraMesh.faceIndices.size() * sizeof(unsigned int), &cameraMesh.faceIndices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(cameraEdgeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cameraVBO); // VBO is already filled
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cameraEdgeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cameraMesh.edgeIndices.size() * sizeof(unsigned int), &cameraMesh.edgeIndices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    std::string vertexShaderSource = R"(
#version 410 core
layout(location = 0) in vec3 aPos;

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
}
    )";

    std::string fragmentShaderSource = R"(
#version 410 core

uniform vec4 color;
uniform bool useUniformColor;

out vec4 FragColor;
void main() {
    if (useUniformColor) {
        FragColor = color;
    } else {
        FragColor = vec4(gl_FragCoord.x / 2800.0, gl_FragCoord.y / 1600.0, 1600.0, 1.0);
    }
}
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
    bool is_camera_shown = true;
    bool is_camera_tools_shown = true;
    static int currentPolyhedron = 4;
    bool CCTVStandby = true;

    static int currentProjection = 0; // 0 - Перспективная, 1 - Ортографическая
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
    GLint useUniformColorLoc = glGetUniformLocation(shaderProgram, "useUniformColor");
    /// Основной цикл
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /// Очистка буфера цвета и глубины
        glUseProgram(shaderProgram); /// Используем шейдерную программу

        glfwPollEvents(); /// Обработка событий

        processCursorToggle(window);
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Matrix4x4 model;
        Matrix4x4 cameraModel;
        if (is_tools_shown) {
            create_affine_tools(is_tools_shown);
        }

        if (CCTV && CCTVStandby) {
            cache_transformations();      // Сохранение текущих трансформаций
            reset_transformations();      // Сброс параметров
            CCTVStandby = false;          // Переход в активный режим CCTV
        } else if (!CCTV && !CCTVStandby) {
            uncache_transformations();    // Восстановление трансформаций
            CCTVStandby = true;           // Возвращение в standby
        }

        make_affine_transforms(model, mesh);
        make_affine_transforms(cameraModel, cameraMesh);

        auto currentTime = std::chrono::high_resolution_clock::now();
        static auto lastTime = currentTime;
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        updateCameraPosition(deltaTime);
        updateCameraFront();

        show_camera_object_tools(is_camera_tools_shown);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save")) {
                    std::string savePath = "../assets/saved_mesh.obj";
                    Mesh transformedMesh = mesh;
                    for (auto& vertex : transformedMesh.vertices) {
                        vertex = model * vertex;
                    }
                    saveOBJ(transformedMesh, savePath);
                    std::cout << "Модель сохранена в файл: " << savePath << std::endl;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Model")) {
                if (ImGui::MenuItem("Tetrahedron", NULL, currentPolyhedron == 0)) { currentPolyhedron = 0; }
                if (ImGui::MenuItem("Hexahedron", NULL, currentPolyhedron == 1)) { currentPolyhedron = 1; }
                if (ImGui::MenuItem("Octahedron", NULL, currentPolyhedron == 2)) { currentPolyhedron = 2; }
                if (ImGui::MenuItem("Icosahedron", NULL, currentPolyhedron == 3)) { currentPolyhedron = 3; }
                if (ImGui::MenuItem("Dodecahedron", NULL, currentPolyhedron == 4)) { currentPolyhedron = 4; }
                if (ImGui::MenuItem("Teapot", NULL, currentPolyhedron == 5)) { currentPolyhedron = 5; }
                if (ImGui::MenuItem("Cube", NULL, currentPolyhedron == 6)) { currentPolyhedron = 6; }
                if (ImGui::MenuItem("Sphere", NULL, currentPolyhedron == 7)) { currentPolyhedron = 7; }
                if (ImGui::MenuItem("Shuttle", NULL, currentPolyhedron == 8)) { currentPolyhedron = 8; }
                if (ImGui::MenuItem("USS Enterprise", NULL, currentPolyhedron == 9)) { currentPolyhedron = 9; }
                if (ImGui::MenuItem("Soul", NULL, currentPolyhedron == 10)) { currentPolyhedron = 10; }
                switch (currentPolyhedron) {
                    case 0: mesh = loadOBJ("../assets/tetrahedron.obj"); break;
                    case 1: mesh = loadOBJ("../assets/diamond.obj"); break;
                    case 2: mesh = loadOBJ("../assets/octahedron.obj"); break;
                    case 3: mesh = loadOBJ("../assets/icosahedron.obj"); break;
                    case 4: mesh = loadOBJ("../assets/dodecahedron.obj"); break;
                    case 5: mesh = loadOBJ("../assets/utah_teapot_lowpoly.obj"); break;
                    case 6: mesh = loadOBJ("../assets/cube.obj"); break;
                    case 7: mesh = loadOBJ("../assets/sphere.obj"); break;
                    case 8: mesh = loadOBJ("../assets/shuttle.obj"); break;
                    case 9: mesh = loadOBJ("../assets/ussenterprise.obj"); break;
                    case 10: mesh = loadOBJ("../assets/soul.obj"); break;
                }

                mesh.faceIndices.clear();
                mesh.edgeIndices.clear();
                for (const auto& poly : mesh.polygons) {
                    for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
                        int idx0 = poly.vertex_indices[i];
                        int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
                        mesh.edgeIndices.push_back(idx0);
                        mesh.edgeIndices.push_back(idx1);
                    }
                    for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
                        mesh.faceIndices.push_back(poly.vertex_indices[0]);
                        mesh.faceIndices.push_back(poly.vertex_indices[i]);
                        mesh.faceIndices.push_back(poly.vertex_indices[i + 1]);
                    }
                }

                // Update VBO (vertex buffer)
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Point3), &mesh.vertices[0], GL_STATIC_DRAW);

                // Update faceEBO
                glBindVertexArray(faceVAO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faceIndices.size() * sizeof(unsigned int), &mesh.faceIndices[0], GL_STATIC_DRAW);

                // Update edgeEBO
                glBindVertexArray(edgeVAO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.edgeIndices.size() * sizeof(unsigned int), &mesh.edgeIndices[0], GL_STATIC_DRAW);

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Show Tools", NULL, is_tools_shown == 1)) { is_tools_shown = !is_tools_shown; }
            if (ImGui::MenuItem("Camera", NULL, is_camera_shown == 1)) { is_camera_shown = !is_camera_shown; }
            if (ImGui::MenuItem("Camera Tools", NULL, is_camera_tools_shown == 1)) { is_camera_tools_shown = !is_camera_tools_shown; }
            if (ImGui::MenuItem("Perspective", NULL, currentProjection == 0)) { currentProjection = 0; }
            if (ImGui::MenuItem("Axonometric", NULL, currentProjection == 1)) { currentProjection = 1; }
            ImGui::EndMainMenuBar();
        }

        Matrix4x4 projection;
        if (currentProjection == 0) {
            projection = Matrix4x4::perspective(45.0f * M_PI / 180.0f, (float)screenWidth / screenHeight, nearPlaneDistance, 100.0f);
        }
        else {
            float orthoScale = 2.0f;
            float aspectRatio = (float)screenWidth / screenHeight;
            float left = -orthoScale * aspectRatio;
            float right = orthoScale * aspectRatio;
            float bottom = -orthoScale;
            float top = orthoScale;
            float near1 = 0.1f;
            float far1 = 100.0f;

            projection = Matrix4x4::orthographic(left, right, bottom, top, near1, far1);
        }

        Matrix4x4 view = Matrix4x4::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Матрица для объекта камеры
        // Применим трансформации, заданные в camObjPos, camObjRot, camObjScale
        cameraModel = cameraModel * Matrix4x4::translate(Point3(camObjPos.x, camObjPos.y, camObjPos.z));
        cameraModel = cameraModel * Matrix4x4::rotation(camObjRot.x * M_PI/180.0f, Point3(1,0,0));
        cameraModel = cameraModel * Matrix4x4::rotation(camObjRot.y * M_PI/180.0f, Point3(0,1,0));
        cameraModel = cameraModel * Matrix4x4::rotation(camObjRot.z * M_PI/180.0f, Point3(0,0,1));
        cameraModel = cameraModel * Matrix4x4::scale(Point3(camObjScale, camObjScale, camObjScale));

        Matrix4x4 mvp;
        if (CCTV) {
            projection = Matrix4x4::perspective(45.0f * M_PI / 180.0f, (float)screenWidth / screenHeight, nearPlaneDistance, 100.0f);
            view = Matrix4x4::lookAt(
                    Point3(camObjPos.x, camObjPos.y, camObjPos.z), // Позиция камеры
                    Point3(0.0f, 0.0f, 0.0f),                     // Точка, на которую смотрит камера
                    Point3(0.0f, 1.0f, 0.0f)                      // Вектор "вверх"
            );

            mvp = projection * view * model;
        } else {
            mvp = projection * view * model; // Правильный порядок умножения матриц
        }

        ImGui::Render();

        glUseProgram(shaderProgram);

        // Set the MVP matrix
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.m);

        // --- Draw Faces with Gradient Color ---
        glUniform1i(useUniformColorLoc, GL_FALSE); // Use gradient color
        glBindVertexArray(faceVAO);
        glDrawElements(GL_TRIANGLES, mesh.faceIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // --- Draw Edges in White Color ---
        glUniform1i(useUniformColorLoc, GL_TRUE); // Use uniform color
        glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 1.0f); // Set color to white
        glBindVertexArray(edgeVAO);
        glDrawElements(GL_LINES, mesh.edgeIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (is_camera_shown) {
            // Вычисляем MVP для объекта-камеры
            Matrix4x4 cameraObjMVP = projection * view * cameraModel;
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, cameraObjMVP.m);

            // Рисуем камеру
            glUniform1i(useUniformColorLoc, GL_FALSE);
            glBindVertexArray(cameraVAO);
            glDrawElements(GL_TRIANGLES, cameraMesh.faceIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glUniform1i(useUniformColorLoc, GL_TRUE); // Use uniform color
            glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 1.0f); // Set color to white
            glBindVertexArray(cameraEdgeVAO);
            glDrawElements(GL_LINES, cameraMesh.edgeIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Обновление окна
        glfwSwapBuffers(window);
    }
    /// Конец основного цикла

    glDeleteVertexArrays(1, &faceVAO);
    glDeleteVertexArrays(1, &edgeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &faceEBO);
    glDeleteBuffers(1, &edgeEBO);

    glfwTerminate();
    return 0;
}

void show_camera_object_tools(bool& is_camera_tools_shown) {
    if (!is_camera_tools_shown) return;
    ImGui::Begin("Camera Object Tools", &is_camera_tools_shown);
    ImGui::Text("Position");
    ImGui::SliderFloat("X", &camObjPos.x, -10.0f, 10.0f);
    ImGui::SliderFloat("Y", &camObjPos.y, -10.0f, 10.0f);
    ImGui::SliderFloat("Z", &camObjPos.z, -10.0f, 10.0f);

    ImGui::Separator();
    ImGui::Text("Rotation (degrees)");
    ImGui::SliderFloat("Rot X", &camObjRot.x, -180.0f, 180.0f);
    ImGui::SliderFloat("Rot Y", &camObjRot.y, -180.0f, 180.0f);
    ImGui::SliderFloat("Rot Z", &camObjRot.z, -180.0f, 180.0f);

    ImGui::Separator();
    ImGui::Text("Scale");
    ImGui::SliderFloat("Scale", &camObjScale, 0.1f, 2.0f);
    ImGui::Text("Radius");
    ImGui::SliderFloat("Radius", &cameraRadius, 3.0f, 10.0f);

    ImGui::Separator();
    ImGui::Text("CCTV");
    if (ImGui::Checkbox("Watch through CCTV", &CCTV)) {}
    ImGui::End();
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

static bool is_pressed = false;
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        translation[2] += 0.02f;
        if (translation[2] > 99.0f) translation[2] = 99.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        translation[2] -= 0.02f;
        if (translation[2] < -10.0f) translation[2] = -10.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        translation[0] += 0.02f;
        if (translation[0] > 99.0f) translation[0] = 99.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        translation[0] -= 0.02f;
        if (translation[0] < -10.0f) translation[0] = -10.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        translation[1] += 0.02f;
        if (translation[1] > 99.0f) translation[1] = 99.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        translation[1] -= 0.02f;
        if (translation[1] < -10.0f) translation[1] = -10.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!is_pressed) {
            CCTV = true;
            is_pressed = true;
        }
    }
    if ((glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)) {
        if (is_pressed) {
            is_pressed = false;
            CCTV = false;
        }
    }

}

void processCursorToggle(GLFWwindow* window) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!pressed) {
            pressed = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true; // Reset firstMouse flag
        }
    } else {
        if (pressed) {
            pressed = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(lastY - ypos); // Reversed since y-coordinates range from bottom to top
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Clamp the pitch angle
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        float yawRad = yaw * static_cast<float>(M_PI) / 180.0f;
        float pitchRad = pitch * static_cast<float>(M_PI) / 180.0f;

        Point3 front;
        front.x = std::cos(yawRad) * std::cos(pitchRad);
        front.y = std::sin(pitchRad);
        front.z = std::sin(yawRad) * std::cos(pitchRad);
        cameraFront = front.normalize();

        // Update cameraUp vector
        Point3 worldUp(0.0f, 1.0f, 0.0f);
        Point3 cameraRight = cameraFront.cross(worldUp).normalize();
        cameraUp = cameraRight.cross(cameraFront).normalize();
    } else {
        firstMouse = true; // Reset firstMouse flag when cursor is not disabled
    }

    // Pass event to ImGui
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
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

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        mouse_callback(window, xpos, ypos);
    }
    // Передача события в ImGui
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
    ImGui_ImplGlfw_CharCallback(window, codepoint);
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