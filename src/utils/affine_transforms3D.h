#ifndef AFFIEN_TRANS_3D_H
#define AFFINE_TRANS_3D_H

#include "Mesh.h"
#include "Matrix4x4.h"

#define _USE_MATH_DEFINES
#include<math.h>

static float translation[3] = { 0.0f, 0.0f, 0.0f };
static float rotation[3] = { 0.0f, 0.0f, 0.0f };
static float scaling[3] = { 1.0f, 1.0f, 1.0f };

static bool reflectXY = false;
static bool reflectXZ = false;
static bool reflectYZ = false;

float globalScale = 1.0f;
Point3 figureCenter;

size_t axis = 0;
static float rotation_val[4] = { 0.0, 0.0, 0.0 , 0.0 };
bool show_custom_vec_window = false;
Point3 x, y;

void reset_transformations() { //сброс параметров
    translation[0] = 0.0; translation[1] = 0.0; translation[2] = 0.0;
    rotation[0] = 0.0; rotation[1] = 0.0; rotation[2] = 0.0;
    scaling[0] = 1.0; scaling[1] = 1.0; scaling[2] = 1.0;
    reflectXY = false;
    reflectXZ = false;
    reflectYZ = false;
    globalScale = 1.0f;
    rotation_val[0] = 0.0; rotation_val[1] = 0.0; rotation_val[2] = 0.0; rotation_val[3] = 0.0;
}


