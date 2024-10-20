#include "includes.h"

using std::vector;
using std::map;
using std::array;
using std::cout;
using std::endl;

//AppState cur_state = AppState::MainMenu;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
map<int, Line> YXColor;

//void render() {
//    switch (cur_state) {
//    case AppState::MainMenu:
//        drawMainMenu();
//        break;
//    case AppState::Lab03:
//        GradTr();
//        break;
//    case AppState::Lab04:
//        renderScreen2();
//        break;
//    }
//}

struct Point {
    float x, y;
};

Point p1 = { 50, 550 };
Point p2 = { 750, 550 };
Point p3 = { 400, 50 };

void fillYXarray(int x1, int y1, int x2, int y2, array<short,3> c1, array<short,3> c2) {

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    double z = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;


    int dR = c2[0] - c1[0];
    int dG = c2[1] - c1[1];
    int dB = c2[2] - c1[2];

    while (true) {
        double cur_dist = 1 - sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)) / z;
        if (cur_dist < 0)
            cur_dist = 0;
        else if (cur_dist > 1)
            cur_dist = 1;

        short cur_c[3];
        cur_c[0] = c1[0] + round(dR * cur_dist);
        cur_c[1] = c1[1] + round(dG * cur_dist);
        cur_c[2] = c1[2] + round(dB * cur_dist);

        if (YXColor.find(y1) != YXColor.end()) {
            YXColor[y1] = Line(YXColor[y1].x1, x1, YXColor[y1].c1, cur_c);
        }
        else {
            short black[3] = { 0, 0, 0 };
            YXColor[y1] = Line(x1, INT32_MAX, cur_c, black);
        }


        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}


array<short, 3> getRandomColor() {
    array<short, 3> c;
    c[0] = rand() % 256;
    c[1] = rand() % 256;
    c[2] = rand() % 256;
    return c;
}

void drawPixel(ImDrawList* drawList, float x, float y, ImU32 color) {
    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), color);
}

void rasterizeTriangle(ImDrawList* drawList, array<short, 3> c1, array<short, 3> c2, array<short, 3> c3) {
    
    fillYXarray(p1.x, p1.y, p2.x, p2.y, c1, c2);
    fillYXarray(p2.x, p2.y, p3.x, p3.y, c2, c3);
    fillYXarray(p3.x, p3.y, p1.x, p1.y, c3, c1);

    for (auto x : YXColor) {
        auto line = x.second.GetGradienLine(x.first);
        for (auto pixcel : line) {
            drawPixel(drawList, pixcel.first, x.first, pixcel.second);
        }
    }
    YXColor.clear();
}


int main()
{
    srand(static_cast<unsigned int>(time(0)));
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");


    ImU32 triangleColor = IM_COL32(255, 0, 0, 255); 
    bool first = true;

    auto c1 = getRandomColor();
    auto c2 = getRandomColor();
    auto c3 = getRandomColor();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Lab03");
        ImGui::Text("Gradient Triangle");
        
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        
        rasterizeTriangle(drawList, c1, c2, c3);

        if (ImGui::Button("Change Triangle Color")) {
            c1 = getRandomColor();
            c2 = getRandomColor();
            c3 = getRandomColor();
            rasterizeTriangle(drawList, c1, c2, c3);
            cout << c1[0] << " " << c1[1] << " " << c1[2] << endl;
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка ресурсов
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}