#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <vector>
#include <stack>
#include <unordered_set>
#include <cmath>
#include <cstring>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace std;

// Размер текстуры
const int canvasWidth = 800;
const int canvasHeight = 600;

int canvasFillWidth = 400;
int canvasFillHeight = 300;

int loadedImageWidth;
int loadedImageHeight;


// Текстура для рисования
GLuint canvasTexture;
GLuint canvasFillTexture;
std::vector<GLubyte> canvasData(canvasWidth * canvasHeight * 4, 255); // RGBA
std::vector<GLubyte> canvasFillData(canvasFillWidth * canvasFillHeight * 4, 255); // RGBA

// Инициализация текстуры
void InitCanvasTexture(GLuint& texture, std::vector<GLubyte>& data,int width, int height) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void ClearCanvas() {
    std::fill(canvasData.begin(), canvasData.end(), 255); // Заполняем вектор 255 (Белый)
    glBindTexture(GL_TEXTURE_2D, canvasTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, canvasData.data()); // Обновляем текстуру
}

// Функция рисования "толстой" линии
void DrawCircleOnCanvas(int cx, int cy, int radius, const GLubyte color[4]) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                int nx = cx + x;
                int ny = cy + y;
                if (nx >= 0 && nx < canvasWidth && ny >= 0 && ny < canvasHeight) {
                    int index = (ny * canvasWidth + nx) * 4; // Индекс пикселя в векторе данных
                    memcpy(&canvasData[index], color, 4); // Копируем цвет в данные текстуры
                }
            }
        }
    }
    glBindTexture(GL_TEXTURE_2D, canvasTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, canvasData.data()); // Обновляем текстуру
}
void FloodFill(int x, int y, int width, int height, std::vector<GLubyte>& canvasData, const GLubyte targetColor[4], const GLubyte fillColor[4]) {
    // Проверка границ
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    int pixelIndex = (y * width + x) * 4;
    if (memcmp(&canvasData[pixelIndex], targetColor, 4) != 0 || memcmp(&canvasData[pixelIndex], fillColor, 4) == 0) {
        return;
    }

    int x_left = x;
    int x_right = x;

    // Найти крайние левые границы
    while (x_left > 0) {
        int leftIndex = (y * width + (x_left - 1)) * 4;
        if (memcmp(&canvasData[leftIndex], targetColor, 4) != 0) {
            break;
        }
        x_left--;
    }

    // Найти крайние правые границы
    while (x_right < width - 1) {
        int rightIndex = (y * width + (x_right + 1)) * 4;
        if (memcmp(&canvasData[rightIndex], targetColor, 4) != 0) {
            break;
        }
        x_right++;
    }

    // Закрасить интервал
    for (int i = x_left; i <= x_right; i++) {
        int index = (y * width + i) * 4;
        memcpy(&canvasData[index], fillColor, 4);
    }

    // Рекурсивно проверить строки выше и ниже
    for (int i = x_left; i <= x_right; i++) {
        if (y > 0) {
            int upIndex = ((y - 1) * width + i) * 4;
            if (memcmp(&canvasData[upIndex], targetColor, 4) == 0) {
                FloodFill(i, y - 1, width, height, canvasData, targetColor, fillColor);
            }
        }
        if (y < height - 1) {
            int downIndex = ((y + 1) * width + i) * 4;
            if (memcmp(&canvasData[downIndex], targetColor, 4) == 0) {
                FloodFill(i, y + 1, width, height, canvasData, targetColor, fillColor);
            }
        }
    }
}

