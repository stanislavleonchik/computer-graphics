#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>

struct Point {
    float x, y;
};

std::vector<Point> points;
std::vector<Point> hull;
bool computeHull = false;

unsigned int pointVAO, pointVBO;
unsigned int hullVAO, hullVBO;
unsigned int shaderProgram;

int screenWidth = 800;
int screenHeight = 600;

// Функция для вычисления векторного произведения
float cross(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x)*(B.y - O.y) - (A.y - O.y)*(B.x - O.x);
}

// Алгоритм Грэхема для построения выпуклой оболочки
void grahamScan() {
    if (points.size() < 3) return;

    // Находим точку с минимальной y-координатой
    Point p0 = *std::min_element(points.begin(), points.end(),
                                 [](const Point& a, const Point& b) {
                                     return a.y < b.y || (a.y == b.y && a.x < b.x);
                                 });

    // Сортируем точки по полярному углу относительно p0
    std::sort(points.begin(), points.end(),
              [p0](const Point& a, const Point& b) {
                  float thetaA = atan2(a.y - p0.y, a.x - p0.x);
                  float thetaB = atan2(b.y - p0.y, b.x - p0.x);
                  return thetaA < thetaB;
              });

    // Строим оболочку
    hull.clear();
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    for (size_t i = 2; i < points.size(); ++i) {
        while (hull.size() >= 2 &&
               cross(hull[hull.size()-2], hull[hull.size()-1],
                     points[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }
}

// Источники шейдеров
const char* vertexShaderSource = R"glsl(
    #version 410 core
    layout (location = 0) in vec2 aPos;

    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 410 core
    out vec4 FragColor;

    uniform vec3 color;

    void main() {
        FragColor = vec4(color, 1.0);
    }
)glsl";

// Функции для компиляции шейдеров и создания шейдерной программы
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Проверяем на ошибки компиляции
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Ошибка компиляции шейдера:\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Проверяем на ошибки линковки
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Ошибка линковки шейдерной программы:\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

// Колбэк для изменения размера окна
void framebuffer_size_callback(GLFWwindow* window,
                               int width, int height) {
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

// Обработка ввода мыши
void mouse_button_callback(GLFWwindow* window, int button,
                           int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT &&
        action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float x = (xpos / width) * 2 - 1;
        float y = -((ypos / height) * 2 - 1);
        points.push_back({x, y});

        // Обновляем VBO для точек
        glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), &points[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT &&
        action == GLFW_PRESS) {
        computeHull = true;
    }
}

// Обработка клавиатуры
void processInput(GLFWwindow *window) {
    // Закрываем окно по нажатию ESC
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) return -1;
    // Установка версии OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Создаем окно
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Выпуклая оболочка - Метод Грэхема", NULL, NULL);
    if (!window) {
        std::cout << "Не удалось создать окно GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)
                                  glfwGetProcAddress)) {
        std::cout << "Не удалось инициализировать GLAD" << std::endl;
        return -1;
    }
    // Устанавливаем начальный видовой порт
    glViewport(0, 0, screenWidth, screenHeight);
    // Устанавливаем колбэки
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Компилируем и линкуем шейдеры
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    // Генерируем VAO и VBO для точек
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    // Генерируем VAO и VBO для выпуклой оболочки
    glGenVertexArrays(1, &hullVAO);
    glGenBuffers(1, &hullVBO);
    glBindVertexArray(hullVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hullVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    // Главный цикл
    while (!glfwWindowShouldClose(window)) {
        // Обработка ввода
        processInput(window);
        // Очистка буфера
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Используем шейдерную программу
        glUseProgram(shaderProgram);
        int colorLocation = glGetUniformLocation(shaderProgram, "color");
        // Рисуем точки
        glBindVertexArray(pointVAO);
        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
        glPointSize(5.0f);
        glDrawArrays(GL_POINTS, 0, points.size());
        glBindVertexArray(0);
        // Вычисляем выпуклую оболочку при необходимости
        if (computeHull) {
            grahamScan();
            computeHull = false;
            // Обновляем VBO для оболочки
            glBindBuffer(GL_ARRAY_BUFFER, hullVBO);
            glBufferData(GL_ARRAY_BUFFER, hull.size() * sizeof(Point), &hull[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        // Рисуем выпуклую оболочку
        if (!hull.empty()) {
            glBindVertexArray(hullVAO);
            glUniform3f(colorLocation, 0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_LINE_LOOP, 0, hull.size());
            glBindVertexArray(0);
        }
        // Обновляем окно
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Очистка ресурсов
    glDeleteVertexArrays(1, &pointVAO);
    glDeleteBuffers(1, &pointVBO);
    glDeleteVertexArrays(1, &hullVAO);
    glDeleteBuffers(1, &hullVBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}