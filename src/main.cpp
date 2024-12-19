#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <string>
#include <fstream>
#include <sstream>
#include <map>

GLuint CompileShader(GLenum type, const std::string& source);
struct Point3 {
    float x, y, z;

    Point3() : x(0.0f), y(0.0f), z(0.0f) {}
    Point3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    // Операции над векторами
    Point3 operator+(const Point3& other) const {
        return Point3(x + other.x, y + other.y, z + other.z);
    }

    Point3 operator-(const Point3& other) const {
        return Point3(x - other.x, y - other.y, z - other.z);
    }

    Point3 operator*(float scalar) const {
        return Point3(x * scalar, y * scalar, z * scalar);
    }

    Point3 cross(const Point3& other) const {
        return Point3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
        );
    }

    float dot(const Point3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Point3 normalize() const {
        float length = std::sqrt(x * x + y * y + z * z);
        if (length == 0.0f) return Point3(0.0f, 0.0f, 0.0f);
        return Point3(x / length, y / length, z / length);
    }
};

double lastX, lastY;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

Point3 cameraPos(0.0f, 0.0f, 3.0f);
Point3 cameraFront(0.0f, 0.0f, -1.0f);
Point3 cameraUp(0.0f, 1.0f, 0.0f);

struct Matrix4x4 { // Матрица 4x4 для трансформаций
    float m[16];

    Matrix4x4() {
        for (int i = 0; i < 16; ++i)
            m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    Matrix4x4 operator*(const Matrix4x4& other) const { // Умножение матриц
        Matrix4x4 result;
        for (int row = 0; row < 4; ++row)
            for (int col = 0; col < 4; ++col)
                result.m[col * 4 + row] = m[0 * 4 + row] * other.m[col * 4 + 0] +
                                          m[1 * 4 + row] * other.m[col * 4 + 1] +
                                          m[2 * 4 + row] * other.m[col * 4 + 2] +
                                          m[3 * 4 + row] * other.m[col * 4 + 3];
        return result;
    }

    Point3 operator*(const Point3& v) const { // Умножение матрицы на вектор
        float x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12];
        float y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13];
        float z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14];
        float w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];

        if (w != 0.0f) {
            x /= w;
            y /= w;
            z /= w;
        }

        return { x, y, z };
    }

    static Matrix4x4 perspective(float fov, float aspect, float near1, float far1) { // Матрица перспективной проекции
        Matrix4x4 result;
        float tanHalfFov = std::tan(fov / 2);

        result.m[0] = 1 / (aspect * tanHalfFov);
        result.m[5] = 1 / tanHalfFov;
        result.m[10] = -(far1 + near1) / (far1 - near1);
        result.m[11] = -1.0f;
        result.m[14] = -(2 * far1 * near1) / (far1 - near1);
        result.m[15] = 0.0f;

        return result;
    }

    static Matrix4x4 orthographic(float left, float right, float bottom, float top, float near1, float far1) {
        Matrix4x4 result;
        result.m[0] = 2.0f / (right - left);
        result.m[5] = 2.0f / (top - bottom);
        result.m[10] = -2.0f / (far1 - near1);
        result.m[12] = -(right + left) / (right - left);
        result.m[13] = -(top + bottom) / (top - bottom);
        result.m[14] = -(far1 + near1) / (far1 - near1);
        result.m[15] = 1.0f;
        return result;
    }

    static Matrix4x4 lookAt(const Point3& eye, const Point3& center, const Point3& up) { // Матрица просмотра (камера)
        Point3 f = (center - eye).normalize();
        Point3 s = f.cross(up).normalize();
        Point3 u = s.cross(f);

        Matrix4x4 result;
        result.m[0] = s.x;
        result.m[4] = s.y;
        result.m[8] = s.z;
        result.m[12] = -s.dot(eye);

        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;
        result.m[13] = -u.dot(eye);

        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;
        result.m[14] = f.dot(eye);

        result.m[3] = 0.0f;
        result.m[7] = 0.0f;
        result.m[11] = 0.0f;
        result.m[15] = 1.0f;

        return result;
    }

    static Matrix4x4 translate(const Point3& t) {
        Matrix4x4 result;
        result.m[12] = t.x;
        result.m[13] = t.y;
        result.m[14] = t.z;

        return result;
    }

    static Matrix4x4 scale(const Point3& s) {
        Matrix4x4 result;
        result.m[0] = s.x;
        result.m[5] = s.y;
        result.m[10] = s.z;

        return result;
    }

    static Matrix4x4 rotation(float angle, const Point3& axis) {
        Matrix4x4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        float t = 1 - c;

        Point3 a = axis.normalize();

        result.m[0] = c + a.x * a.x * t;
        result.m[4] = a.x * a.y * t - a.z * s;
        result.m[8] = a.x * a.z * t + a.y * s;

        result.m[1] = a.y * a.x * t + a.z * s;
        result.m[5] = c + a.y * a.y * t;
        result.m[9] = a.y * a.z * t - a.x * s;

        result.m[2] = a.z * a.x * t - a.y * s;
        result.m[6] = a.z * a.y * t + a.x * s;
        result.m[10] = c + a.z * a.z * t;

        result.m[15] = 1.0f;

        return result;
    }
};
struct Material {
    // Base color
    Point3 color;

