#pragma once
#include <vector>

struct Polygon3 {
    std::vector<int> vertex_indices;
    std::vector<int> texture_indices;
    std::vector<int> normal_indices;
};
