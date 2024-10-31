// main.cpp

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

// Структура для представления точки в 3D пространстве
struct Point3 {
    float x, y, z;

    Point3() : x(0), y(0), z(0) {}
    Point3(float x, float y, float z) : x(x), y(y), z(z) {}

    Point3 operator+(const Point3& other) const {
        return Point3(x + other.x, y + other.y, z + other.z);
    }

    Point3 operator-(const Point3& other) const {
        return Point3(x - other.x, y - other.y, z - other.z);
    }

    Point3 operator*(float scalar) const {
        return Point3(x * scalar, y * scalar, z * scalar);
    }

    // Векторное произведение
    Point3 cross(const Point3& other) const {
        return Point3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
        );
    }

    // Скалярное произведение
    float dot(const Point3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Нормализация вектора
    Point3 normalize() const {
        float len = std::sqrt(x * x + y * y + z * z);
        if (len > 0)
            return Point3(x / len, y / len, z / len);
        return Point3();
    }
};

// Матрица 4x4 для трансформаций
struct Matrix4x4 {
    float m[4][4];

    Matrix4x4() {
        // Инициализация единичной матрицей
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // Умножение матриц
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k)
                    result.m[i][j] += m[i][k] * other.m[k][j];
            }
        return result;
    }

    // Умножение матрицы на вектор
    Point3 operator*(const Point3& v) const {
        float x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
        float y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
        float z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
        float w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3];

        if (w != 0.0f) {
            x /= w;
            y /= w;
            z /= w;
        }

        return Point3(x, y, z);
    }

    // Создание матрицы перспективной проекции
    static Matrix4x4 perspective(float fov, float aspect, float near, float far) {
        Matrix4x4 result;
        float tanHalfFov = std::tan(fov / 2);

        result.m[0][0] = 1 / (aspect * tanHalfFov);
        result.m[1][1] = 1 / tanHalfFov;
        result.m[2][2] = -(far + near) / (far - near);
        result.m[2][3] = -1;
        result.m[3][2] = -(2 * far * near) / (far - near);
        result.m[3][3] = 0;

        return result;
    }

    // Создание матрицы просмотра (камера)
    static Matrix4x4 lookAt(const Point3& eye, const Point3& center, const Point3& up) {
        Point3 f = (center - eye).normalize();
        Point3 s = f.cross(up).normalize();
        Point3 u = s.cross(f);

        Matrix4x4 result;
        result.m[0][0] = s.x;
        result.m[0][1] = s.y;
        result.m[0][2] = s.z;
        result.m[0][3] = -s.dot(eye);

        result.m[1][0] = u.x;
        result.m[1][1] = u.y;
        result.m[1][2] = u.z;
        result.m[1][3] = -u.dot(eye);

        result.m[2][0] = -f.x;
        result.m[2][1] = -f.y;
        result.m[2][2] = -f.z;
        result.m[2][3] = f.dot(eye);

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    static Matrix4x4 translate(const Point3& t) {
        Matrix4x4 result;
        result.m[3][0] = t.x;
        result.m[3][1] = t.y;
        result.m[3][2] = t.z;

        return result;
    }

    static Matrix4x4 scale(const Point3& s) {

        Matrix4x4 result;
        result.m[0][0] = s.x;
        result.m[1][1] = s.y;
        result.m[2][2] = s.z;

        return result;
    }

    static Matrix4x4 rotation(float angle, const Point3& axis) {
        Matrix4x4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        float t = 1 - c;

        Point3 a = axis.normalize();

        result.m[0][0] = c + a.x * a.x * t;
        result.m[0][1] = a.x * a.y * t + a.z * s;
        result.m[0][2] = a.x * a.z * t - a.y * s;
        result.m[0][3] = 0.0f;

        result.m[1][0] = a.y * a.x * t - a.z * s;
        result.m[1][1] = c + a.y * a.y * t;
        result.m[1][2] = a.y * a.z * t + a.x * s;
        result.m[1][3] = 0.0f;

        result.m[2][0] = a.z * a.x * t + a.y * s;
        result.m[2][1] = a.z * a.y * t - a.x * s;
        result.m[2][2] = c + a.z * a.z * t;
        result.m[2][3] = 0.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }
};

// Структура полигона
struct Polygon3 {
    std::vector<int> vertex_indices;
};

// Меш
struct Mesh {
    std::vector<Point3> vertices;
    std::vector<Polygon3> polygons;
    std::vector<unsigned int> indices; // Индексы для отрисовки
};