    // Texture ID (0 if no texture)
    GLuint textureID;

    // Constructor
    Material(const Point3& col = Point3(1.0f, 1.0f, 1.0f), GLuint texID = 0)
            : color(col), textureID(texID) {}
};
class Transform {
public:
    Point3 position;
    Point3 rotation;
    Point3 scale;

    Transform() : position(0.0f, 0.0f, 0.0f),
                  rotation(0.0f, 0.0f, 0.0f),
                  scale(1.0f, 1.0f, 1.0f) {}

    Matrix4x4 getMatrix() const {
        float rx = rotation.x * M_PI / 180.0f;
        float ry = rotation.y * M_PI / 180.0f;
        float rz = rotation.z * M_PI / 180.0f;

        Matrix4x4 model = Matrix4x4::translate(position) *
                          Matrix4x4::rotation(rx, Point3(1.0f, 0.0f, 0.0f)) *
                          Matrix4x4::rotation(ry, Point3(0.0f, 1.0f, 0.0f)) *
                          Matrix4x4::rotation(rz, Point3(0.0f, 0.0f, 1.0f)) *
                          Matrix4x4::scale(scale);
        return model;
    }
};
struct Vertex {
    unsigned int posIndex;
    unsigned int texIndex;
    unsigned int normIndex;

    bool operator<(const Vertex& other) const {
        return std::tie(posIndex, texIndex, normIndex) < std::tie(other.posIndex, other.texIndex, other.normIndex);
    }
};
struct Mesh {
    std::vector<Point3> vertices;          // Вершины
    std::vector<Point3> normals;           // Нормали
    std::vector<float> texCoords;          // Текстурные координаты (u, v)
    std::vector<Point3> colors;            // Цвета вершин
    std::vector<unsigned int> indices;     // Индексы граней
};
Mesh loadOBJ(const std::string& path);
class Model {
public:
    Mesh mesh;
    Transform transform;

    GLuint VAO, VBO, EBO;

    bool isLight = false;
    Point3 emissiveColor = Point3(1.0f, 1.0f, 1.0f);

    Material material;

    // Constructors
    Model(const Mesh& m, const Material& mat = Material()) : mesh(m), material(mat) {
        setupBuffers();
    }

    Model(const std::string& objPath, const Material& mat = Material()) : mesh(loadOBJ(objPath)), material(mat) {
        setupBuffers();
    }

