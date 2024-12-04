#ifndef CREATE_POLYH_H
#define CREATE_POLYH_H

#include"Mesh.h"

Mesh createTetrahedron() { // Функция для создания тетраэдра
    Mesh mesh;
    float sqrt2 = std::sqrt(2.0f);

    mesh.vertices = {
            Point3(1, 0, -1 / sqrt2),
            Point3(-1, 0, -1 / sqrt2),
            Point3(0, 1, 1 / sqrt2),
            Point3(0, -1, 1 / sqrt2)
    };

    mesh.polygons = {
            {{0, 1, 2}},
            {{0, 1, 3}},
            {{0, 2, 3}},
            {{1, 2, 3}}
    };

    mesh.faceIndices.clear();
    mesh.edgeIndices.clear();
    for (const auto& poly : mesh.polygons) {
        // Create edge indices
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.edgeIndices.push_back(idx0);
            mesh.edgeIndices.push_back(idx1);
        }
        // Create face indices (triangulate the polygon)
        for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
            mesh.faceIndices.push_back(poly.vertex_indices[0]);
            mesh.faceIndices.push_back(poly.vertex_indices[i]);
            mesh.faceIndices.push_back(poly.vertex_indices[i + 1]);
        }
    }

    return mesh;
}

Mesh createHexahedron() { // Функция для создания гексаэдра (куба)
    Mesh mesh;

    mesh.vertices = {
            Point3(-0.5f, -0.5f, -0.5f),
            Point3(0.5f, -0.5f, -0.5f),
            Point3(0.5f,  0.5f, -0.5f),
            Point3(-0.5f,  0.5f, -0.5f),
            Point3(-0.5f, -0.5f,  0.5f),
            Point3(0.5f, -0.5f,  0.5f),
            Point3(0.5f,  0.5f,  0.5f),
            Point3(-0.5f,  0.5f,  0.5f)
    };

    mesh.polygons = {
            {{0, 1, 2, 3}},
            {{4, 5, 6, 7}}, // Задняя грань
            {{0, 1, 5, 4}}, // Передняя грань
            {{2, 3, 7, 6}}, // Нижняя грань
            {{0, 3, 7, 4}}, // Верхняя грань
            {{1, 2, 6, 5}} // Левая грань
    }; // Правая грань

    mesh.faceIndices.clear();
    mesh.edgeIndices.clear();
    for (const auto& poly : mesh.polygons) {
        // Create edge indices
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.edgeIndices.push_back(idx0);
            mesh.edgeIndices.push_back(idx1);
        }
        // Create face indices (triangulate the polygon)
        for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
            mesh.faceIndices.push_back(poly.vertex_indices[0]);
            mesh.faceIndices.push_back(poly.vertex_indices[i]);
            mesh.faceIndices.push_back(poly.vertex_indices[i + 1]);
        }
    }

    return mesh;
}

Mesh createOctahedron() { // Функция для создания октаэдра
    Mesh mesh;

    mesh.vertices = {
            Point3(1, 0, 0),
            Point3(-1, 0, 0),
            Point3(0, 1, 0),
            Point3(0, -1, 0),
            Point3(0, 0, 1),
            Point3(0, 0, -1)
    };

    mesh.polygons = {
            {{0, 2, 4}},
            {{2, 1, 4}},
            {{1, 3, 4}},
            {{3, 0, 4}},
            {{0, 2, 5}},
            {{2, 1, 5}},
            {{1, 3, 5}},
            {{3, 0, 5}}
    };

    mesh.faceIndices.clear();
    mesh.edgeIndices.clear();
    for (const auto& poly : mesh.polygons) {
        // Create edge indices
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.edgeIndices.push_back(idx0);
            mesh.edgeIndices.push_back(idx1);
        }
        // Create face indices (triangulate the polygon)
        for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
            mesh.faceIndices.push_back(poly.vertex_indices[0]);
            mesh.faceIndices.push_back(poly.vertex_indices[i]);
            mesh.faceIndices.push_back(poly.vertex_indices[i + 1]);
        }
    }

    return mesh;
}