// Функция для создания тетраэдра
Mesh createTetrahedron() {
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

    // Создание списка индексов
    mesh.indices.clear();
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

// Функция для создания гексаэдра (куба)
Mesh createHexahedron() {
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
            {{0, 1, 2, 3}}, // Задняя грань
            {{4, 5, 6, 7}}, // Передняя грань
            {{0, 1, 5, 4}}, // Нижняя грань
            {{2, 3, 7, 6}}, // Верхняя грань
            {{0, 3, 7, 4}}, // Левая грань
            {{1, 2, 6, 5}}  // Правая грань
    };

    // Создание списка индексов
    mesh.indices.clear();
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

// Функция для создания октаэдра
Mesh createOctahedron() {
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

    // Создание списка индексов
    mesh.indices.clear();
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

// Функция для создания икосаэдра
Mesh createIcosahedron() {
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

    // Нормализация вершин для единичной сферы
    for (auto& v : mesh.vertices) {
        v = v.normalize();
    }

    // Создание списка индексов
    mesh.indices.clear();
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

// Функция для создания додекаэдра
Mesh createDodecahedron() {
    Mesh mesh;
    const float phi = (1.0 + std::sqrt(5.0)) / 2.0; // Золотое сечение
    const float a = 1.0f;
    const float b = 1.0f / phi;
    const float c = 2.0f - phi;

    // Координаты вершин додекаэдра
    mesh.vertices = {
            { c,  0,  a},   // 1
            {-c,  0,  a},   // 2
            {-b,  b,  b},   // 3
            { 0,  a,  c},   // 4
            { b,  b,  b},   // 5
            { b, -b,  b},   // 6
            { 0, -a,  c},   // 7
            {-b, -b,  b},   // 8
            { c,  0, -a},   // 9
            {-c,  0, -a},   // 10
            {-b, -b, -b},   // 11
            { 0, -a, -c},   // 12
            { b, -b, -b},   // 13
            { b,  b, -b},   // 14
            { 0,  a, -c},   // 15
            {-b,  b, -b},   // 16
            { a,  c,  0},   // 17
            {-a,  c,  0},   // 18
            {-a, -c,  0},   // 19
            { a, -c,  0}    // 20
    };

    // Нормализация вершин
    for (auto& v : mesh.vertices) {
        v = v.normalize();
    }

    // Грани додекаэдра (каждая грань - пятиугольник)
    mesh.polygons = {
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

    // Создание списка индексов
    mesh.indices.clear();
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

// Функция компиляции шейдера
GLuint CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src_cstr = source.c_str();
    glShaderSource(shader, 1, &src_cstr, nullptr);
    glCompileShader(shader);

    // Проверка на ошибки компиляции
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Ошибка компиляции шейдера: " << infoLog << std::endl;
    }
    return shader;
}

// Глобальные переменные для управления камерой
double lastX, lastY;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

Point3 cameraPos(0.0f, 0.0f, 3.0f);
Point3 cameraFront(0.0f, 0.0f, -1.0f);
Point3 cameraUp(0.0f, 1.0f, 0.0f);
float nearPlaneDistance = 0.7f; // Начальное значение ближней плоскости
bool isCursorFree = false; // Переменная состояния курсора

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

    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos); // обратите внимание на обратный порядок
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Ограничение угла обзора по вертикали
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Обновление направления камеры
    Point3 front;
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
        isCursorFree = true;
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

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Настройка GLFW для использования современного OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Для Mac OS X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lemotech 3D", nullptr, nullptr);
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

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Не удалось инициализировать GLAD" << std::endl;
        return -1;
    }

    setup_imgui(window);
    // Установка колбэка для мыши

    // Задание размера окна просмотра
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

    // Включение глубинного теста
    glEnable(GL_DEPTH_TEST);

    // Переменная для текущего меша
    Mesh mesh = createDodecahedron();

    // Создание списка индексов
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
        }
    }

    // Создание VBO и VAO
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Привязка VAO
    glBindVertexArray(VAO);

    // Вершинный буфер
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Point3), &mesh.vertices[0], GL_STATIC_DRAW);

    // Индексный буфер
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

    // Настройка вершинных атрибутов
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point3), (void*)0);
    glEnableVertexAttribArray(0);

    // Отвязываем VAO
    glBindVertexArray(0);

    // Загрузка и компиляция шейдеров
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
        out vec4 FragColor;

        void main() {
            FragColor = vec4(1.0); // Белый цвет
        }
    )";

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Создание шейдерной программы
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Проверка на ошибки линковки
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Ошибка линковки шейдерной программы: " << infoLog << std::endl;
    }

    // Удаление шейдеров, они больше не нужны
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Основной цикл
    bool is_tools_shown = true;
    static int currentPolyhedron = 4;
    const std::map<std::string, bool> polyhedronNames = {{"Tetrahedron", false}, {"Hexahedron", false}, {"Octahedron", false}, {"Icosahedron", false},
                                                      {"Dodecahedron", false} };
    while (!glfwWindowShouldClose(window)) {
        // Очистка буфера цвета и глубины
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Обработка событий
        glfwPollEvents();


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
                if (ImGui::MenuItem("Tools", NULL, is_tools_shown)) {
                    is_tools_shown = !is_tools_shown;
                }
                if (ImGui::MenuItem("Tetrahedron", NULL, currentPolyhedron == 0)) {
                    currentPolyhedron = 0;

                }
                if (ImGui::MenuItem("Hexahedron", NULL, currentPolyhedron == 1)) {
                    currentPolyhedron = 1;

                }
                if (ImGui::MenuItem("Octahedron", NULL, currentPolyhedron == 2)) {
                    currentPolyhedron = 2;

                }
                if (ImGui::MenuItem("Icosahedron", NULL, currentPolyhedron == 3)) {
                    currentPolyhedron = 3;

                }
                if (ImGui::MenuItem("Dodecahedron", NULL, currentPolyhedron == 4)) {
                    currentPolyhedron = 4;

                }
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
            // Начало окна ImGui
            ImGui::Begin("Affine Transformations", &is_tools_shown);

            // Смещение
            ImGui::Text("Translation");
            ImGui::SliderFloat("X", &translation[0], -5.0f, 5.0f);
            ImGui::SliderFloat("Y", &translation[1], -5.0f, 5.0f);
            ImGui::SliderFloat("Z", &translation[2], -5.0f, 5.0f);

            ImGui::Separator();

            // Вращение
            ImGui::Text("Rotation (degrees)");
            ImGui::SliderFloat("Pitch", &rotation[0], -180.0f, 180.0f);
            ImGui::SliderFloat("Yaw", &rotation[1], -180.0f, 180.0f);
            ImGui::SliderFloat("Roll", &rotation[2], -180.0f, 180.0f);

            ImGui::Separator();

            // Масштабирование
            ImGui::Text("Scaling");
            ImGui::SliderFloat("Scale X", &scaling[0], 0.1f, 5.0f);
            ImGui::SliderFloat("Scale Y", &scaling[1], 0.1f, 5.0f);
            ImGui::SliderFloat("Scale Z", &scaling[2], 0.1f, 5.0f);

            // Конец окна ImGui
            ImGui::End();
        }




        // Настройка матриц проекции и вида
        Matrix4x4 projection = Matrix4x4::perspective(45.0f * M_PI / 180.0f, (float)screenWidth / screenHeight, nearPlaneDistance, 100.0f);
        Matrix4x4 view = Matrix4x4::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Создание матрицы модели и применение аффинных преобразований
        Matrix4x4 model;

        // Применение масштабирования
        model = Matrix4x4::scale(Point3(scaling[0], scaling[1], scaling[2])) * model;

        // Применение вращения (последовательно вокруг осей X, Y, Z)
        model = Matrix4x4::rotation(rotation[0] * M_PI / 180.0f, Point3(1.0f, 0.0f, 0.0f)) * model;
        model = Matrix4x4::rotation(rotation[1] * M_PI / 180.0f, Point3(0.0f, 1.0f, 0.0f)) * model;
        model = Matrix4x4::rotation(rotation[2] * M_PI / 180.0f, Point3(0.0f, 0.0f, 1.0f)) * model;

        // Применение смещения
        model = Matrix4x4::translate(Point3(translation[0], translation[1], translation[2])) * model;

        // Формирование общей матрицы MVP
        Matrix4x4 mvp = model * projection * view;
        ImGui::Render();


        // Использование шейдерной программы
        glUseProgram(shaderProgram);

        // Передача матрицы MVP в шейдер
        GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp.m[0][0]);

        // Привязка VAO и отрисовка меша
        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Обновление окна
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Очистка ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}