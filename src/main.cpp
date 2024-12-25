#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>
#include <unordered_map>
#include <array>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --------------------- Shader Compilation and Program Linking ---------------------
GLuint CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src_cstr = source.c_str();
    glShaderSource(shader, 1, &src_cstr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader Compilation Error:\n" << infoLog << std::endl;
    }
    return shader;
}
struct ShaderProgram {
    GLuint ID;
    // Uniform locations для основного шейдера
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
    // Uniforms for reflections
    GLint enableReflection;
    GLint cubeMap;
    // Uniforms for per-face reflections (only for room)
    GLint faceReflectionFlags;

    // Uniforms for depth shader (устанавливаются при создании depthShader, если нужно)
    GLint depth_lightSpaceMatrix;
    GLint depth_model;

    ShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc) {
        GLuint vertexShader   = CompileShader(GL_VERTEX_SHADER, vertexSrc);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        // Check for linking errors
        GLint success;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar infoLog[1024];
            glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
            std::cerr << "Shader Program Linking Error:\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Кэшируем локации uniform-переменных (если они есть в шейдере)
        uMVP            = glGetUniformLocation(ID, "uMVP");
        uModel          = glGetUniformLocation(ID, "uModel");
        isLight         = glGetUniformLocation(ID, "isLight");
        emissiveColor   = glGetUniformLocation(ID, "emissiveColor");
        useVertexColor  = glGetUniformLocation(ID, "useVertexColor");
        objectColor     = glGetUniformLocation(ID, "objectColor");
        hasTexture      = glGetUniformLocation(ID, "hasTexture");
        lightPos        = glGetUniformLocation(ID, "lightPos");
        viewPos         = glGetUniformLocation(ID, "viewPos");
        texture1        = glGetUniformLocation(ID, "texture1");
        enableReflection= glGetUniformLocation(ID, "enableReflection");
        cubeMap         = glGetUniformLocation(ID, "cubeMap");
        faceReflectionFlags = glGetUniformLocation(ID, "faceReflectionFlags");
        // Эти два для depthShader (могут быть -1, если не найдены)
        depth_lightSpaceMatrix = glGetUniformLocation(ID, "lightSpaceMatrix");
        depth_model            = glGetUniformLocation(ID, "model");
    }

    void use() const {
        glUseProgram(ID);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc >= 0) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
        }
    }

    void setVec3(const std::string &name, const glm::vec3 &value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc >= 0) {
            glUniform3fv(loc, 1, &value[0]);
        }
    }

    void setInt(const std::string &name, int value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc >= 0) {
            glUniform1i(loc, value);
        }
    }

    void setBool(const std::string &name, bool value) const {
        setInt(name, value ? 1 : 0);
    }

    // Метод для установки faceReflectionFlags
    void setFaceReflectionFlags(const std::array<bool, 6>& flags) const {
        // Если faceReflectionFlags == -1, значит в шейдере нет такого uniform
        if (faceReflectionFlags < 0) return;

        int intFlags[6];
        for (int i = 0; i < 6; ++i) {
            intFlags[i] = flags[i] ? 1 : 0;
        }
        glUniform1iv(faceReflectionFlags, 6, intFlags);
    }

    // Метод для включения отражений на всех 6 сторонах (для не-комнаты)
    void setAllReflectionFlags() const {
        // Если faceReflectionFlags == -1, значит в шейдере нет такого uniform
        if (faceReflectionFlags < 0) return;

        int intFlags[6] = {1, 1, 1, 1, 1, 1};
        glUniform1iv(faceReflectionFlags, 6, intFlags);
    }
};
// --------------------- Texture Manager ---------------------
class TextureManager {
public:
    static GLuint LoadTexture(const std::string& path) {
        if (textures.find(path) != textures.end()) {
            return textures[path];
        }

        GLuint textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        stbi_set_flip_vertically_on_load(true); // Для стандартных 2D-текстур
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format;
            if (nrComponents == 1)      format = GL_RED;
            else if (nrComponents == 3) format = GL_RGB;
            else if (nrComponents == 4) format = GL_RGBA;
            else                       format = GL_RGB; // fallback

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
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
// --------------------- Geometric Structures ---------------------
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
    glm::vec3 rotation; // В градусах
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
// --------------------- Model Class with VAO/VBO/EBO ---------------------
class Model {
public:
    Mesh mesh;
    std::string name;
    Transform transform;
    GLuint VAO, VBO, EBO;
    bool isLight = false;
    glm::vec3 emissiveColor = glm::vec3(1.0f);
    Material material;
    bool enableReflection = false; // отражения включены/выключены
    bool isRoom = false; // если это «комната» (room)
    std::array<bool, 6> faceReflectionFlags = { true, true, true, true, true, true }; // только для комнаты

    Model(const Mesh& m, const Material& mat = Material()) : mesh(m), material(mat) {
        setupBuffers();
    }

    // Конструктор: загрузка из .obj
    Model(const std::string& objPath, const std::string& objName = "", const Material& mat = Material())
            : material(mat){
        mesh = loadOBJ(objPath);

        if (objName.empty()) {
            // Извлекаем имя файла (без пути и расширения)
            size_t slashPos = objPath.find_last_of("/\\");
            size_t dotPos = objPath.find_last_of(".");
            if (slashPos == std::string::npos) slashPos = 0; else slashPos++;
            if (dotPos == std::string::npos) dotPos = objPath.size();
            name = objPath.substr(slashPos, dotPos - slashPos);
        } else {
            name = objName;
        }

        setupBuffers();
    }

    ~Model() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    // Рисуем модель основным шейдером (который умеет и тени, и отражения)
    void draw(const ShaderProgram& shader,
              const glm::mat4& view,
              const glm::mat4& projection,
              GLuint cubeMapTexture) const{
        shader.use();

        glm::mat4 modelMatrix = transform.getMatrix();
        glm::mat4 mvp = projection * view * modelMatrix;

        // Передаём матрицы в шейдер
        if (shader.uMVP >= 0)   glUniformMatrix4fv(shader.uMVP,   1, GL_FALSE, glm::value_ptr(mvp));
        if (shader.uModel >= 0) glUniformMatrix4fv(shader.uModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        if (shader.isLight >= 0)      glUniform1i(shader.isLight, isLight ? 1 : 0);
        if (shader.emissiveColor >= 0)glUniform3fv(shader.emissiveColor, 1, glm::value_ptr(emissiveColor));

        bool hasVertexColors = !mesh.colors.empty();
        if (shader.useVertexColor >= 0) glUniform1i(shader.useVertexColor, hasVertexColors ? 1 : 0);

        if (shader.objectColor >= 0)   glUniform3fv(shader.objectColor, 1, glm::value_ptr(material.color));

        // Текстура
        if (material.textureID != 0 && shader.hasTexture >= 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.textureID);
            if (shader.texture1 >= 0) glUniform1i(shader.texture1, 0);
            glUniform1i(shader.hasTexture, 1);
        }
        else {
            if (shader.hasTexture >= 0) glUniform1i(shader.hasTexture, 0);
        }

        // Кубкарта
        if (shader.cubeMap >= 0) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
            glUniform1i(shader.cubeMap, 2);
        }
        // Включаем/выключаем отражения
        if (shader.enableReflection >= 0) {
            glUniform1i(shader.enableReflection, enableReflection ? 1 : 0);
        }

        // Если это комната – устанавливаем флаги отражения на каждую грань
        // Иначе (просто объект с enableReflection) – включаем все грани
        if (isRoom && shader.faceReflectionFlags >= 0) {
            shader.setFaceReflectionFlags(faceReflectionFlags);
        }
        else if (enableReflection && shader.faceReflectionFlags >= 0) {
            shader.setAllReflectionFlags();
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Метод для рендера в depth-проходе (тени)
    void drawDepth(const ShaderProgram& depthShader, const glm::mat4& lightSpaceMatrix) const {
        depthShader.use();

        // Если в depthShader есть соответствующие uniform-переменные:
        if (depthShader.depth_model >= 0) {
            glm::mat4 modelMatrix = transform.getMatrix();
            glUniformMatrix4fv(depthShader.depth_model, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        }
        if (depthShader.depth_lightSpaceMatrix >= 0) {
            glUniformMatrix4fv(depthShader.depth_lightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Загрузка .obj (упрощённая)
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
                temp_vertices.push_back(vertex);
            }
            else if (prefix == "vt") {
                glm::vec2 tex;
                ss >> tex.x >> tex.y;
                temp_texCoords.push_back(tex);
            }
            else if (prefix == "vn") {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (prefix == "f") {
                std::string vertexStr;
                std::vector<Vertex> faceVertices;
                while (ss >> vertexStr) {
                    std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
                    std::stringstream vertexSS(vertexStr);
                    Vertex vertex = {0, 0, 0};
                    vertexSS >> vertex.posIndex;
                    if (!(vertexSS >> vertex.texIndex))  vertex.texIndex  = 0;
                    if (!(vertexSS >> vertex.normIndex)) vertex.normIndex = 0;
                    faceVertices.push_back(vertex);
                }

                // Fan triangulation
                for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                    std::array<Vertex, 3> tri = {faceVertices[0], faceVertices[i], faceVertices[i + 1]};
                    for (const auto& v : tri) {
                        mesh.vertices.push_back(temp_vertices[v.posIndex - 1]);

                        if (v.texIndex > 0 && v.texIndex - 1 < temp_texCoords.size())
                            mesh.texCoords.push_back(temp_texCoords[v.texIndex - 1]);
                        else
                            mesh.texCoords.push_back(glm::vec2(0.0f, 0.0f));

                        if (v.normIndex > 0 && v.normIndex - 1 < temp_normals.size())
                            mesh.normals.push_back(temp_normals[v.normIndex - 1]);
                        else
                            mesh.normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

                        mesh.colors.push_back(glm::vec3(1.0f)); // по умолчанию белый
                        mesh.indices.push_back((unsigned int)(mesh.vertices.size() - 1));
                    }
                }
            }
            // Остальные префиксы игнорируем
        }

        file.close();
        return mesh;
    }

    // Создание и настройка буферов VAO/VBO/EBO
    void setupBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Interleaved: position(3) + normal(3) + texCoords(2) + color(3) = 11 floats
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
        // 0: position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);

        // 1: normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

        // 2: texCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));

        // 3: color
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));

        glBindVertexArray(0);
    }
};
// --------------------- Scene Class ---------------------
class Scene {
public:
    std::vector<std::shared_ptr<Model>> models;