Mesh createIcosahedron() { // Функция для создания икосаэдра
    Mesh mesh;
    const float t = (1.0 + std::sqrt(5.0)) / 2.0;

    mesh.vertices = {
            Point3(-1,  t,  0),
            Point3(1,  t,  0),
            Point3(-1, -t,  0),
            Point3(1, -t,  0),
            Point3(0, -1,  t),
            Point3(0,  1,  t),
            Point3(0, -1, -t),
            Point3(0,  1, -t),
            Point3(t,  0, -1),
            Point3(t,  0,  1),
            Point3(-t,  0, -1),
            Point3(-t,  0,  1)
    };

    mesh.polygons = {
            {{0, 11, 5}}, {{0, 5, 1}}, {{0, 1, 7}}, {{0, 7, 10}}, {{0, 10, 11}},
            {{1, 5, 9}}, {{5, 11, 4}}, {{11, 10, 2}}, {{10, 7, 6}}, {{7, 1, 8}},
            {{3, 9, 4}}, {{3, 4, 2}}, {{3, 2, 6}}, {{3, 6, 8}}, {{3, 8, 9}},
            {{4, 9, 5}}, {{2, 4, 11}}, {{6, 2, 10}}, {{8, 6, 7}}, {{9, 8, 1}}
    };

    for (auto& v : mesh.vertices) { // Нормализация вершин для единичной сферы
        v = v.normalize();
    }

    mesh.faceIndices.clear();
    mesh.edgeIndices.clear();
    for (const auto& poly : mesh.polygons) {
        // Create edge indices
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.edgeIndices.push_back(idx0);
            mesh.edgeIndices.push_back(idx1);
        }
        // Create face indices (triangulate the polygon)
        for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
            mesh.faceIndices.push_back(poly.vertex_indices[0]);
            mesh.faceIndices.push_back(poly.vertex_indices[i]);
            mesh.faceIndices.push_back(poly.vertex_indices[i + 1]);
        }
    }

    return mesh;
}

Mesh createDodecahedron() { // Функция для создания додекаэдра
    Mesh mesh;
    const float phi = (1.0 + std::sqrt(5.0)) / 2.0;
    const float a = 1.0f; // Золотое сечение
    const float b = 1.0f / phi;
    const float c = 2.0f - phi;

    mesh.vertices = { // Координаты вершин додекаэдра
            { c,  0,  a},
            {-c,  0,  a}, // 1
            {-b,  b,  b}, // 2
            { 0,  a,  c}, // 3
            { b,  b,  b}, // 4
            { b, -b,  b}, // 5
            { 0, -a,  c}, // 6
            {-b, -b,  b}, // 7
            { c,  0, -a}, // 8
            {-c,  0, -a}, // 9
            {-b, -b, -b}, // 10
            { 0, -a, -c}, // 11
            { b, -b, -b}, // 12
            { b,  b, -b}, // 13
            { 0,  a, -c}, // 14
            {-b,  b, -b}, // 15
            { a,  c,  0}, // 16
            {-a,  c,  0}, // 17
            {-a, -c,  0}, // 18
            { a, -c,  0} // 19
    }; // 20

    for (auto& v : mesh.vertices) { // Нормализация вершин
        v = v.normalize();
    }

    mesh.polygons = { // Грани додекаэдра (каждая грань - пятиугольник)
            {{0, 1, 2, 3, 4}},
            {{0, 5, 6, 7, 1}},
            {{0, 4, 16, 19, 5}},
            {{1, 7, 18, 17, 2}},
            {{2, 17, 15, 14, 3}},
            {{3, 14, 13, 16, 4}},
            {{6, 11}},
            {{8, 9, 10, 11, 12}},
            {{8, 13, 14, 15, 9}},
            {{8, 12, 19, 16, 13}},
            {{9, 15, 17, 18, 10}},
    };

    mesh.faceIndices.clear();
    mesh.edgeIndices.clear();
    for (const auto& poly : mesh.polygons) {
        // Create edge indices
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.edgeIndices.push_back(idx0);
            mesh.edgeIndices.push_back(idx1);
        }
        // Create face indices (triangulate the polygon)
        for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
            mesh.faceIndices.push_back(poly.vertex_indices[0]);
            mesh.faceIndices.push_back(poly.vertex_indices[i]);
            mesh.faceIndices.push_back(poly.vertex_indices[i + 1]);
        }
    }

    return mesh;
}


#endif // !CREATE_POLYH_H
