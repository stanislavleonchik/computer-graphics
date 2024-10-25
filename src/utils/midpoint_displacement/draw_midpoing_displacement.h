#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"

#include "../../models/MidpointDisplacementLine.h"
#include <vector>
#include <deque>
#include <random>
#include <cmath>
using std::vector, std::deque;

void draw_line(vector<GLubyte>& data, int width, int height, MidpointDisplacementLine line) {
    int dx = abs(line.x1 - line.x0);
    int dy = abs(line.y1 - line.y0);
    int sx = (line.x0 < line.x1) ? 1 : -1;
    int sy = (line.y0 < line.y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        // Рисуем пиксель на позиции (x0, y0)
        if (line.x0 >= 0 && line.x0 < width && line.y0 >= 0 && line.y0 < height) {
            int index = ((height - line.y0) * width + line.x0) * 4;  // Предполагается, что формат RGB
            data[index] = 0;    // R
            data[index + 1] = 0;  // G
            data[index + 2] = 0;  // B
            data[index + 3] = 255;  // a
        }

        // Если линия завершена
        if (line.x0 == line.x1 && line.y0 == line.y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            line.x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            line.y0 += sy;
        }
    }
}

void draw_midpoint_displacement(deque<MidpointDisplacementLine>& lines, int roughness, int width, int height, vector<GLubyte>& data, GLuint& texture) {
    std::random_device rd;   // Источник случайности
    std::mt19937 gen(rd());  // Генератор на основе Mersenne Twister
    std::uniform_int_distribution<> dis(-1 * roughness, roughness);
    if (lines.empty()) {
        int x_0 = 0;
        int y_0 = rand() % height;

        int x_1 = width;
        int y_1 = rand() % height;

        lines.push_back({x_0, y_0, x_1, y_1});
        draw_line(data, width, height, lines.back());
    }
    else
    {
        size_t lines_count = lines.size();
        for (int i = 0; i < lines_count; i++) {
            MidpointDisplacementLine current_line = lines.front();
            lines.pop_front();
            int length = sqrt(pow(current_line.x1 - current_line.x0, 2) + pow(current_line.y1 - current_line.y0, 2));
            int rand_val = static_cast<int>(dis(gen) * length * 0.1);
            // cout << rand_val << endl;
            int h = (current_line.y1 + current_line.y0)/2 + rand_val;
            h = std::max(std::min(h, height - 1), 1); // Округляем до допустимых значений
            int mid_x = (current_line.x0 + current_line.x1) / 2;
            lines.push_back({current_line.x0, current_line.y0, mid_x, h});
            draw_line(data, width, height, lines.back());
            lines.push_back({mid_x, h, current_line.x1, current_line.y1});
            draw_line(data, width, height, lines.back());
        }

    }



}
