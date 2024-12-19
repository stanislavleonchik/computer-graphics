#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "Mesh.h"
#include "Matrix4x4.h"
#include "create_plot.h"
#include "affine_transforms3D.h"
#include "rotation_figure_creator.h"
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
void ModelsView(Mesh& mesh, int& meshNum, GLuint& texture);
void updateMeshBuffers(GLuint VBO, GLuint EBO, const Mesh& mesh);
GLuint LoadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // Загрузка изображения
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        // Передача данных текстуры в OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Установка параметров обертки и фильтрации
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Не удалось загрузить текстуру: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

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

    Mesh mesh = loadOBJ("../assets/cube.obj");
    Mesh cameraMesh = loadOBJ("../assets/camera.obj");
    mesh.init_edges_faces();
    cameraMesh.init_edges_faces();
    // Загрузка текстуры
    GLuint texture = LoadTexture("../assets/uss.png");
    if (texture == 0) {
        std::cerr << "Текстура не загружена!" << std::endl;
    }

    SurfaceParams params(
        [](float x, float y) { return std::sin(x) * std::cos(y); }, // Функция z = sin(x) * cos(y)
        -3.14f, 3.14f,                                            // Диапазон по x
        -3.14f, 3.14f,                                            // Диапазон по y
        50,                                                       // Разрешение
        "Sine-Cosine Surface"                                     // Имя поверхности
    );

    // Создание VAO и VBO для граней
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Загрузка данных вершин
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Point3), &mesh.vertices[0], GL_STATIC_DRAW);

    // Загрузка данных индексов граней
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faceIndices.size() * sizeof(unsigned int), &mesh.faceIndices[0], GL_STATIC_DRAW);

    // Атрибут позиции
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)0);
    glEnableVertexAttribArray(0);

    // Атрибут текстурных координат
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)(offsetof(Point3, texCoord)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    GLuint edgeVAO, edgeEBO;
    glGenVertexArrays(1, &edgeVAO);
    glGenBuffers(1, &edgeEBO);

    glBindVertexArray(edgeVAO);

    // Используем тот же VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Загрузка данных индексов ребер
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.edgeIndices.size() * sizeof(unsigned int), &mesh.edgeIndices[0], GL_STATIC_DRAW);

    // Атрибут позиции
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)0);
    glEnableVertexAttribArray(0);

    // Атрибут текстурных координат (можно отключить, если не нужны для ребер)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)(offsetof(Point3, texCoord)));
    glEnableVertexAttribArray(1);

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
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
    )";

    std::string fragmentShaderSource = R"(
#version 410 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform bool useUniformColor;
uniform vec4 color;

