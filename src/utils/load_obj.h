#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "../models/Mesh.h"

Mesh loadOBJ(const std::string& path) {
    Mesh mesh;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << path << std::endl;
        return mesh;
    }

    std::vector<Point3> temp_vertices;
    std::vector<TextureCoord> temp_texcoords;
    std::vector<Point3> temp_normals;
    std::vector<Point3> uniqueVertices;
    std::map<Point3, unsigned int> vertexToIndex;

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
            // Текстурные координаты
            float u, v;
            ss >> u >> v;
            temp_texcoords.emplace_back(u, v);
        } else if (prefix == "vn") {
            // Нормали (не используются в текущей реализации)
            float x, y, z;
            ss >> x >> y >> z;
            temp_normals.emplace_back(x, y, z);
        } else if (prefix == "f") {
            // Грани
            Polygon3 poly;
            std::string vertex_info;
            while (ss >> vertex_info) {
                std::stringstream vertex_ss(vertex_info);
                std::string index_str;
                int vertex_index = -1, texcoord_index = -1, normal_index = -1;

                // Формат: v/t/n
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

                // Создание уникальной вершины
                Point3 vertex;
                vertex = temp_vertices[vertex_index];
                if (texcoord_index != -1 && texcoord_index < temp_texcoords.size()) {
                    vertex.texCoord = temp_texcoords[texcoord_index];
                } else {
                    vertex.texCoord = {0.0f, 0.0f}; // Значение по умолчанию
                }

                // Проверка, существует ли уже такая вершина
                if (vertexToIndex.find(vertex) == vertexToIndex.end()) {
                    uniqueVertices.push_back(vertex);
                    unsigned int newIndex = static_cast<unsigned int>(uniqueVertices.size() - 1);
                    vertexToIndex[vertex] = newIndex;
                    poly.vertex_indices.push_back(newIndex);
                } else {
                    poly.vertex_indices.push_back(vertexToIndex[vertex]);
                }
            }
            mesh.polygons.push_back(poly);
        }
    }

    // Устанавливаем вершины
    mesh.vertices = uniqueVertices;

    // Создание списков индексов граней и ребер
    mesh.init_edges_faces();

    file.close();
    return mesh;
}