    ~Model() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    // In Model::draw()
    void draw(GLuint shaderProgram, const Matrix4x4& view, const Matrix4x4& projection) {
        Matrix4x4 modelMatrix = transform.getMatrix();
        Matrix4x4 mvp = projection * view * modelMatrix;

        GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
        GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
        GLint isLightLoc = glGetUniformLocation(shaderProgram, "isLight");
        GLint emissiveColorLoc = glGetUniformLocation(shaderProgram, "emissiveColor");
        GLint useVertexColorLoc = glGetUniformLocation(shaderProgram, "useVertexColor");
        GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        GLint hasTextureLoc = glGetUniformLocation(shaderProgram, "hasTexture");

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.m);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMatrix.m);

        glUniform1i(isLightLoc, isLight ? 1 : 0);
        glUniform3f(emissiveColorLoc, emissiveColor.x, emissiveColor.y, emissiveColor.z);

        // Set whether to use vertex color
        bool hasVertexColors = !mesh.colors.empty();
        glUniform1i(useVertexColorLoc, hasVertexColors ? 1 : 0);

        // Set object color
        glUniform3f(objectColorLoc, material.color.x, material.color.y, material.color.z);

        // Handle texture
        if (material.textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.textureID);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
            glUniform1i(hasTextureLoc, 1);
        } else {
            glUniform1i(hasTextureLoc, 0);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void setupBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // In Model::setupBuffers()
        std::vector<float> interleavedData;
        interleavedData.reserve(mesh.vertices.size() * 11); // 3 pos + 3 normals + 2 texCoords + 3 colors

        for (size_t i = 0; i < mesh.vertices.size(); ++i) {
            // Position
            interleavedData.push_back(mesh.vertices[i].x);
            interleavedData.push_back(mesh.vertices[i].y);
            interleavedData.push_back(mesh.vertices[i].z);

            // Normal
            if (!mesh.normals.empty()) {
                interleavedData.push_back(mesh.normals[i].x);
                interleavedData.push_back(mesh.normals[i].y);
                interleavedData.push_back(mesh.normals[i].z);
            } else {
                interleavedData.push_back(0.0f);
                interleavedData.push_back(0.0f);
                interleavedData.push_back(0.0f);
            }

            // Texture Coordinates
            if (!mesh.texCoords.empty() && (i * 2 + 1) < mesh.texCoords.size()) {
                interleavedData.push_back(mesh.texCoords[i * 2]);
                interleavedData.push_back(mesh.texCoords[i * 2 + 1]);
            } else {
                interleavedData.push_back(0.0f);
                interleavedData.push_back(0.0f);
            }

            // Colors
            if (!mesh.colors.empty()) {
                interleavedData.push_back(mesh.colors[i].x);
                interleavedData.push_back(mesh.colors[i].y);
                interleavedData.push_back(mesh.colors[i].z);
            } else {
                interleavedData.push_back(1.0f);
                interleavedData.push_back(1.0f);
                interleavedData.push_back(1.0f);
            }
        }
        // Передача данных в VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

        // Передача индексов в EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        // Настройка атрибутов вершин
        // Позиция
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Нормаль
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Текстурные координаты
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // Цвета
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }
};
class Scene {
public:
    std::vector<std::shared_ptr<Model>> models;

    void addModel(const std::shared_ptr<Model>& model) {
        models.push_back(model);
    }

    void drawAll(GLuint shaderProgram, const Matrix4x4& view, const Matrix4x4& projection) {
        for (auto& model : models) {
            if (model->material.textureID != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, model->material.textureID);
                glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
                glUniform1i(glGetUniformLocation(shaderProgram, "hasTexture"), 1);
            } else {
                glUniform1i(glGetUniformLocation(shaderProgram, "hasTexture"), 0);
            }

            // Set object color
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), model->material.color.x, model->material.color.y, model->material.color.z);
            model->draw(shaderProgram, view, projection);
        }
    }
};

void processInput(GLFWwindow* window);
void processCursorToggle(GLFWwindow* window);
void setup_style(ImGuiStyle& style, ImGuiIO& io);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow* window, unsigned int codepoint);
void setup_imgui(GLFWwindow* window);
void show_tools(Scene& scene);
GLuint LoadTexture(const char* path);