//окно для задания вектора, вокруг которого будет крутиться объект
void show_create_custom_vec() {
    ImGui::Begin("Custom Vector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("First point");

    ImGui::PushItemWidth(50);
    ImGui::InputFloat("x1", &x.x);
    ImGui::SameLine();
    ImGui::InputFloat("y1", &x.y);
    ImGui::SameLine();
    ImGui::InputFloat("z1", &x.z);
    ImGui::PopItemWidth();

    ImGui::Text("Second point");

    ImGui::PushItemWidth(50);
    ImGui::InputFloat("x2", &y.x);
    ImGui::SameLine();
    ImGui::InputFloat("y2", &y.y);
    ImGui::SameLine();
    ImGui::InputFloat("z2", &y.z);
    ImGui::PopItemWidth();

    if (ImGui::Button("Ok"))
        show_custom_vec_window = false;

    ImGui::End();
}

//вычисление центра фигуры
Point3 calculateFigureCenter(const std::vector<Point3>& vertices) {
    Point3 center = { 0.0f, 0.0f, 0.0f };
    for (const auto& vertex : vertices) {
        center.x += vertex.x;
        center.y += vertex.y;
        center.z += vertex.z;
    }
    // Получаем среднее значение по каждой оси
    int numVertices = vertices.size();
    center.x /= numVertices;
    center.y /= numVertices;
    center.z /= numVertices;
    return center;
}

void make_axis_rotation(Point3 center, Matrix4x4& transforms)
{
    Point3 vec = x - y;
    vec = vec.normalize();
    transforms = Matrix4x4::translate(Point3(-x.x, -x.y, -x.z)) * transforms;
    transforms = Matrix4x4::rotation(rotation_val[3] * M_PI / 180.0f, vec) * transforms;
    transforms = Matrix4x4::translate(x) * transforms;
}

void make_affine_transforms(Matrix4x4& model, Mesh mesh) {

    figureCenter = calculateFigureCenter(mesh.vertices);
    model = Matrix4x4::translate(Point3(-figureCenter.x, -figureCenter.y, -figureCenter.z)) * model;

    // Шаг 2: Применяем масштабирование и поворот
    model = Matrix4x4::scale(Point3(scaling[0] * globalScale, scaling[1] * globalScale, scaling[2] * globalScale)) * model;
    model = Matrix4x4::rotation(rotation_val[0] * M_PI / 180.0, Point3(1.0, 0.0, 0.0)) * model;
    model = Matrix4x4::rotation(rotation_val[1] * M_PI / 180.0, Point3(0.0, 1.0, 0.0)) * model;
    model = Matrix4x4::rotation(rotation_val[2] * M_PI / 180.0, Point3(0.0, 0.0, 1.0)) * model;

    // Шаг 3: Возвращаем фигуру обратно на её исходное место
    model = Matrix4x4::translate(figureCenter) * model;

    if (reflectXY) { // Отражение относительно плоскости XY
        Matrix4x4 reflectMatrix = Matrix4x4::scale(Point3(1.0f, 1.0f, -1.0f));
        model = reflectMatrix * model;
        //reflectXY = false;
    }

    if (reflectXZ) { // Отражение относительно плоскости XZ
        Matrix4x4 reflectMatrix = Matrix4x4::scale(Point3(1.0f, -1.0f, 1.0f));
        model = reflectMatrix * model;
        //reflectXZ = false;
    }

    if (reflectYZ) { // Отражение относительно плоскости YZ
        Matrix4x4 reflectMatrix = Matrix4x4::scale(Point3(-1.0f, 1.0f, 1.0f));
        model = reflectMatrix * model;
        //reflectYZ = false;
    }


    model = Matrix4x4::scale(Point3(scaling[0], scaling[1], scaling[2])) * model; // Применение масштабирования

    model = Matrix4x4::rotation(rotation[0] * M_PI / 180.0f, Point3(1.0f, 0.0f, 0.0f)) * model; // Применение вращения (последовательно вокруг осей X, Y, Z)
    model = Matrix4x4::rotation(rotation[1] * M_PI / 180.0f, Point3(0.0f, 1.0f, 0.0f)) * model;
    model = Matrix4x4::rotation(rotation[2] * M_PI / 180.0f, Point3(0.0f, 0.0f, 1.0f)) * model;

    model = Matrix4x4::translate(Point3(translation[0], translation[1], translation[2])) * model; // Применение смещения
}

void create_affine_tools(bool is_shown) {


    ImGui::Begin("Affine Transformations", &is_shown); // Начало окна ImGui

    ImGui::Text("Translation"); // Смещение
    ImGui::SliderFloat("X", &translation[0], -5.0f, 5.0f);
    ImGui::SliderFloat("Y", &translation[1], -5.0f, 5.0f);
    ImGui::SliderFloat("Z", &translation[2], -5.0f, 5.0f);

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
    if (ImGui::Checkbox("Reflect XY", &reflectXY)) {
        // Сбрасываем другие отражения, если выбрано это
        if (reflectXY) { reflectXZ = false; reflectYZ = false; }
    }
    if (ImGui::Checkbox("Reflect XZ", &reflectXZ)) {
        if (reflectXZ) { reflectXY = false; reflectYZ = false; }
    }
    if (ImGui::Checkbox("Reflect YZ", &reflectYZ)) {
        if (reflectYZ) { reflectXY = false; reflectXZ = false; }
    }
    ImGui::Separator();

    // Добавим слайдер для глобального масштаба
    ImGui::Text("Global Scale"); // Глобальное масштабирование
    ImGui::SliderFloat("Global Scale", &globalScale, 0.1f, 5.0f);

    ImGui::Separator();

    ImGui::Text("Rotation around center");

    ImGui::Text("Choose an axis");
    if (ImGui::RadioButton("x", axis == 0)) axis = 0;
    ImGui::SameLine();
    if (ImGui::RadioButton("y", axis == 1)) axis = 1;
    ImGui::SameLine();
    if (ImGui::RadioButton("z", axis == 2)) axis = 2;
    if (ImGui::RadioButton("custom vector", axis == 3)) {
        show_custom_vec_window = true;
        axis = 3;
    }

    ImGui::Text("Rotation value: ");
    ImGui::SliderFloat("Roatation value", &rotation_val[axis], -180.0, 180.0);

    if (show_custom_vec_window)
        show_create_custom_vec();

    if (ImGui::Button("Reset"))
        reset_transformations();


    ImGui::End(); // Конец окна ImGui
    
}

#endif // !AFFIEN_TRANS_3D_H