void fillWithImage(int x, int y,int mouseX, int mouseY, int width, int height, std::vector<GLubyte>& canvasData,
                   const GLubyte targetColor[4], const std::vector<GLubyte>& imageData,
                   int imgWidth, int imgHeight) {
    // Проверка границ
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    int pixelIndex = (y * width + x) * 4;
    if (memcmp(&canvasData[pixelIndex], targetColor, 4) != 0) {
        return;
    }

    int x_left = x;
    int x_right = x;

    // Найти крайние левые границы
    while (x_left > 0) {
        int leftIndex = (y * width + (x_left - 1)) * 4;
        if (memcmp(&canvasData[leftIndex], targetColor, 4) != 0) {
            break;
        }
        x_left--;
    }

    // Найти крайние правые границы
    while (x_right < width - 1) {
        int rightIndex = (y * width + (x_right + 1)) * 4;
        if (memcmp(&canvasData[rightIndex], targetColor, 4) != 0) {
            break;
        }
        x_right++;
    }

    int imageCenterX = imgWidth / 2;
    int imageCenterY = imgHeight / 2;

    // Закрасить интервал, используя данные из imageData
    for (int i = x_left; i <= x_right; i++) {
        // Индекс текущего пикселя в canvas
        int index = (y * width + i) * 4;

        // Рассчитываем смещение между координатами canvas и image
        int offsetX = (i - mouseX + imageCenterX) % imgWidth;  // Смещение по X (модуль для цикличности)
        int offsetY = (y - mouseY + imageCenterY)% imgHeight; // Смещение по Y (циклично по высоте изображения)
        if (offsetX < 0)
            offsetX += imgWidth;
        if (offsetY < 0)
            offsetY += imgHeight;

        // Рассчитываем индекс пикселя в imageData
        int imgIndex = (offsetY * imgWidth + offsetX) * 4;

        // Копируем данные пикселя из imageData в canvasData
        memcpy(&canvasData[index], &imageData[imgIndex], 4);
    }

    // Рекурсивно проверяем строки выше и ниже
    for (int i = x_left; i <= x_right; i++) {
        if (y > 0) {
            int upIndex = ((y - 1) * width + i) * 4;
            if (memcmp(&canvasData[upIndex], targetColor, 4) == 0) {
                fillWithImage(i, y - 1, mouseX, mouseY, width, height, canvasData, targetColor, imageData, imgWidth, imgHeight);
            }
        }
        if (y < height - 1) {
            int downIndex = ((y + 1) * width + i) * 4;
            if (memcmp(&canvasData[downIndex], targetColor, 4) == 0) {
                fillWithImage(i, y + 1, mouseX, mouseY, width, height, canvasData, targetColor, imageData, imgWidth, imgHeight);
            }
        }
    }
}








bool IsSimilarColor(const GLubyte pixel[4], const GLubyte targetColor[4], int tolerance = 10) {
    // Сравниваем цвет с учетом некоторой допустимой разницы
    return (abs(pixel[0] - targetColor[0]) < tolerance &&
            abs(pixel[1] - targetColor[1]) < tolerance &&
            abs(pixel[2] - targetColor[2]) < tolerance &&
            abs(pixel[3] - targetColor[3]) < tolerance);
}

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& pair) const {
        auto hash1 = std::hash<T1>()(pair.first);
        auto hash2 = std::hash<T2>()(pair.second);
        return hash1 ^ hash2; // XOR для комбинирования хэшей
    }
};

std::vector<std::pair<int, int>> GetBorderPoints(int x, int y, const std::vector<GLubyte>& canvasData, const GLubyte targetColor[4]) {
    int x_left = x;

    // Найдем крайние левые границы
    while (x_left > 0 && !IsSimilarColor(&canvasData[(y * canvasWidth + x_left) * 4], targetColor)) {
        x_left--;
    }

    // Направления для обхода
    std::vector<std::pair<int, int>> directions = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1},
        {-1, 0}, {-1, -1}, {0, -1}, {1, -1},
    };

    std::stack<std::pair<int, int>> stack;
    std::unordered_set<std::pair<int, int>, pair_hash> labeled; // Используем unordered_set для хранения меток
    std::vector<std::pair<int, int>> result;

    stack.push({x_left, y});

    while (!stack.empty()) {
        auto point = stack.top();
        stack.pop();
        x = point.first;
        y = point.second;

        // Проверяем, что пиксель не был отмечен и находится в пределах границ
        if (labeled.find(point) == labeled.end() && x >= 0 && x < canvasWidth && y >= 0 && y < canvasHeight && IsSimilarColor(&canvasData[(y * canvasWidth + x) * 4], targetColor)) {
            result.push_back(point);
            labeled.insert(point);

            // Добавляем соседние пиксели в стек
            for (const auto& direction : directions) {
                stack.push({x + direction.first, y + direction.second});
            }
        }
    }

    return result;
}

void HighlightBorder(int x, int y, std::vector<GLubyte>& canvasData, const GLubyte targetColor[4], const GLubyte borderColor[4]) {
    for (auto pixel : GetBorderPoints(x, y, canvasData, targetColor)) {
        int index = (pixel.second * canvasWidth + pixel.first) * 4;
        memcpy(&canvasData[index], borderColor, 4);
    }
    glBindTexture(GL_TEXTURE_2D, canvasTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, canvasData.data()); // Обновляем текстуру

}

void loadImage(const char* filePath) {
    if (filePath) {
        std::cout << "Loading image: " << filePath << std::endl;

        // Загрузка изображения с помощью stb_image
        int nChannels;
        int width, height;
        unsigned char* imageData = stbi_load(filePath, &width, &height, &nChannels, 4); // Принудительно загружаем как RGBA (4 канала)

        if (imageData) {
            // Обновляем ширину и высоту canvas
            loadedImageWidth = width;
            loadedImageHeight = height;

            // Переносим данные в canvasFillData
            canvasFillData.assign(imageData, imageData + (width * height * 4));

            // Освобождаем ресурсы после загрузки
            stbi_image_free(imageData);

            // Создаем или обновляем текстуру для canvas
            InitCanvasTexture(canvasFillTexture, canvasFillData, width, height);

            std::cout << "Load successful." << std::endl;
        } else {
            std::cerr << "Error: failed to load the image." << std::endl;
        }
    } else {
        std::cerr << "Error: image path not specified." << std::endl;
    }
}


