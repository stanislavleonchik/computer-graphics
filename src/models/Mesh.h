#pragma once
#include<vector>
#include<cmath>

struct TextureCoord {
    float u, v;
};

struct Point3 {
    float x, y, z;
    TextureCoord texCoord{};

    Point3() : x(0), y(0), z(0) {}
    Point3(float x, float y, float z) : x(x), y(y), z(z) {}

    Point3 operator+(const Point3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    Point3 operator-(const Point3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    Point3 operator-() const {
        return { -x, -y, -z };
    }

    Point3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    bool operator<(const Point3& other) const {
        return memcmp(this, &other, sizeof(Point3)) < 0;
    }

    Point3 cross(const Point3& other) const {
        return {
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
        };
    }

    float dot(const Point3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Point3 normalize() const {
        float len = std::sqrt(x * x + y * y + z * z);
        if (len > 0)
            return { x / len, y / len, z / len };
        return {};
    }
};

struct Polygon3 {
    std::vector<int> vertex_indices;
    std::vector<int> texture_indices;
    std::vector<int> normal_indices;
};

struct Mesh {
    std::vector<Point3> vertices;
    std::vector<Polygon3> polygons;
    std::vector<unsigned int> faceIndices;
    std::vector<unsigned int> edgeIndices;

    void init_edges_faces() {
        faceIndices.clear();
        edgeIndices.clear();
        for (const auto& poly : polygons) {
            for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
                int idx0 = poly.vertex_indices[i];
                int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
                edgeIndices.push_back(idx0);
                edgeIndices.push_back(idx1);
            }
            for (size_t i = 1; i + 1 < poly.vertex_indices.size(); ++i) {
                faceIndices.push_back(poly.vertex_indices[0]);
                faceIndices.push_back(poly.vertex_indices[i]);
                faceIndices.push_back(poly.vertex_indices[i + 1]);
            }
        }
    }
};