void main() {
    if (useUniformColor) {
        FragColor = color;
    } else {
        FragColor = texture(texture1, TexCoord);
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
    bool is_camera_shown = false;
    bool is_camera_tools_shown = false;
    bool is_surface_tools_shown = false;
    bool is_rf_creator_shown = false;
    static int meshNum = 4;
    bool CCTVStandby = true;
    bool isFacesShown = true;

    static int currentProjection = 0; // 0 - Перспективная, 1 - Ортографическая
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
    GLint useUniformColorLoc = glGetUniformLocation(shaderProgram, "useUniformColor");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
    GLint texture1Loc = glGetUniformLocation(shaderProgram, "texture1");

    /// Установка текстурного сэмплера (должен быть установлен один раз, например, после линковки шейдеров)
    glUseProgram(shaderProgram);
    glUniform1i(texture1Loc, 0); // GL_TEXTURE0
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
                if (ImGui::MenuItem("Load Model")) {
                    reset_transformations();
                    /*
                    /Users/controldata/Downloads/utah_teapot_lowpoly.obj
                    */
                    std::string modelFilePath;
                    std::cin >> modelFilePath;
                    mesh = loadOBJ(modelFilePath);
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Point3), &mesh.vertices[0], GL_STATIC_DRAW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.edgeIndices.size() * sizeof(unsigned int), &mesh.edgeIndices[0], GL_STATIC_DRAW);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Model")) {
                ModelsView(mesh, meshNum, texture);

                if (!mesh.vertices.empty()) {
                    mesh.init_edges_faces();
                    updateMeshBuffers(VBO, EBO, mesh);
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Show Tools", NULL, is_tools_shown == 1)) { is_tools_shown = !is_tools_shown; }
            if (ImGui::MenuItem("Camera", NULL, is_camera_shown == 1)) { is_camera_shown = !is_camera_shown; }
            if (ImGui::MenuItem("Camera Tools", NULL, is_camera_tools_shown == 1)) { is_camera_tools_shown = !is_camera_tools_shown; }
            if (ImGui::MenuItem("Surface Tools", NULL, is_surface_tools_shown)) { is_surface_tools_shown = !is_surface_tools_shown; }
            if (ImGui::MenuItem("Rotation figure", NULL, is_rf_creator_shown)) { is_rf_creator_shown = !is_rf_creator_shown; }
            if (ImGui::MenuItem("Perspective", NULL, currentProjection == 0)) { currentProjection = 0; }
            if (ImGui::MenuItem("Axonometric", NULL, currentProjection == 1)) { currentProjection = 1; }
            if (ImGui::MenuItem("Show Faces", NULL, isFacesShown)) { isFacesShown = !isFacesShown; }
            ImGui::EndMainMenuBar();
        }

        if (is_surface_tools_shown) {
            create_surface_menu(is_surface_tools_shown, params, mesh);
            updateMeshBuffers(VBO, EBO, mesh);
        }
        if (is_rf_creator_shown) {
            rf_tools(is_rf_creator_shown, window, mesh);
            updateMeshBuffers(VBO, EBO, mesh);
        }

        make_affine_transforms(model, mesh);

        Matrix4x4 projection;
        if (currentProjection == 0) {
            projection = Matrix4x4::perspective(45.0f * M_PI / 180.0f, (float)screenWidth / screenHeight, nearPlaneDistance, 100.0f);
        } else {
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
            mvp = projection * view * model;
        }

        ImGui::Render();

        glUseProgram(shaderProgram);

        /// Set the MVP matrix
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.m);

        /// Привязка текстуры
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        /// Отрисовка граней с текстурой
        glUniform1i(useUniformColorLoc, GL_FALSE); // Использовать текстуру
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.faceIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        /// Отрисовка ребер (если нужно)
        if (isFacesShown) {
            glUniform1i(useUniformColorLoc, GL_FALSE); // Использовать текстуру
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, mesh.faceIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        /// Отрисовка ребер в белом цвете
        glUniform1i(useUniformColorLoc, GL_TRUE); // Использовать единообразный цвет
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // Белый цвет
        glBindVertexArray(edgeVAO);
        glDrawElements(GL_LINES, static_cast<GLsizei>(mesh.edgeIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (is_camera_shown) {
            // Матрица для объекта камеры
            // Применим трансформации, заданные в camObjPos, camObjRot, camObjScale
            cameraModel = cameraModel * Matrix4x4::translate(Point3(camObjPos.x, camObjPos.y, camObjPos.z));
            cameraModel = cameraModel * Matrix4x4::rotation(camObjRot.x * M_PI/180.0f, Point3(1,0,0));
            cameraModel = cameraModel * Matrix4x4::rotation(camObjRot.y * M_PI/180.0f, Point3(0,1,0));
            cameraModel = cameraModel * Matrix4x4::rotation(camObjRot.z * M_PI/180.0f, Point3(0,0,1));
            cameraModel = cameraModel * Matrix4x4::scale(Point3(camObjScale, camObjScale, camObjScale));
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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &edgeVAO);
    glDeleteBuffers(1, &edgeEBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);
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
void updateMeshBuffers(GLuint VBO, GLuint EBO, const Mesh& mesh) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Point3), &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faceIndices.size() * sizeof(unsigned int), &mesh.faceIndices[0], GL_STATIC_DRAW);
}
void ModelsView(Mesh& mesh, int& meshNum, GLuint& texture) {
    if (ImGui::MenuItem("Tetrahedron", NULL, meshNum == 0)) { meshNum = 0; mesh = loadOBJ("../assets/tetrahedron.obj"); }
    if (ImGui::MenuItem("Hexahedron", NULL, meshNum == 1)) { meshNum = 1; mesh = loadOBJ("../assets/diamond.obj"); }
    if (ImGui::MenuItem("Octahedron", NULL, meshNum == 2)) { meshNum = 2; mesh = loadOBJ("../assets/octahedron.obj"); }
    if (ImGui::MenuItem("Icosahedron", NULL, meshNum == 3)) { meshNum = 3; mesh = loadOBJ("../assets/icosahedron.obj"); }
    if (ImGui::MenuItem("Dodecahedron", NULL, meshNum == 4)) { meshNum = 4; mesh = loadOBJ("../assets/dodecahedron.obj"); }
    if (ImGui::MenuItem("Teapot", NULL, meshNum == 5)) { meshNum = 5; mesh = loadOBJ("../assets/utah_teapot_lowpoly.obj"); }
    if (ImGui::MenuItem("Cube", NULL, meshNum == 6)) { meshNum = 6; mesh = loadOBJ("../assets/cube.obj"); }
    if (ImGui::MenuItem("Sphere", NULL, meshNum == 7)) { meshNum = 7; mesh = loadOBJ("../assets/sphere.obj"); }
    if (ImGui::MenuItem("Shuttle", NULL, meshNum == 8)) { meshNum = 8; mesh = loadOBJ("../assets/shuttle.obj"); }
    if (ImGui::MenuItem("USS Enterprise", NULL, meshNum == 9)) { meshNum = 9; mesh = loadOBJ("../assets/ussenterprise.obj"); texture = LoadTexture("../assets/uss.png"); }
    if (ImGui::MenuItem("Saul", NULL, meshNum == 10)) { meshNum = 10; mesh = loadOBJ("../assets/saul.obj"); texture = LoadTexture("../assets/model.png"); }
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
