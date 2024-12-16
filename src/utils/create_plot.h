//
// Created by Ly4aznik on 03.12.2024.
//

#ifndef CREATE_PLOT_H
#define CREATE_PLOT_H

#include "Mesh.h"
#include <functional> // Для передачи функции f(x, y)

struct SurfaceParams {
    std::function<float(float, float)> function; // Функция поверхности
    float x_min, x_max;                          // Диапазон по x
    float y_min, y_max;                          // Диапазон по y
    int resolution;                              // Разрешение сетки
    std::string name;                            // Имя поверхности (необязательно)

    // Конструктор для удобства создания структуры
    SurfaceParams(std::function<float(float, float)> func,
                  float x0, float x1,
                  float y0, float y1,
                  int res,
                  std::string surface_name = "Unnamed Surface")
        : function(std::move(func)), x_min(x0), x_max(x1),
          y_min(y0), y_max(y1), resolution(res), name(std::move(surface_name)) {}
};

// Функция для создания сегмента поверхности
Mesh createSurfaceSegment(const SurfaceParams& params) {
    Mesh mesh;
    if (params.resolution < 1) return mesh;

    float dx = (params.x_max - params.x_min) / params.resolution;
    float dy = (params.y_max - params.y_min) / params.resolution;

    // Создаём вершины
    for (int i = 0; i <= params.resolution; ++i) {
        float x = params.x_min + i * dx;
        for (int j = 0; j <= params.resolution; ++j) {
            float y = params.y_min + j * dy;
            float z = params.function(x, y); // Вычисление z по заданной функции
            mesh.vertices.emplace_back(x, y, z);
        }
    }

    // Создаём полигоны (треугольники)
    for (int i = 0; i < params.resolution; ++i) {
        for (int j = 0; j < params.resolution; ++j) {
            int v0 = i * (params.resolution + 1) + j;
            int v1 = v0 + 1;
            int v2 = v0 + (params.resolution + 1);
            int v3 = v2 + 1;

            // Первый треугольник
            mesh.polygons.push_back({{v0, v1, v2}});
            // Второй треугольник
            mesh.polygons.push_back({{v1, v3, v2}});
        }
    }

    // Создаём индексы для рёбер
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

void create_surface_menu(bool& is_shown, SurfaceParams& params, Mesh& current_mesh) {
    static int selected_function = 0; // Выбранная функция

    // Привязываем начальные значения диапазонов и разрешения к params
    static float x_range[2] = { params.x_min, params.x_max }; // Диапазон по X
    static float y_range[2] = { params.y_min, params.y_max }; // Диапазон по Y
    static int resolution = params.resolution; // Частота разбиения

    ImGui::Begin("Surface Settings", &is_shown); // Начало окна ImGui

    ImGui::Text("Range Settings"); // Настройки диапазонов
    ImGui::SliderFloat2("X Range", x_range, -10.0f, 10.0f);
    ImGui::SliderFloat2("Y Range", y_range, -10.0f, 10.0f);

    ImGui::Separator();

    ImGui::Text("Resolution"); // Частота разбиения
    ImGui::SliderInt("Divisions", &resolution, 1, 200);

    ImGui::Separator();

    ImGui::Text("Choose Function"); // Выбор функции
    const char* functions[] = {
        "f(x, y) = sin(x) * cos(y)",
        "f(x, y) = x^2 - y^2",
        "f(x, y) = sqrt(x^2 + y^2)",
        "f(x, y) = exp(-x^2 - y^2)"
    };
    ImGui::Combo("Function", &selected_function, functions, IM_ARRAYSIZE(functions));

    ImGui::Separator();

    if (ImGui::Button("Build Surface")) {
        // Здесь вызов функции для создания поверхности с выбранными параметрами
        std::function<float(float, float)> chosen_function;

        switch (selected_function) {
        case 0: // f(x, y) = sin(x) * cos(y)
            chosen_function = [](float x, float y) { return std::sin(x) * std::cos(y); };
            params.name = "f(x, y) = sin(x) * cos(y)";
            break;
        case 1: // f(x, y) = x^2 - y^2
            chosen_function = [](float x, float y) { return x * x - y * y; };
            params.name = "f(x, y) = x^2 - y^2";
            break;
        case 2: // f(x, y) = sqrt(x^2 + y^2)
            chosen_function = [](float x, float y) { return std::sqrt(x * x + y * y); };
            params.name = "f(x, y) = sqrt(x^2 + y^2)";
            break;
        case 3: // f(x, y) = exp(-x^2 - y^2)
            chosen_function = [](float x, float y) { return std::exp(-x * x - y * y); };
            params.name = "f(x, y) = exp(-x^2 - y^2)";
            break;
        }

        // Обновляем параметры
        params.function = chosen_function;
        params.x_min = x_range[0];
        params.x_max = x_range[1];
        params.y_min = y_range[0];
        params.y_max = y_range[1];
        params.resolution = resolution;

        // Создаём поверхность с обновлёнными параметрами
        current_mesh = createSurfaceSegment(params);


    }

    if (ImGui::Button("Reset Settings")) {
        x_range[0] = -5.0f; x_range[1] = 5.0f;
        y_range[0] = -5.0f; y_range[1] = 5.0f;
        resolution = 50;
        selected_function = 0;
    }

    ImGui::End();
}


#endif //CREATE_PLOT_H
