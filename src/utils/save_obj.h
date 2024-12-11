#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "../models/Mesh.h"

void saveOBJ(const Mesh& mesh, const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: " << path << std::endl;
        std::cerr << "Ошибка: " << strerror(errno) << " (код " << errno << ")" << std::endl;
        return;
    }

    // Записываем вершины
    for (const auto& vertex : mesh.vertices) {
        file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    // Записываем грани
    for (const auto& poly : mesh.polygons) {
        file << "f";
        for (const auto& idx : poly.vertex_indices) {
            // В OBJ индексация начинается с 1
            file << " " << (idx + 1);
        }
        file << "\n";
    }

    file.close();
}