int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Создаем окно
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Drawing with Texture", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Выключаем вертикальную синхронизацию

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Подключаем OpenGL и GLFW в ImGui
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Инициализация текстуры
    InitCanvasTexture(canvasTexture, canvasData, canvasWidth, canvasHeight);
    InitCanvasTexture(canvasFillTexture, canvasFillData, canvasFillWidth, canvasFillHeight);

    // Основной цикл
    bool pencilMode = true;
    bool fillMode = false;
    bool fillWithImageMode = false;
    bool borderMode = false;
    int penSize = 10; // Толщина линии
    const GLubyte penColor[4] = { 0, 0, 0, 255 }; // Белый цвет
    const GLubyte backgroundColor[4] = {255, 255, 255, 255}; // Цвет, который нужно заменить (например, белый)
    const GLubyte fillColor[4] = {0, 0, 255, 255};      // Новый цвет (синий)
    const GLubyte borderColor[4] = {255, 0, 0, 255};      // Новый цвет (Красный)

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Начинаем новый фрейм ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Окно интерфейса ImGui
        ImGui::Begin("Tools");
        // Используем радиокнопки для выбора режима
        static int mode = 0; // 0 - карандаш, 1 - заливка, 2 - ластик, 3 - прямоугольник
        ImGui::RadioButton("Pen", &mode, 0);
        ImGui::RadioButton("Fill", &mode, 1);
        ImGui::RadioButton("Fill with image", &mode, 2);
        ImGui::RadioButton("Border", &mode, 3);

        // Устанавливаем флаги в зависимости от выбранного режима
        pencilMode = (mode == 0);
        fillMode = (mode == 1);
        fillWithImageMode = (mode == 2);
        borderMode = (mode == 3);

        // Добавляем слайдер для регулирования толщины линии
        ImGui::SliderInt("Pen size", &penSize, 1, 20);

        // Добавляем кнопку Clear для очистки холста
        if (ImGui::Button("Clear main canvas")) {
            ClearCanvas();
        }
        if (ImGui::Button("Load image")) {
            loadImage("C:/Users/Ly4aznik/Desktop/fruits.jpg");
        }

        ImVec2 canvasFillSize = ImVec2(canvasFillWidth, canvasFillHeight);
        ImVec2 canvasFillPos = ImGui::GetCursorScreenPos();

        // Отображаем текстуру
        ImGui::Image((void*)(intptr_t)canvasFillTexture, canvasFillSize);

        ImGui::End();

        // Окно для рисования с флагом, чтобы его нельзя было перемещать
        ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoMove);

        ImVec2 canvasSize = ImVec2(canvasWidth, canvasHeight);
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();

        // Отображаем текстуру
        ImGui::Image((void*)(intptr_t)canvasTexture, canvasSize);

        // Невидимая кнопка для перехвата событий
        //ImGui::InvisibleButton("canvas", canvasSize, ImGuiButtonFlags_MouseButtonLeft);

        // Проверяем, что курсор находится в области холста и активен режим рисования
        bool isHovered = ImGui::IsItemHovered();
        bool isMouseDown = ImGui::IsMouseDown(0);  // Левая кнопка мыши

        if (isHovered && isMouseDown) {
            ImVec2 mousePos = ImGui::GetMousePos();
            int x = (int)(mousePos.x - canvasPos.x);
            int y = (int)(mousePos.y - canvasPos.y);

            if (pencilMode){
                DrawCircleOnCanvas(x, y, penSize, penColor); // Рисуем круг (жирная линия) на текстуре
            }
            else if (fillMode) {
                FloodFill(x, y, canvasWidth, canvasHeight, canvasData, backgroundColor, fillColor);
                glBindTexture(GL_TEXTURE_2D, canvasTexture);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, canvasData.data());
            }
            else if (fillWithImageMode) {
                fillWithImage(x, y, x, y, canvasWidth, canvasHeight, canvasData, backgroundColor, canvasFillData, loadedImageWidth, loadedImageHeight);
                glBindTexture(GL_TEXTURE_2D, canvasTexture);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, canvasData.data());
            }
            else if (borderMode) {
                HighlightBorder(x,y, canvasData, penColor, borderColor);
            }
        }


        ImGui::End(); // Завершаем окно Canvas

        // Завершаем фрейм ImGui
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Обновляем окно
        glfwSwapBuffers(window);
    }

    // Очистка ресурсов
    glDeleteTextures(1, &canvasTexture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