    void addModel(const std::shared_ptr<Model>& model) {
        int counter = 0;
        for(auto& ml: models) {
            if (ml->name.find(model->name) != std::string::npos)
                counter++;
        }
        if (counter)
            model->name += std::to_string(counter);
        models.push_back(model);
    }

    // Отрисовать все модели основным шейдером (с отражениями/тенями и т.д.)
    void drawAll(const ShaderProgram& shader,
                 const glm::mat4& view,
                 const glm::mat4& projection,
                 GLuint cubeMapTexture) const{
        for (const auto& model : models) {
            model->draw(shader, view, projection, cubeMapTexture);
        }
    }

    // Отрисовать все модели в depth-проход (для построения карты теней)
    void drawAllDepth(const ShaderProgram& depthShader,
                      const glm::mat4& lightSpaceMatrix) const{
        // Обычно мы не рисуем зеркально-отражающие объекты в depth pass
        // (или же можем рисовать, но это вопрос дизайна теней; в данном коде исключают отражающиеся)
        for (const auto& model : models) {
            if (!model->enableReflection)
                model->drawDepth(depthShader, lightSpaceMatrix);
        }
    }
};
// --------------------- Глобальные переменные камеры ---------------------
Scene scene;
glm::vec3 cameraPos(0.0f, 20.0f, 30.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

double lastX, lastY;
float yaw = -90.0f;
float pitch = -25.0f;
bool firstMouse = true;

// --------------------- Объявление функций ---------------------
void processInput(GLFWwindow* window, float deltaTime, std::shared_ptr<Model> model);
void processCursorToggle(GLFWwindow* window);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow* window, unsigned int codepoint);

void setup_imgui(GLFWwindow* window);
void show_tools(Scene& scene);
void setup_style(ImGuiStyle& style, ImGuiIO& io);
GLuint loadCubemap(const std::string& path){
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (size_t i = 0; i < 6; i++){
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
std::string ReadFileToString(const std::string& filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    fileStream.close();
    return buffer.str();
}

// --------------------- main() ---------------------
int main() {
    // 1. Инициализация GLFW
    if (!glfwInit()) {
        return -1;
    }

    // 2. Настройка контекста OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Создаём окно в полноэкранном режиме
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Lemotech 3D", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Контекст
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Инициализация ImGui
    setup_imgui(window);

    // Калбэки
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);
    cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront.y = sin(glm::radians(pitch));
    cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront   = glm::normalize(cameraFront);

    cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    cameraUp    = glm::normalize(glm::cross(cameraRight, cameraFront));

    // --------------------- Шейдер для основного рендера (отражения, тени) ---------------------
    ShaderProgram shaderProgram(ReadFileToString("../assets/VS.glsl"), ReadFileToString("../assets/FS.glsl"));
    // --------------------- Шейдер для depth pass ---------------------
    ShaderProgram depthShader(ReadFileToString("../assets/DepthVS.glsl"), ReadFileToString("../assets/DepthFS.glsl"));
    ShaderProgram skyboxShader(ReadFileToString("../assets/SkyboxVS.glsl"), ReadFileToString("../assets/SkyboxFS.glsl"));
    // --------------------- Включаем буфер глубины и culling ---------------------
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    // --------------------- FBO/Texture для shadow map ---------------------
    const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --------------------- Создание динамической кубкарты ---------------------
    GLuint dynamicCubeMap;
    glGenTextures(1, &dynamicCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
    unsigned int cubeMapSize = 512;
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                     cubeMapSize, cubeMapSize, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // FBO/RBO для рендера в кубкарту
    GLuint captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeMapSize, cubeMapSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --------------------- Загружаем модели ---------------------
    std::string texturePath = "../assets/uss.png";
    GLuint texID = TextureManager::LoadTexture(texturePath);

    // 2) Пара кубов
    auto USS = std::make_shared<Model>("../assets/USS.obj", "USS Enterprise");
    USS->transform.position = glm::vec3(1.0f, 3.0f, 0.0f);
    USS->transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
    USS->transform.scale    = glm::vec3(0.5f, 0.5f, 0.5f);
    USS->enableReflection   = false;
    USS->material.textureID = texID;

    scene.addModel(USS);


    std::string cubemapPath = "../assets/cubemap/DayInTheClouds4k.jpg";

    stbi_set_flip_vertically_on_load(false);
    GLuint skyboxTexture = loadCubemap(cubemapPath);
    float skyboxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f
    };
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    // 1) Комната
    auto room = std::make_shared<Model>("../assets/Room.obj", "",Material());
    room->material.color = glm::vec3(1.0f);
    room->transform.position = glm::vec3(0.0f, -1.05f, 1.0f);
    room->transform.scale = glm::vec3(50.0f, 0.1f, 50.0f);
    room->enableReflection = false;
    room->material.textureID = TextureManager::LoadTexture("../assets/field.hdr");


    // Раскрашиваем стены комнаты (просто пример)
    const glm::vec3 faceColors[6] = {
            glm::vec3(1.0f, 0.149f, 0.0f),     // +X
            glm::vec3(0.921f, 0.921f, 0.921f),// -X
            glm::vec3(0.0f, 0.59f, 1.0f),      // +Y
            glm::vec3(0.921f, 0.921f, 0.921f),// -Y
            glm::vec3(0.921f, 0.921f, 0.921f),// +Z
            glm::vec3(0.921f, 0.921f, 0.921f) // -Z
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
    room->setupBuffers();
    scene.addModel(room);

    auto Tree = std::make_shared<Model>("../assets/Tree.obj", "Tree");
    Tree->transform.position = glm::vec3(-3.5f, 0.0f, -3.5f);
    Tree->transform.scale = glm::vec3(0.005f, 0.005f, 0.005f);
    Tree->enableReflection   = false;
    Tree->material.textureID = TextureManager::LoadTexture("../assets/Tree/Tree.png");
    scene.addModel(Tree);

    auto Capsule = std::make_shared<Model>("../assets/Capsule/Capsule.obj", "Capsule");
    Capsule->transform.position = glm::vec3(-3.5f, 0.0f, -3.5f);
    Capsule->transform.scale = glm::vec3(0.05f, 0.05f, 0.05f);
    Capsule->enableReflection   = false;
    Capsule->material.textureID = TextureManager::LoadTexture("../assets/Capsule/Capsule.png");
    scene.addModel(Capsule);

    auto Sun = std::make_shared<Model>("../assets/Sphere.obj", "Sun");
    Sun->transform.position = glm::vec3(0.0f, 22.0f, 10.0f);
    Sun->transform.scale    = glm::vec3(1.0f, 1.0f, 1.0f);
    Sun->transform.rotation = glm::vec3(180.0f, 0.0f, 0.0f);
    Sun->isLight            = true;
    Sun->emissiveColor      = glm::vec3(0.97f, 0.97f, 0.87f);
    scene.addModel(Sun);

    // Позиция света совпадает с center Sun
    glm::vec3 lightPosInWorld = Sun->transform.position;

    // Матрицы «камер» для кубкарты
    std::vector<glm::mat4> captureViews = {
            glm::lookAt(glm::vec3(0.0f), glm::vec3(+1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, +1.0f,  0.0f),  glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, +1.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    float capsuleY = Capsule->transform.position.y; // Initialize to current y-position
    bool isFalling = true;                          // Start by falling
    float fallSpeed = 7.0f;                         // Units per second
    float minY = -1.0f;                             // Minimum y-position
    float maxY = USS->transform.position.y;
    // --------------------- Цикл рендера ---------------------
    while (!glfwWindowShouldClose(window)) {
        // Вычисляем deltaTime
        static auto lastFrameTime = std::chrono::high_resolution_clock::now();
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;

        glfwPollEvents();
        processCursorToggle(window);
        processInput(window, deltaTime, USS);
        USS->transform.position = { cameraPos.x, cameraPos.y -9.0f, cameraPos.z -16.0f};
        Capsule->transform.position = { cameraPos.x, cameraPos.y -10.0f, cameraPos.z -16.0f};
        Capsule->transform.position.y += (int)Capsule->transform.position.y % 25;

        glm::vec3 basePosition = glm::vec3(cameraPos.x, cameraPos.y - 10.0f, cameraPos.z - 16.0f);

        // Update maxY based on USS's current y-position
        maxY = USS->transform.position.y;

        // Update capsuleY based on current movement direction
        if (isFalling) {
            capsuleY -= fallSpeed * deltaTime;
            if (capsuleY <= minY) {
                capsuleY = basePosition.y;
            }
        }

        // Apply the updated y-position to the Capsule
        Capsule->transform.position = glm::vec3(basePosition.x, capsuleY, basePosition.z);

        // Обновляем позицию света
        lightPosInWorld = Sun->transform.position;

        // ===================== 1) Рендер сцены в кубкарту =====================
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glViewport(0, 0, cubeMapSize, cubeMapSize);
        glEnable(GL_DEPTH_TEST);

        // Сохраняем исходные флаги отражений и отключаем их у всех,
        // чтобы объекты не «видели» своих же отражений (рекурсия).
        std::vector<bool> originalReflectionFlags;
        originalReflectionFlags.reserve(scene.models.size());
        for (auto& model : scene.models) {
            originalReflectionFlags.push_back(model->enableReflection);
            if (model->enableReflection) {
                model->enableReflection = false;
            }
        }

        // Для каждой грани кубкарты
        for (unsigned int i = 0; i < 6; ++i) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dynamicCubeMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Вычисляем view и projection для этой грани кубкарты
            glm::mat4 view = captureViews[i];
            glm::mat4 projection = captureProjection;

            // В качестве «камеры» для окружения выступает точка (0,0,0).
            // Если вы хотите отражения вокруг конкретного объекта, нужно сместить всё так,
            // чтобы объект был в (0,0,0). Либо использовать lookAt от конкретной позиции.
            // Но здесь для простоты центрируем в origin.

            // Рисуем сцену тем же shaderProgram, но у всех объектов отражения отключены
            // (для избежания рекурсии).
            shaderProgram.use();
            // Т.к. мы можем захотеть тени в окружении, назначим lightSpaceMatrix, lightPos etc.
            // Но можно и не делать этого, если хотим «упростить» картинку для отражений.
            // (здесь оставлено для полноты)
            float near_plane = 1.0f, far_plane = 25.0f;
            float orthoSize  = 10.0f;
            glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
            glm::mat4 lightView       = glm::lookAt(lightPosInWorld, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;
            shaderProgram.setMat4("lightSpaceMatrix", lightSpaceMatrix);
            shaderProgram.setVec3("lightPos", lightPosInWorld);
            // Камеру берём в (0,0,0)
            shaderProgram.setVec3("viewPos", glm::vec3(0.0f));

            // Подвязываем shadowMap (если хотим тени в отражении)
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            shaderProgram.setInt("shadowMap", 1);

            // Кубкарту для самого себя не привязываем, т.к. отражения отключены.

            // Рисуем все модели:
            // (объекты просто нарисуются обычным шейдером, но без отражений)
            for (auto& model : scene.models) {
                // Временно матрицу модели «сдвигать» не будем,
                // но если нужно отражать относительно другого центра,
                // придётся учитывать смещение.
                glm::mat4 localView = view;
                glm::mat4 localProj = projection;
                model->draw(shaderProgram, localView, localProj, /*cubeMap=*/0);
            }
        }

        // Возвращаем отражения на место
        for (size_t i = 0; i < scene.models.size(); ++i) {
            scene.models[i]->enableReflection = originalReflectionFlags[i];
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ===================== 2) Генерация shadow map =====================
        float near_plane = 1.0f, far_plane = 100.0f;
        float orthoSize = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPosInWorld, glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthShader.use();
        // Установим lightSpaceMatrix в depthShader
        if (depthShader.depth_lightSpaceMatrix >= 0) {
            glUniformMatrix4fv(depthShader.depth_lightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        }
        // Рисуем все «неотражающиеся» объекты в depth pass
        scene.drawAllDepth(depthShader, lightSpaceMatrix);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ===================== 3) Рендер основной сцены (на экран) =====================
        int screenWidth, screenHeight;
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.use();
        // lightSpaceMatrix для теней
        shaderProgram.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // свет
        shaderProgram.setVec3("lightPos", lightPosInWorld);
        shaderProgram.setVec3("viewPos", cameraPos);

        // Привязываем shadowMap на юнит 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shaderProgram.setInt("shadowMap", 1);

        // Привязываем динамическую кубкарту на юнит 2
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        shaderProgram.setInt("cubeMap", 2);

        // Матрицы камеры для «обычного» рендера
        glm::mat4 view  = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        float aspectRatio = (float)screenWidth / (float)std::max(screenHeight, 1);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

        scene.drawAll(shaderProgram, view, projection, dynamicCubeMap);

        // 2) Меняем функцию глубины, чтобы фрагменты skybox прошли, если глубина <= равна (GL_LEQUAL)
        glDepthFunc(GL_LEQUAL);

// Отключаем запись в z-буфер, чтобы skybox не «затирал» глубину
        glDepthMask(GL_FALSE);

        skyboxShader.use();
// передаём projection и view (без трансляции)
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); // убираем позицию камеры
        skyboxShader.setMat4("view", skyboxView);
        skyboxShader.setMat4("projection", projection);

// привязываем куб-карту
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
// если юниформ называется skybox, то
        skyboxShader.setInt("skybox", 0);

// рендерим куб
        glDrawArrays(GL_TRIANGLES, 0, 36);

// возвращаем настройки
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        // ===================== ImGui =====================
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Панель инструментов
        show_tools(scene);

        // Рендерим ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glDeleteProgram(shaderProgram.ID);
    glDeleteProgram(depthShader.ID);
    glDeleteTextures(1, &depthMap);
    glDeleteFramebuffers(1, &depthMapFBO);
    glDeleteTextures(1, &dynamicCubeMap);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
    TextureManager::Clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// --------------------- Реализация вспомогательных функций ---------------------
void processInput(GLFWwindow* window, float deltaTime, std::shared_ptr<Model> model) {
    auto speed_coef = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        speed_coef = 4;
    float cameraSpeed = 12.5f * deltaTime * speed_coef;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos.z += cameraFront.z * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos.z -= cameraFront.z * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        auto c = glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        cameraPos -= c;
        model->transform.rotation.y += c.x * 4;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        auto c = glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        cameraPos += c;
        model->transform.rotation.y -= c.x * 4;
    }
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

        yaw   += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront.y = sin(glm::radians(pitch));
        cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront   = glm::normalize(cameraFront);

        cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUp    = glm::normalize(glm::cross(cameraRight, cameraFront));
    }

    // передаём в ImGui
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
// --------------------- ImGui ---------------------
void setup_imgui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    setup_style(ImGui::GetStyle(), io);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Загрузим шрифт
    io.Fonts->AddFontFromFileTTF("../assets/helvetica_regular.otf", 16.0f);
    io.FontDefault = io.Fonts->Fonts.back();

    unsigned char* tex_pixels = nullptr;
    int tex_width, tex_height;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    io.Fonts->TexID = (void *)(intptr_t)tex;
}
void setup_style(ImGuiStyle& style, ImGuiIO& io) {
    style.FrameRounding = 12.0f;
    style.FrameBorderSize = 1.0f;
    style.WindowRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding     = 6.0f;
    style.PopupRounding    = 6.0f;
    style.ChildRounding    = 6.0f;
    style.WindowPadding    = ImVec2(15, 15);
    style.FramePadding     = ImVec2(10, 6);
    style.ItemSpacing      = ImVec2(10, 10);

    ImVec4 buttonColor        = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
    ImVec4 buttonHoveredColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    ImVec4 buttonActiveColor  = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    ImVec4 borderColor        = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

    style.Colors[ImGuiCol_Button]        = buttonColor;
    style.Colors[ImGuiCol_ButtonHovered] = buttonHoveredColor;
    style.Colors[ImGuiCol_ButtonActive]  = buttonActiveColor;
    style.Colors[ImGuiCol_Border]        = borderColor;
    style.Colors[ImGuiCol_Text]          = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
}
// --------------------- Окно инструментов ImGui ---------------------
void show_tools(Scene& scene) {
    static size_t activeModelIndex = 0;

    ImGui::Begin("Tools");

    // Выбор активного объекта
    if (!scene.models.empty()) {
        if (activeModelIndex >= scene.models.size()) {
            activeModelIndex = 0;
        }
        std::string currentName = scene.models[activeModelIndex]->name;

        if (ImGui::BeginCombo("Active Object", currentName.c_str())) {
            for (size_t i = 0; i < scene.models.size(); ++i) {
                bool isSelected = (activeModelIndex == i);
                std::string label = scene.models[i]->name;
                if (ImGui::Selectable(label.c_str(), isSelected))
                    activeModelIndex = i;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    // Редактирование выбранного объекта
    if (activeModelIndex < scene.models.size()) {
        auto& model = scene.models[activeModelIndex];
        if (ImGui::CollapsingHeader(model->name.c_str())) {
            ImGui::Text("Transformation");
            ImGui::DragFloat3("Position", glm::value_ptr(model->transform.position), 0.1f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(model->transform.rotation), 1.0f);
            ImGui::DragFloat3("Scale",    glm::value_ptr(model->transform.scale),    0.1f);

            ImGui::Separator();
            ImGui::Text("Material");
            ImGui::ColorEdit3("Color", glm::value_ptr(model->material.color));

            // Кнопка загрузки текстуры (заглушка)
            if (ImGui::Button("Load Texture")) {
                // Сюда можно вставить реальный file dialog
                std::string texturePath = "../assets/uss.png";
                GLuint texID = TextureManager::LoadTexture(texturePath);
                if (texID != 0) {
                    model->material.textureID = texID;
                }
            }

            // Удалить текстуру
            if (ImGui::Button("Remove Texture")) {
                model->material.textureID = 0;
            }

            ImGui::Separator();

            // Отражения
                ImGui::Text("Reflection");
                // Кнопка включения/выключения отражений
                if (model->enableReflection) {
                    if (ImGui::Button("Disable Reflection")) {
                        model->enableReflection = false;
                    }
                }
                else {
                    if (ImGui::Button("Enable Reflection")) {
                        model->enableReflection = true;
                    }
                }
        }
    }

    ImGui::End();
}