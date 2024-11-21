#pragma once

struct Point3 {
    float x, y, z;
    float w = 1.0f; // По умолчанию w = 1.0, используется для перспективных преобразований
};
