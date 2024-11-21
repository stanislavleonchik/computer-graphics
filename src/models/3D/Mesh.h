#pragma once
#include <vector>
#include <cmath>
#include "Point3.h"
#include "TextureCoord.h"
#include "Polygon3.h"
#include "VertexNormal.h"

struct Color3 {
    float r, g, b;
};

// Обновленная структура Mesh с новым именем для вектора цветов
struct Mesh {
    std::vector<Point3> vertices;
    std::vector<TextureCoord> texture_coords;
    std::vector<VertexNormal> vertex_normals;
    std::vector<Polygon3> polygons;
    std::vector<Color3> colors; // Обновленное имя: Color3
};

Mesh create_tetrahedron() {
    Mesh mesh;
    // Определяем вершины
    float sqrt_2 = std::sqrt(2.0f);
    mesh.vertices = {
            {1, 0, -1 / sqrt_2},
            {-1, 0, -1 / sqrt_2},
            {0, 1, 1 / sqrt_2},
            {0, -1, 1 / sqrt_2}
    };

    // Определяем цвета для каждой вершины
    mesh.colors = {
            {1.0f, 0.0f, 0.0f}, // Красный
            {0.0f, 1.0f, 0.0f}, // Зеленый
            {0.0f, 0.0f, 1.0f}, // Синий
            {1.0f, 1.0f, 0.0f}  // Желтый
    };

    // Определяем грани (полигоны)
    mesh.polygons = {
            {{0, 1, 2}},
            {{0, 3, 1}},
            {{1, 3, 2}},
            {{2, 3, 0}}
    };

    return mesh;
}

Mesh create_hexahedron() {
    Mesh mesh;
    mesh.vertices = {
            {-1, -1, -1},
            {1, -1, -1},
            {1,  1, -1},
            {-1, 1, -1},
            {-1, -1, 1},
            {1, -1, 1},
            {1,  1, 1},
            {-1, 1, 1}
    };
    mesh.polygons = {
            {{0, 1, 2, 3}},
            {{1, 5, 6, 2}},
            {{5, 4, 7, 6}},
            {{4, 0, 3, 7}},
            {{3, 2, 6, 7}},
            {{0, 4, 5, 1}}
    };
    return mesh;
}

Mesh create_octahedron() {
    Mesh mesh;
    mesh.vertices = {
            {1, 0, 0},
            {-1, 0, 0},
            {0, 1, 0},
            {0, -1, 0},
            {0, 0, 1},
            {0, 0, -1}
    };
    mesh.polygons = {
            {{0, 2, 4}},
            {{2, 1, 4}},
            {{1, 3, 4}},
            {{3, 0, 4}},
            {{0, 5, 2}},
            {{2, 5, 1}},
            {{1, 5, 3}},
            {{3, 5, 0}}
    };
    return mesh;
}

Mesh create_icosahedron() {
    Mesh mesh;
    const float t = (1.0 + std::sqrt(5.0)) / 2.0;

    mesh.vertices = {
            {-1,  t,  0},
            { 1,  t,  0},
            {-1, -t,  0},
            { 1, -t,  0},
            { 0, -1,  t},
            { 0,  1,  t},
            { 0, -1, -t},
            { 0,  1, -t},
            { t,  0, -1},
            { t,  0,  1},
            {-t,  0, -1},
            {-t,  0,  1}
    };

    mesh.polygons = {
            {{0, 11, 5}},
            {{0, 5, 1}},
            {{0, 1, 7}},
            {{0, 7, 10}},
            {{0, 10, 11}},
            {{1, 5, 9}},
            {{5, 11, 4}},
            {{11, 10, 2}},
            {{10, 7, 6}},
            {{7, 1, 8}},
            {{3, 9, 4}},
            {{3, 4, 2}},
            {{3, 2, 6}},
            {{3, 6, 8}},
            {{3, 8, 9}},
            {{4, 9, 5}},
            {{2, 4, 11}},
            {{6, 2, 10}},
            {{8, 6, 7}},
            {{9, 8, 1}}
    };
    return mesh;
}

Mesh create_dodecahedron() {
    Mesh mesh;
    const float phi = (1.0 + std::sqrt(5.0)) / 2.0;
    const float a = 1.0f;
    const float b = 1.0f / phi;
    const float c = 2.0f - phi;

    mesh.vertices = {
            { c,  0,  a},
            {-c,  0,  a},
            {-b,  b,  b},
            { 0,  a,  c},
            { b,  b,  b},
            { b, -b,  b},
            { 0, -a,  c},
            {-b, -b,  b},
            { c,  0, -a},
            {-c,  0, -a},
            {-b, -b, -b},
            { 0, -a, -c},
            { b, -b, -b},
            { b,  b, -b},
            { 0,  a, -c},
            {-b,  b, -b},
            { a,  c,  0},
            {-a,  c,  0},
            {-a, -c,  0},
            { a, -c,  0}
    };

    mesh.polygons = {
            {{0, 1, 2, 3, 4}},
            {{0, 5, 6, 7, 1}},
            {{0, 4, 16, 19, 5}},
            {{1, 7, 18, 17, 2}},
            {{2, 17, 15, 14, 3}},
            {{3, 14, 13, 16, 4}},
            {{5, 19, 12, 6}},
            {{6, 12, 11, 10, 7}},
            {{7, 10, 18}},
            {{8, 9, 10, 11, 12}},
            {{8, 13, 14, 15, 9}},
            {{8, 12, 19, 16, 13}},
            {{9, 15, 17, 18, 10}},
            {{20, 19, 16, 13, 12}},
            {{20, 12, 6, 5, 19}}
    };
    return mesh;
}