#ifndef MESH_H
#define MESH_H
#include<vector>
#include<cmath>

struct Point3 { // Структура для представления точки в 3D пространстве
    float x, y, z;

    Point3() : x(0), y(0), z(0) {}
    Point3(float x, float y, float z) : x(x), y(y), z(z) {}

    Point3 operator+(const Point3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    Point3 operator-(const Point3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    Point3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    Point3 cross(const Point3& other) const { // Векторное произведение
        return {
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
        };
    }

    float dot(const Point3& other) const { // Скалярное произведение
        return x * other.x + y * other.y + z * other.z;
    }

    Point3 normalize() const { // Нормализация вектора
        float len = std::sqrt(x * x + y * y + z * z);
        if (len > 0)
            return { x / len, y / len, z / len };
        return {};
    }

    float abs() const {
        return std::sqrt(x * x + y * y + z * z);
    }
};

struct Polygon3 { // Структура полигона
    std::vector<unsigned> vertex_indices;
};

struct Mesh { // Меш
    std::vector<Point3> vertices;
    std::vector<Polygon3> polygons;
    std::vector<unsigned int> indices;
}; // Индексы для отрисовки

#endif // !MESH_H