Scene scene;
int main() {
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Lemotech 3D", nullptr, nullptr);
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Не удалось инициализировать GLAD" << std::endl;
        return -1;
    }

    setup_imgui(window);

    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);

    // Vertex Shader
        std::string vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec2 aTexCoord;
        layout(location = 3) in vec3 aColor; // New attribute for color

        uniform mat4 uMVP;
        uniform mat4 uModel;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;
        out vec3 VertexColor; // Pass to fragment shader

        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            FragPos = vec3(uModel * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(uModel))) * aNormal;
            TexCoord = aTexCoord;
            VertexColor = aColor; // Assign vertex color
        }
    )";

    // Fragment Shader
    std::string fragmentShaderSource = R"(
        #version 410 core

        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        in vec3 VertexColor;

        out vec4 FragColor;

        uniform vec3 lightPos;
        uniform vec3 viewPos;
        uniform sampler2D texture1;

        uniform bool isLight;
        uniform vec3 emissiveColor;

        uniform bool useVertexColor;
        uniform bool hasTexture;
        uniform vec3 objectColor; // New uniform for object color

        void main() {
            if (isLight) {
                FragColor = vec4(emissiveColor, 1.0);
                return;
            }

            // Ambient lighting
            vec3 ambient = 0.1 * vec3(1.0, 1.0, 1.0);

            // Diffuse lighting
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

            // Specular lighting
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);

            // Texture
            vec3 textureColor = hasTexture ? texture(texture1, TexCoord).rgb : vec3(1.0, 1.0, 1.0);

            // Object color
            vec3 finalColor = objectColor;
            if(useVertexColor) {
                finalColor *= VertexColor;
            }

            // Combine color with texture
            finalColor *= textureColor;

            vec3 result = (ambient + diffuse + specular) * finalColor;
            FragColor = vec4(result, 1.0);
        }
    )";

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    {
        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Ошибка линковки шейдерной программы:\n" << infoLog << std::endl;
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT); // Отсекаем передние грани
    glFrontFace(GL_CCW);

    // Get uniform locations
    GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    GLint useVertexColorLoc = glGetUniformLocation(shaderProgram, "useVertexColor");

    GLuint texture = LoadTexture("../assets/room.png");
    if (texture == 0) {
        std::cerr << "Текстура не загружена!" << std::endl;
    }

    std::shared_ptr<Model> room = std::make_shared<Model>("../assets/room.obj");
    const Point3 faceColors[6] = {
            Point3(1.0f, 0.149f, 0.0f), // Left - Red
            Point3(0.921f, 0.921f, 0.921f),
            Point3(0.0f, 0.59f, 1.0f), // Right - Blue
            Point3(0.921f, 0.921f, 0.921f),
            Point3(0.921f, 0.921f, 0.921f),
            Point3(0.921f, 0.921f, 0.921f)
    };

    // Предполагается, что каждая грань состоит из 2 треугольников (6 вершин)
    size_t verticesPerFace = 6;
    for (size_t face = 0; face < 6; ++face) {
        for (size_t vert = 0; vert < verticesPerFace; ++vert) {
            size_t index = face * verticesPerFace + vert;
            if (index < room->mesh.colors.size()) {
                room->mesh.colors[index] = faceColors[face];
            }
        }
    }
    room->setupBuffers();

    room->material.color = Point3(1.0f, 1.0f, 1.0f);
    room->transform.position = Point3(0.0f, 2.35f, 1.0f);
    room->transform.scale = Point3(5.0f, 3.3f, 14.0f);

    scene.addModel(room);

    std::shared_ptr<Model> cubeItem = std::make_shared<Model>("../assets/cube.obj");
    cubeItem->transform.position = Point3(1.0f, 0.0f, -3.6f);
    cubeItem->transform.rotation = Point3(0.0f, 8.0f, 0.0f);
    cubeItem->transform.scale = Point3(0.5f, 0.95f, 0.5f);
    scene.addModel(cubeItem);

    std::shared_ptr<Model> cubeItem2 = std::make_shared<Model>("../assets/cube.obj");
    cubeItem2->transform.position = Point3(-1.0f, -0.45f, -2.6f);
    cubeItem2->transform.rotation = Point3(0.0f, -4.0f, 0.0f);
    cubeItem2->transform.scale = Point3(0.5f, 0.5f, 0.5f);
    scene.addModel(cubeItem2);

    std::shared_ptr<Model> sphereItem = std::make_shared<Model>("../assets/sphere.obj");
    sphereItem->transform.position = Point3(-3.5f, 0.0f, -3.5f);
    scene.addModel(sphereItem);

    std::shared_ptr<Model> lightPanel = std::make_shared<Model>("../assets/plane.obj");

    lightPanel->transform.position = Point3(0.0f, 5.6f, -5.0f);
    lightPanel->transform.scale = Point3(2.0f, 1.0f, 2.0f);
    lightPanel->transform.rotation = Point3(180.0f, 0.0f, 0.0f);

    lightPanel->isLight = true;
    lightPanel->emissiveColor = Point3(1.0f, 1.0f, 0.8f);
    scene.addModel(lightPanel);

    Point3 lightPosInWorld = lightPanel->transform.position;

    while (!glfwWindowShouldClose(window)) {
        static auto lastFrameTime = std::chrono::high_resolution_clock::now();
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;

        glfwPollEvents();
        processCursorToggle(window);
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        lightPosInWorld = lightPanel->transform.position;
        glUniform3f(lightPosLoc, lightPosInWorld.x, lightPosInWorld.y, lightPosInWorld.z);
        glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        Matrix4x4 view = Matrix4x4::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        int screenWidth, screenHeight;
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        float aspectRatio = static_cast<float>(screenWidth) / screenHeight;
        Matrix4x4 projection = Matrix4x4::perspective(45.0f * M_PI / 180.0f, aspectRatio, 0.1f, 100.0f);

        scene.drawAll(shaderProgram, view, projection);

        show_tools(scene);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save")) {
                    std::string savePath = "../assets/saved_cube.obj";
                    std::cout << "Cube state saved to: " << savePath << std::endl;
                }
                if (ImGui::MenuItem("Load Model")) {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

GLuint CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src_cstr = source.c_str();
    glShaderSource(shader, 1, &src_cstr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Ошибка компиляции шейдера:\n" << infoLog << std::endl;
    }
    return shader;
}
void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * 0.016f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos = cameraPos + cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos = cameraPos - cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos = cameraPos - cameraFront.cross(cameraUp).normalize() * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos = cameraPos + cameraFront.cross(cameraUp).normalize() * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos = cameraPos + cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos = cameraPos - cameraUp * cameraSpeed;
}
void processCursorToggle(GLFWwindow* window) {
    static bool pressed_flag = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!pressed_flag) {
            pressed_flag = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
        }
    } else {
        if (pressed_flag) {
            pressed_flag = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(lastY - ypos);
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        float yawRad = yaw * static_cast<float>(M_PI) / 180.0f;
        float pitchRad = pitch * static_cast<float>(M_PI) / 180.0f;

        Point3 front;
        front.x = std::cos(yawRad) * std::cos(pitchRad);
        front.y = std::sin(pitchRad);
        front.z = std::sin(yawRad) * std::cos(pitchRad);
        cameraFront = front.normalize();

        Point3 worldUp(0.0f, 1.0f, 0.0f);
        Point3 cameraRight = cameraFront.cross(worldUp).normalize();
        cameraUp = cameraRight.cross(cameraFront).normalize();
    }

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
    style.Colors[ImGuiCol_Button] = buttonColor;
    style.Colors[ImGuiCol_ButtonHovered] = buttonHoveredColor;
    style.Colors[ImGuiCol_ButtonActive] = buttonActiveColor;
    style.Colors[ImGuiCol_Border] = borderColor;
    style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
}
void show_tools(Scene& scene) {
    ImGui::Begin("Tools");

    for (size_t i = 0; i < scene.models.size(); ++i) {
        std::string header = "Object " + std::to_string(i + 1);
        if (ImGui::CollapsingHeader(header.c_str())) {
            ImGui::Text("Translation");
            ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), (float*)&scene.models[i]->transform.position, 0.1f);

            ImGui::Text("Rotation (degrees)");
            ImGui::DragFloat3(("Rotation##" + std::to_string(i)).c_str(), (float*)&scene.models[i]->transform.rotation, 1.0f);

            ImGui::Text("Scale");
            ImGui::DragFloat3(("Scale##" + std::to_string(i)).c_str(), (float*)&scene.models[i]->transform.scale, 0.1f);

            // Material Controls
            ImGui::Separator();
            ImGui::Text("Material");

            // Color Picker
            ImGui::ColorEdit3(("Color##" + std::to_string(i)).c_str(), (float*)&scene.models[i]->material.color);

            // Texture Selector
            if (ImGui::Button(("Load Texture##" + std::to_string(i)).c_str())) {
                // Implement texture loading dialog or specify texture paths
                // For simplicity, assign a predefined texture
                std::string texturePath = "../assets/uss.png";
                GLuint texID = LoadTexture(texturePath.c_str());
                if (texID != 0) {
                    scene.models[i]->material.textureID = texID;
                }
            }

            // Option to Remove Texture
            if (ImGui::Button(("Remove Texture##" + std::to_string(i)).c_str())) {
                scene.models[i]->material.textureID = 0;
            }
        }
    }

    ImGui::End();
}
GLuint LoadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
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
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

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
Mesh loadOBJ(const std::string& path) {
    Mesh mesh;
    std::vector<Point3> temp_vertices;
    std::vector<Point3> temp_normals;
    std::vector<float> temp_texCoords;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << path << std::endl;
        return mesh;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            temp_vertices.emplace_back(x, y, z);
        }
        else if (prefix == "vt") {
            float u, v;
            ss >> u >> v;
            temp_texCoords.push_back(u);
            temp_texCoords.push_back(v);
        }
        else if (prefix == "vn") {
            float x, y, z;
            ss >> x >> y >> z;
            temp_normals.emplace_back(x, y, z);
        }
        else if (prefix == "f") {
            std::string vertexStr;
            std::vector<Vertex> faceVertices;
            while (ss >> vertexStr) {
                std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
                std::stringstream vertexSS(vertexStr);
                unsigned int vIdx, tIdx, nIdx;
                vIdx = tIdx = nIdx = 0;
                vertexSS >> vIdx;
                if (!vertexSS.eof()) vertexSS >> tIdx;
                if (!vertexSS.eof()) vertexSS >> nIdx;

                faceVertices.push_back(Vertex{ vIdx, tIdx, nIdx });
            }

            // Триангуляция полигона фан-триангуляцией
            for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                for (size_t j = 0; j < 3; ++j) {
                    unsigned int vertexIndex = (j == 0) ? 0 : (j == 1) ? i : (j == 2) ? (i + 1) : 0;
                    Vertex currentVertex = faceVertices[vertexIndex];
                    // Добавляем позицию
                    mesh.vertices.push_back(temp_vertices[currentVertex.posIndex - 1]);
                    // Добавляем текстурные координаты
                    if (currentVertex.texIndex > 0 && (currentVertex.texIndex - 1) * 2 + 1 < temp_texCoords.size()) {
                        mesh.texCoords.push_back(temp_texCoords[(currentVertex.texIndex - 1) * 2]);
                        mesh.texCoords.push_back(temp_texCoords[(currentVertex.texIndex - 1) * 2 + 1]);
                    }
                    else {
                        mesh.texCoords.push_back(0.0f);
                        mesh.texCoords.push_back(0.0f);
                    }
                    // Добавляем нормали
                    if (currentVertex.normIndex > 0 && currentVertex.normIndex - 1 < temp_normals.size()) {
                        mesh.normals.push_back(temp_normals[currentVertex.normIndex - 1]);
                    }
                    else {
                        mesh.normals.emplace_back(0.0f, 0.0f, 0.0f);
                    }

                    // Присваиваем цвет (пока белый, позже изменим)
                    mesh.colors.emplace_back(1.0f, 1.0f, 1.0f);
                    // Добавляем индекс новой вершины
                    mesh.indices.push_back(static_cast<unsigned int>(mesh.vertices.size() - 1));
                }
            }
        }
        // Игнорируем остальные префиксы
    }

    file.close();
    return mesh;
}
