#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "../models/Mesh.h"

Mesh loadOBJ(const std::string& path) {
    Mesh mesh;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << path << std::endl;
        return mesh;
    }

    std::vector<Point3> temp_vertices;
    std::vector<Point3> temp_normals;
    std::vector<TextureCoord> temp_texcoords;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            // Координаты вершин
            float x, y, z;
            ss >> x >> y >> z;
            temp_vertices.emplace_back(x, y, z);
        } else if (prefix == "vt") {
            // Координаты текстур
            float u, v;
            ss >> u >> v;
            temp_texcoords.push_back({u, v, 0.0f});
        } else if (prefix == "vn") {
            // Нормали
            float x, y, z;
            ss >> x >> y >> z;
            temp_normals.push_back({x, y, z});
        } else if (prefix == "f") {
            // Грани
            Polygon3 poly;
            std::string vertex_info;
            while (ss >> vertex_info) {
                std::stringstream vertex_ss(vertex_info);
                std::string index_str;
                int vertex_index = -1, texcoord_index = -1, normal_index = -1;

                std::getline(vertex_ss, index_str, '/');
                if (!index_str.empty()) {
                    vertex_index = std::stoi(index_str) - 1;
                }

                if (std::getline(vertex_ss, index_str, '/')) {
                    if (!index_str.empty()) {
                        texcoord_index = std::stoi(index_str) - 1;
                    }
                    if (std::getline(vertex_ss, index_str)) {
                        if (!index_str.empty()) {
                            normal_index = std::stoi(index_str) - 1;
                        }
                    }
                }

                poly.vertex_indices.push_back(vertex_index);
                if (texcoord_index != -1) {
                    poly.texture_indices.push_back(texcoord_index);
                }
                if (normal_index != -1) {
                    poly.normal_indices.push_back(normal_index);
                }
            }
            mesh.polygons.push_back(poly);
        }
    }

    // Устанавливаем вершины
    mesh.vertices = temp_vertices;

    // Создание списка индексов для отрисовки линий
    mesh.indices.clear();
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
        }
    }

    file.close();
    return mesh;
}
