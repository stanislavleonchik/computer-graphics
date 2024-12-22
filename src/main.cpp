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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <unordered_map>
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
        std::cerr << "Shader Compilation Error:\n" << infoLog << std::endl;
    }
    return shader;
}
struct ShaderProgram {
    GLuint ID;
    // Cached uniform locations
    GLint uMVP;
    GLint uModel;
    GLint isLight;
    GLint emissiveColor;
    GLint useVertexColor;
    GLint objectColor;
    GLint hasTexture;
    GLint lightPos;
    GLint viewPos;
    GLint texture1;

    ShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc) {
        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        // Check linking errors
        GLint success;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            std::cerr << "Shader Program Linking Error:\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Cache uniform locations
        uMVP = glGetUniformLocation(ID, "uMVP");
        uModel = glGetUniformLocation(ID, "uModel");
        isLight = glGetUniformLocation(ID, "isLight");
        emissiveColor = glGetUniformLocation(ID, "emissiveColor");
        useVertexColor = glGetUniformLocation(ID, "useVertexColor");
        objectColor = glGetUniformLocation(ID, "objectColor");
        hasTexture = glGetUniformLocation(ID, "hasTexture");
        lightPos = glGetUniformLocation(ID, "lightPos");
        viewPos = glGetUniformLocation(ID, "viewPos");
        texture1 = glGetUniformLocation(ID, "texture1");
    }

    void use() const {
        glUseProgram(ID);
    }
};
class TextureManager {
public:
    static GLuint LoadTexture(const std::string& path) {
        if (textures.find(path) != textures.end()) {
            return textures[path];
        }

        GLuint textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
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
            textures[path] = textureID;
            return textureID;
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
            stbi_image_free(data);
            return 0;
        }
    }

    static void Clear() {
        for (auto& [path, id] : textures) {
            glDeleteTextures(1, &id);
        }
        textures.clear();
    }

private:
    static std::unordered_map<std::string, GLuint> textures;
};
std::unordered_map<std::string, GLuint> TextureManager::textures;
struct Mesh {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> colors;
    std::vector<unsigned int> indices;
};
struct Vertex {
    unsigned int posIndex;
    unsigned int texIndex;
    unsigned int normIndex;

    bool operator<(const Vertex& other) const {
        return std::tie(posIndex, texIndex, normIndex) < std::tie(other.posIndex, other.texIndex, other.normIndex);
    }
};
class Transform {
public:
    glm::vec3 position;
    glm::vec3 rotation; // In degrees
    glm::vec3 scale;

    Transform() : position(0.0f), rotation(0.0f), scale(1.0f) {}

    glm::mat4 getMatrix() const {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }
};
struct Material {
    glm::vec3 color;
    GLuint textureID;

    Material(const glm::vec3& col = glm::vec3(1.0f), GLuint texID = 0)
            : color(col), textureID(texID) {}
};
class Model {
public:
    Mesh mesh;
    std::string name;
    Transform transform;
    GLuint VAO, VBO, EBO;
    bool isLight = false;
    glm::vec3 emissiveColor = glm::vec3(1.0f);
    Material material;

    Model(const Mesh& m, const Material& mat = Material()) : mesh(m), material(mat) {
        setupBuffers();
    }

    Model(const std::string& objPath, const Material& mat = Material()) : material(mat) {
        mesh = loadOBJ(objPath);
        name = std::filesystem::path(objPath).stem().string();
        setupBuffers();
    }

    ~Model() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw(const ShaderProgram& shader, const glm::mat4& view, const glm::mat4& projection) const {
        shader.use();

        glm::mat4 modelMatrix = transform.getMatrix();
        glm::mat4 mvp = projection * view * modelMatrix;

        glUniformMatrix4fv(shader.uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(shader.uModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        glUniform1i(shader.isLight, isLight ? 1 : 0);
        glUniform3fv(shader.emissiveColor, 1, glm::value_ptr(emissiveColor));

        bool hasVertexColors = !mesh.colors.empty();
        glUniform1i(shader.useVertexColor, hasVertexColors ? 1 : 0);

        glUniform3fv(shader.objectColor, 1, glm::value_ptr(material.color));

        if (material.textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.textureID);
            glUniform1i(shader.texture1, 0);
            glUniform1i(shader.hasTexture, 1);
        }
        else {
            glUniform1i(shader.hasTexture, 0);
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

        // Interleaved Data: position(3) + normal(3) + texCoords(2) + color(3)
        std::vector<float> interleavedData;
        interleavedData.reserve(mesh.vertices.size() * 11);

        for (size_t i = 0; i < mesh.vertices.size(); ++i) {
            // Position
            interleavedData.push_back(mesh.vertices[i].x);
            interleavedData.push_back(mesh.vertices[i].y);
            interleavedData.push_back(mesh.vertices[i].z);

            // Normal
            interleavedData.push_back(mesh.normals[i].x);
            interleavedData.push_back(mesh.normals[i].y);
            interleavedData.push_back(mesh.normals[i].z);

            // TexCoords
            interleavedData.push_back(mesh.texCoords[i].x);
            interleavedData.push_back(mesh.texCoords[i].y);

            // Color
            interleavedData.push_back(mesh.colors[i].x);
            interleavedData.push_back(mesh.colors[i].y);
            interleavedData.push_back(mesh.colors[i].z);
        }

        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

        // EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        // Vertex Attributes
        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);

        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

        // TexCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));

        // Color
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));

