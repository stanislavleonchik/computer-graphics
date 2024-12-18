#pragma once
#include "Mesh.h"
#include "Matrix4x4.h"
#include <cmath>
#include "imgui.h"
#define _USE_MATH_DEFINES
#include <math.h>


static float translation[3] = {0.0f, 0.0f, 0.0f};
static float rotation[3] = {0.0f, 0.0f, 0.0f};
static float scaling[3] = {1.0f, 1.0f, 1.0f};

static bool reflectXY = false;
static bool reflectXZ = false;
static bool reflectYZ = false;

float globalScale = 1.0f;
Point3 figureCenter;

size_t axis = 0;
static float rotation_val[4] = {0.0f, 0.0f, 0.0f, 0.0f};
bool show_custom_vec_window = false;
Point3 xInput, yInput;

void reset_transformations() { // Сбросьте параметры трансформации
    translation[0] = 0.0f;
    translation[1] = 0.0f;
    translation[2] = 0.0f;
    rotation[0] = 0.0f;
    rotation[1] = 0.0f;
    rotation[2] = 0.0f;
    scaling[0] = 1.0f;
    scaling[1] = 1.0f;
    scaling[2] = 1.0f;
    reflectXY = false;
    reflectXZ = false;
    reflectYZ = false;
    globalScale = 1.0f;
    rotation_val[0] = 0.0f;
    rotation_val[1] = 0.0f;
    rotation_val[2] = 0.0f;
    rotation_val[3] = 0.0f;
}

struct cache {
    static float cache_translation[3];
    static float cache_rotation[3];
    static float cache_scaling[3];
    static bool cache_reflectXY;
    static bool cache_reflectXZ;
    static bool cache_reflectYZ;
    static float cache_globalScale;
    static float cache_rotation_val[4];
};

float cache::cache_translation[3] = {0.0f, 0.0f, 0.0f};
float cache::cache_rotation[3] = {0.0f, 0.0f, 0.0f};
float cache::cache_scaling[3] = {1.0f, 1.0f, 1.0f};
bool cache::cache_reflectXY = false;
bool cache::cache_reflectXZ = false;
bool cache::cache_reflectYZ = false;
float cache::cache_globalScale = 1.0f;
float cache::cache_rotation_val[4] = {0.0f, 0.0f, 0.0f, 0.0f};


void cache_transformations() { // Сбросьте параметры трансформации
    cache::cache_translation[0] = translation[0];
    cache::cache_translation[1] = translation[1];
    cache::cache_translation[2] = translation[2];
    cache::cache_rotation[0] = rotation[0];
    cache::cache_rotation[1] = rotation[1];
    cache::cache_rotation[2] = rotation[2];
    cache::cache_scaling[0] = scaling[0];
    cache::cache_scaling[1] = scaling[1];
    cache::cache_scaling[2] = scaling[2];
    cache::cache_reflectXY = reflectXY;
    cache::cache_reflectXZ = reflectXZ;
    cache::cache_reflectYZ = reflectYZ;
    cache::cache_globalScale = globalScale;
    cache::cache_rotation_val[0] = rotation_val[0];
    cache::cache_rotation_val[1] = rotation_val[1];
    cache::cache_rotation_val[2] = rotation_val[2];
    cache::cache_rotation_val[3] = rotation_val[3];
}

void uncache_transformations() {
    translation[0] = cache::cache_translation[0];
    translation[1] = cache::cache_translation[1];
    translation[2] = cache::cache_translation[2];
    rotation[0] = cache::cache_rotation[0];
    rotation[1] = cache::cache_rotation[1];
    rotation[2] = cache::cache_rotation[2];
    scaling[0] = cache::cache_scaling[0];
    scaling[1] = cache::cache_scaling[1];
    scaling[2] = cache::cache_scaling[2];
    reflectXY = cache::cache_reflectXY;
    reflectXZ = cache::cache_reflectXZ;
    reflectYZ = cache::cache_reflectYZ;
    globalScale = cache::cache_globalScale;
    rotation_val[0] = cache::cache_rotation_val[0];
    rotation_val[1] = cache::cache_rotation_val[1];
    rotation_val[2] = cache::cache_rotation_val[2];
    rotation_val[3] = cache::cache_rotation_val[3];
}