        glBindVertexArray(0);
    }

private:
    // Оптимизированная функция loadOBJ с использованием GLM
    Mesh loadOBJ(const std::string& path) {
        Mesh mesh;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec3> temp_normals;
        std::vector<glm::vec2> temp_texCoords;

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return mesh;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                glm::vec3 vertex;
                ss >> vertex.x >> vertex.y >> vertex.z;
                temp_vertices.emplace_back(vertex);
            }
            else if (prefix == "vt") {
                glm::vec2 tex;
                ss >> tex.x >> tex.y;
                temp_texCoords.emplace_back(tex);
            }
            else if (prefix == "vn") {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                temp_normals.emplace_back(normal);
            }
            else if (prefix == "f") {
                std::string vertexStr;
                std::vector<Vertex> faceVertices;
                while (ss >> vertexStr) {
                    std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
                    std::stringstream vertexSS(vertexStr);
                    Vertex vertex = {0, 0, 0};
                    vertexSS >> vertex.posIndex;
                    if (!(vertexSS >> vertex.texIndex)) vertex.texIndex = 0;
                    if (!(vertexSS >> vertex.normIndex)) vertex.normIndex = 0;
                    faceVertices.push_back(vertex);
                }

                // Fan triangulation
                for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                    std::array<Vertex, 3> tri = {faceVertices[0], faceVertices[i], faceVertices[i + 1]};
                    for (const auto& v : tri) {
                        mesh.vertices.emplace_back(temp_vertices[v.posIndex - 1]);
                        if (v.texIndex > 0 && v.texIndex - 1 < temp_texCoords.size())
                            mesh.texCoords.emplace_back(temp_texCoords[v.texIndex - 1]);
                        else
                            mesh.texCoords.emplace_back(0.0f, 0.0f);

                        if (v.normIndex > 0 && v.normIndex - 1 < temp_normals.size())
                            mesh.normals.emplace_back(temp_normals[v.normIndex - 1]);
                        else
                            mesh.normals.emplace_back(0.0f, 0.0f, 0.0f);

                        mesh.colors.emplace_back(1.0f, 1.0f, 1.0f); // Default white
                        mesh.indices.push_back(static_cast<unsigned int>(mesh.vertices.size() - 1));
                    }
                }
            }
            // Ignore other prefixes
        }

        file.close();
        return mesh;
    }
};
class Scene {
public:
    std::vector<std::shared_ptr<Model>> models;

    void addModel(const std::shared_ptr<Model>& model) {
        models.push_back(model);
    }

    void drawAll(const ShaderProgram& shader, const glm::mat4& view, const glm::mat4& projection) const {
        for (const auto& model : models) {
            model->draw(shader, view, projection);
        }
    }
};

// Global Variables
Scene scene;
glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

double lastX, lastY;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