//окно для задания вектора, вокруг которого будет крутиться объект
void show_create_custom_vec() {
    ImGui::Begin("Custom Vector", &show_custom_vec_window, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("First point");

    ImGui::PushItemWidth(50);
    ImGui::InputFloat("x1", &xInput.x);
    ImGui::SameLine();
    ImGui::InputFloat("y1", &xInput.y);
    ImGui::SameLine();
    ImGui::InputFloat("z1", &xInput.z);
    ImGui::PopItemWidth();

    ImGui::Text("Second point");

    ImGui::PushItemWidth(50);
    ImGui::InputFloat("x2", &yInput.x);
    ImGui::SameLine();
    ImGui::InputFloat("y2", &yInput.y);
    ImGui::SameLine();
    ImGui::InputFloat("z2", &yInput.z);
    ImGui::PopItemWidth();

    if (ImGui::Button("Ok"))
        show_custom_vec_window = false;

    ImGui::End();
}

Point3 calculateFigureCenter(const std::vector<Point3> &vertices) {
    Point3 center = {0.0f, 0.0f, 0.0f};
    for (const auto &vertex: vertices) {
        center = center + vertex;
    }
    // Получаем среднее значение по каждой оси
    int numVertices = vertices.size();
    center = center * (1.0f / numVertices);
    return center;
}

void make_vec_rotation(Matrix4x4 &model) {
    if (axis == 3 && rotation_val[3] != 0.0f) {
        Point3 vec = yInput - xInput;
        vec = vec.normalize();

        // Перевод модели так, чтобы точка x находилась в начале координат
        Matrix4x4 translateToOrigin = Matrix4x4::translate(-xInput);

        // Поворот модели вокруг вектора vec
        float angle = rotation_val[3] * M_PI / 180.0f;
        Matrix4x4 rotateAroundVector = Matrix4x4::rotation(angle, vec);
        Matrix4x4 translateBack = Matrix4x4::translate(xInput);

        // трансформации
        model = model * translateToOrigin * rotateAroundVector * translateBack;
    }
}

void make_affine_transforms(Matrix4x4 &model, const Mesh &mesh) {
    model = Matrix4x4();

    // Сдвиг
    Matrix4x4 translateMatrix = Matrix4x4::translate(Point3(translation[0], translation[1], translation[2]));

    // Вращение
    Matrix4x4 rotationMatrixX = Matrix4x4::rotation(rotation[0] * M_PI / 180.0f, Point3(1.0f, 0.0f, 0.0f));
    Matrix4x4 rotationMatrixY = Matrix4x4::rotation(rotation[1] * M_PI / 180.0f, Point3(0.0f, 1.0f, 0.0f));
    Matrix4x4 rotationMatrixZ = Matrix4x4::rotation(rotation[2] * M_PI / 180.0f, Point3(0.0f, 0.0f, 1.0f));

    Matrix4x4 rotationMatrix = rotationMatrixZ * rotationMatrixY * rotationMatrixX;

    // Масштабирование
    Matrix4x4 scaleMatrix = Matrix4x4::scale(Point3(scaling[0], scaling[1], scaling[2]));

    // Отражение
    Matrix4x4 reflectionMatrix = Matrix4x4();
    if (reflectXY) {
        reflectionMatrix = Matrix4x4::scale(Point3(1.0f, 1.0f, -1.0f));
    } else if (reflectXZ) {
        reflectionMatrix = Matrix4x4::scale(Point3(1.0f, -1.0f, 1.0f));
    } else if (reflectYZ) {
        reflectionMatrix = Matrix4x4::scale(Point3(-1.0f, 1.0f, 1.0f));
    }

    // Глобальное масштабирование
    Matrix4x4 globalScaleMatrix = Matrix4x4::scale(Point3(globalScale, globalScale, globalScale));

    // Вращение вокруг центра
    Matrix4x4 centerRotationMatrix = Matrix4x4();
    if (axis != 3 && rotation_val[axis] != 0.0f) {
        float angle = rotation_val[axis] * M_PI / 180.0f;
        Point3 rotationAxis;
        if (axis == 0) rotationAxis = Point3(1.0f, 0.0f, 0.0f);
        if (axis == 1) rotationAxis = Point3(0.0f, 1.0f, 0.0f);
        if (axis == 2) rotationAxis = Point3(0.0f, 0.0f, 1.0f);

        // Вычисляем центр фигуры
        figureCenter = calculateFigureCenter(mesh.vertices);

        // Перевести в центр, повернуть, перевести обратно
        Matrix4x4 translateToCenter = Matrix4x4::translate(-figureCenter);
        Matrix4x4 rotateAroundAxis = Matrix4x4::rotation(angle, rotationAxis);
        Matrix4x4 translateBack = Matrix4x4::translate(figureCenter);

        centerRotationMatrix = translateToCenter * rotateAroundAxis * translateBack;
    }

    // Применение всех преобразований к матрице модели
    model = model
            * translateMatrix
            * rotationMatrix
            * scaleMatrix
            * reflectionMatrix
            * globalScaleMatrix
            * centerRotationMatrix;

    // Вращение вокруг пользовательского вектора
    make_vec_rotation(model);
}

void create_affine_tools(bool& is_shown) {

    ImGui::Begin("Affine Transformations", &is_shown); // Запуск окна ImGui

    ImGui::Text("Translation"); // Сдвиг
    ImGui::SliderFloat("Shift X", &translation[0], -5.0f, 5.0f);
    ImGui::SliderFloat("Shift Y", &translation[1], -5.0f, 5.0f);
    ImGui::SliderFloat("Shift Z", &translation[2], -5.0f, 5.0f);

    ImGui::Separator();

    ImGui::Text("Rotation (degrees)"); // Вращение
    ImGui::SliderFloat("Pitch", &rotation[0], -180.0f, 180.0f);
    ImGui::SliderFloat("Yaw", &rotation[1], -180.0f, 180.0f);
    ImGui::SliderFloat("Roll", &rotation[2], -180.0f, 180.0f);

    ImGui::Separator();

    ImGui::Text("Scaling"); // Масштабирование
    ImGui::SliderFloat("Scale X", &scaling[0], 0.1f, 5.0f);
    ImGui::SliderFloat("Scale Y", &scaling[1], 0.1f, 5.0f);
    ImGui::SliderFloat("Scale Z", &scaling[2], 0.1f, 5.0f);

    ImGui::Separator();

    ImGui::Text("Reflection"); // Отражение
    if (ImGui::Checkbox("XY", &reflectXY)) {
        // Сбросьте другие отражения, если выбрано это.
        if (reflectXY) {
            reflectXZ = false;
            reflectYZ = false;
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("XZ", &reflectXZ)) {
        if (reflectXZ) {
            reflectXY = false;
            reflectYZ = false;
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("YZ", &reflectYZ)) {
        if (reflectYZ) {
            reflectXY = false;
            reflectXZ = false;
        }
    }

    ImGui::Separator();

    // Ползунок глобального масштабирования
    ImGui::Text("Global Scale");
    ImGui::SliderFloat("Global Scale", &globalScale, 0.1f, 5.0f);

    ImGui::Separator();

    ImGui::Text("Rotation around center");

    ImGui::Text("Choose an axis");
    if (ImGui::RadioButton("X", axis == 0)) axis = 0;
    ImGui::SameLine();
    if (ImGui::RadioButton("Y", axis == 1)) axis = 1;
    ImGui::SameLine();
    if (ImGui::RadioButton("Z", axis == 2)) axis = 2;
    ImGui::SameLine();
    if (ImGui::RadioButton("Custom Vector", axis == 3)) {
        show_custom_vec_window = true;
        axis = 3;
    }

    ImGui::Text("Rotation Value:");
    ImGui::SliderFloat("Rotation Value", &rotation_val[axis], -180.0f, 180.0f);

    if (show_custom_vec_window)
        show_create_custom_vec();

    ImGui::Separator();

    if (ImGui::Button("Reset"))
        reset_transformations();

    ImGui::End(); // Завершение работы окна ImGui
}