void processInput(GLFWwindow* window, float deltaTime);
void processCursorToggle(GLFWwindow* window);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow* window, unsigned int codepoint);
void setup_imgui(GLFWwindow* window);
void show_tools(Scene& scene);
void setup_style(ImGuiStyle& style, ImGuiIO& io);

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Настройка контекста OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Создание окна
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Lemotech 3D", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Установка контекста и настройки GLAD
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Настройка ImGui
    setup_imgui(window);

    // Установка колбэков
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);

    // Загрузка шейдеров
    std::string vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec2 aTexCoord;
        layout(location = 3) in vec3 aColor;

        uniform mat4 uMVP;
        uniform mat4 uModel;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;
        out vec3 VertexColor;

        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            FragPos = vec3(uModel * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(uModel))) * aNormal;
            TexCoord = aTexCoord;
            VertexColor = aColor;
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 330 core

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
        uniform vec3 objectColor;

        void main() {
            if (isLight) {
                FragColor = vec4(emissiveColor, 1.0);
                return;
            }

            // Ambient lighting
            vec3 ambient = 0.2 * vec3(1.0);

            // Diffuse lighting
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * vec3(1.0);

            // Specular lighting
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * vec3(1.0);

            // Attenuation
            float distance = length(lightPos - FragPos);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
            diffuse *= attenuation;
            specular *= attenuation;

            // Texture
            vec3 textureColor = hasTexture ? texture(texture1, TexCoord).rgb : vec3(1.0);

            // Object color
            vec3 finalColor = objectColor;
            if(useVertexColor) {
                finalColor *= VertexColor;
            }

            // Combine with texture
            finalColor *= textureColor;

            // Final color
            vec3 result = (ambient + diffuse + specular) * finalColor;
            FragColor = vec4(result, 1.0);
        }
    )";

    ShaderProgram shaderProgram(vertexShaderSource, fragmentShaderSource);

    // Включение теста глубины и отсечения граней
    glEnable(GL_DEPTH_TEST);

    // Отсечение передних граней, так как нормали инвертированы и теперь указывают внутрь
    glCullFace(GL_FRONT);

    // Порядок обхода вершин остается против часовой стрелки (CCW)
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Загрузка текстур
    GLuint texture = TextureManager::LoadTexture("../assets/room.png");
    if (texture == 0) {
        std::cerr << "Failed to load texture!" << std::endl;
    }

    // Загрузка моделей
    std::shared_ptr<Model> room = std::make_shared<Model>("../assets/room.obj");
    // Установка свойств модели
    room->material.color = glm::vec3(1.0f);
    room->transform.position = glm::vec3(0.0f, 2.35f, 1.0f);
    room->transform.scale = glm::vec3(5.0f, 3.3f, 14.0f);
    const glm::vec3 faceColors[6] = {
            glm::vec3(1.0f, 0.149f, 0.0f), // Left - Red
            glm::vec3(0.921f, 0.921f, 0.921f),
            glm::vec3(0.0f, 0.59f, 1.0f), // Right - Blue
            glm::vec3(0.921f, 0.921f, 0.921f),
            glm::vec3(0.921f, 0.921f, 0.921f),
            glm::vec3(0.921f, 0.921f, 0.921f)
    };

    size_t verticesPerFace = 6;
    for (size_t face = 0; face < 6; ++face) {
        for (size_t vert = 0; vert < verticesPerFace; ++vert) {
            size_t index = face * verticesPerFace + vert;
            if (index < room->mesh.colors.size()) {
                room->mesh.colors[index] = faceColors[face];
            }
        }
    }
    for (auto& n : room->mesh.normals) {
        n = glm::normalize(n) * -1.0f;
    }
    room->setupBuffers();
    scene.addModel(room);

    std::shared_ptr<Model> cubeItem = std::make_shared<Model>("../assets/cube.obj");
    cubeItem->transform.position = glm::vec3(1.0f, 0.0f, -3.6f);
    cubeItem->transform.rotation = glm::vec3(0.0f, 8.0f, 0.0f);
    cubeItem->transform.scale = glm::vec3(0.5f, 0.95f, 0.5f);
    scene.addModel(cubeItem);

    std::shared_ptr<Model> cubeItem2 = std::make_shared<Model>("../assets/cube.obj");
    cubeItem2->transform.position = glm::vec3(-1.0f, -0.45f, -2.6f);
    cubeItem2->transform.rotation = glm::vec3(0.0f, -4.0f, 0.0f);
    cubeItem2->transform.scale = glm::vec3(0.5f, 0.5f, 0.5f);
    scene.addModel(cubeItem2);

    std::shared_ptr<Model> sphereItem = std::make_shared<Model>("../assets/sphere.obj");
    sphereItem->transform.position = glm::vec3(-3.5f, 0.0f, -3.5f);
    scene.addModel(sphereItem);

    std::shared_ptr<Model> lightPanel = std::make_shared<Model>("../assets/plane.obj");
    lightPanel->transform.position = glm::vec3(0.0f, 5.6f, -5.0f);
    lightPanel->transform.scale = glm::vec3(2.0f, 1.0f, 2.0f);
    lightPanel->transform.rotation = glm::vec3(180.0f, 0.0f, 0.0f);
    lightPanel->isLight = true;
    lightPanel->emissiveColor = glm::vec3(1.0f, 1.0f, 0.8f);
    scene.addModel(lightPanel);

    glm::vec3 lightPosInWorld = lightPanel->transform.position;

    // Основной цикл
    while (!glfwWindowShouldClose(window)) {
        // Измерение времени кадра
        static auto lastFrameTime = std::chrono::high_resolution_clock::now();
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;

        // Обработка событий и ввода
        glfwPollEvents();
        processCursorToggle(window);
        processInput(window, deltaTime);

        // Начало нового кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Очистка буферов
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Использование шейдера
        shaderProgram.use();

        // Обновление юниформов освещения
        lightPosInWorld = lightPanel->transform.position;
        glUniform3fv(shaderProgram.lightPos, 1, glm::value_ptr(lightPosInWorld));
        glUniform3fv(shaderProgram.viewPos, 1, glm::value_ptr(cameraPos));

        // Настройка текстуры
        if (texture != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glUniform1i(shaderProgram.texture1, 0);
        }

        // Вычисление матриц View и Projection
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        int screenWidth, screenHeight;
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        float aspectRatio = static_cast<float>(screenWidth) / screenHeight;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

        // Отрисовка сцены
        scene.drawAll(shaderProgram, view, projection);

        // Отрисовка интерфейса
        show_tools(scene);

        // Менюбар ImGui
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save")) {
                    std::string savePath = "../assets/saved_cube.obj";
                    std::cout << "Cube state saved to: " << savePath << std::endl;
                }
                if (ImGui::MenuItem("Load Model")) {
                    // Реализация загрузки модели
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Завершение кадра ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Обмен буферов
        glfwSwapBuffers(window);
    }

    // Очистка ресурсов
    glDeleteProgram(shaderProgram.ID);
    TextureManager::Clear();

    // Shutdown ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Закрытие окна и завершение GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, float deltaTime) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraUp * cameraSpeed;
}
void processCursorToggle(GLFWwindow* window) {
    static bool pressed_flag = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!pressed_flag) {
            pressed_flag = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
        }
    }
    else {
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

        // Обновление направления взгляда
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);

        // Пересчет векторов камеры
        glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
        cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Загрузка шрифта
    io.Fonts->AddFontFromFileTTF("../assets/helvetica_regular.otf", 16.0f);
    io.FontDefault = io.Fonts->Fonts.back();

    // Создание текстуры шрифтов
    unsigned char* tex_pixels = nullptr;
    int tex_width, tex_height;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    static size_t activeModelIndex = 0;

    ImGui::Begin("Tools");

    // Список выбора активного объекта
    if (ImGui::BeginCombo("Active Object", ((scene.models[activeModelIndex]->name.c_str() + std::to_string(activeModelIndex + 1))).c_str())) {
        for (size_t i = 0; i < scene.models.size(); ++i) {
            bool isSelected = (activeModelIndex == i);
            if (ImGui::Selectable(("Object " + std::to_string(i + 1)).c_str(), isSelected))
                activeModelIndex = i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Редактирование активного объекта
    if (activeModelIndex < scene.models.size()) {
        auto& model = scene.models[activeModelIndex];
        std::string header = (scene.models[activeModelIndex]->name.c_str() + std::to_string(activeModelIndex + 1));
        if (ImGui::CollapsingHeader(header.c_str())) {
            ImGui::Text("Transformation");

            // Позиция
            ImGui::DragFloat3("Position", glm::value_ptr(model->transform.position), 0.1f);

            // Вращение
            ImGui::DragFloat3("Rotation", glm::value_ptr(model->transform.rotation), 1.0f);

            // Масштаб
            ImGui::DragFloat3("Scale", glm::value_ptr(model->transform.scale), 0.1f);

            // Материал
            ImGui::Separator();
            ImGui::Text("Material");

            // Цвет объекта
            ImGui::ColorEdit3("Color", glm::value_ptr(model->material.color));

            // Загрузка текстуры
            if (ImGui::Button("Load Texture")) {
                // Реализуйте диалог выбора файла или используйте предопределенные пути
                std::string texturePath = "../assets/uss.png";
                GLuint texID = TextureManager::LoadTexture(texturePath);
                if (texID != 0) {
                    model->material.textureID = texID;
                }
            }

            // Удаление текстуры
            if (ImGui::Button("Remove Texture")) {
                model->material.textureID = 0;
            }
        }
    }

    ImGui::End();
}
