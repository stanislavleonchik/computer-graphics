#ifndef MATRIX4X4_H
#define MATRIX4X4_H

struct Matrix4x4 { // Матрица 4x4 для трансформаций
    float m[4][4]{};

    Matrix4x4() {
        for (int i = 0; i < 4; ++i) // Инициализация единичной матрицей
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    Matrix4x4 operator*(const Matrix4x4& other) const { // Умножение матриц
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k)
                    result.m[i][j] += m[i][k] * other.m[k][j];
            }
        return result;
    }

    Point3 operator*(const Point3& v) const { // Умножение матрицы на вектор
        float x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
        float y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
        float z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
        float w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3];

        if (w != 0.0f) {
            x /= w;
            y /= w;
            z /= w;
        }

        return { x, y, z };
    }

    static Matrix4x4 perspective(float fov, float aspect, float near1, float far1) { // Создание матрицы перспективной проекции
        Matrix4x4 result;
        float tanHalfFov = std::tan(fov / 2);

        result.m[0][0] = 1 / (aspect * tanHalfFov);
        result.m[1][1] = 1 / tanHalfFov;
        result.m[2][2] = -(far1 + near1) / (far1 - near1);
        result.m[2][3] = -1;
        result.m[3][2] = -(2 * far1 * near1) / (far1 - near1);
        result.m[3][3] = 0;

        return result;
    }

    static Matrix4x4 lookAt(const Point3& eye, const Point3& center, const Point3& up) { // Создание матрицы просмотра (камера)
        Point3 f = (center - eye).normalize();
        Point3 s = f.cross(up).normalize();
        Point3 u = s.cross(f);

        Matrix4x4 result;
        result.m[0][0] = s.x;
        result.m[0][1] = s.y;
        result.m[0][2] = s.z;
        result.m[0][3] = -s.dot(eye);

        result.m[1][0] = u.x;
        result.m[1][1] = u.y;
        result.m[1][2] = u.z;
        result.m[1][3] = -u.dot(eye);

        result.m[2][0] = -f.x;
        result.m[2][1] = -f.y;
        result.m[2][2] = -f.z;
        result.m[2][3] = f.dot(eye);

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    static Matrix4x4 translate(const Point3& t) {
        Matrix4x4 result;
        result.m[3][0] = t.x;
        result.m[3][1] = t.y;
        result.m[3][2] = t.z;

        return result;
    }

    static Matrix4x4 scale(const Point3& s) {

        Matrix4x4 result;
        result.m[0][0] = s.x;
        result.m[1][1] = s.y;
        result.m[2][2] = s.z;

        return result;
    }

    static Matrix4x4 rotation(float angle, const Point3& axis) {
        Matrix4x4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        float t = 1 - c;

        Point3 a = axis.normalize();

        result.m[0][0] = c + a.x * a.x * t;
        result.m[0][1] = a.x * a.y * t + a.z * s;
        result.m[0][2] = a.x * a.z * t - a.y * s;
        result.m[0][3] = 0.0f;

        result.m[1][0] = a.y * a.x * t - a.z * s;
        result.m[1][1] = c + a.y * a.y * t;
        result.m[1][2] = a.y * a.z * t + a.x * s;
        result.m[1][3] = 0.0f;

        result.m[2][0] = a.z * a.x * t + a.y * s;
        result.m[2][1] = a.z * a.y * t - a.x * s;
        result.m[2][2] = c + a.z * a.z * t;
        result.m[2][3] = 0.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    static Matrix4x4 rotation(float c, float s, const Point3& axis) {
        Matrix4x4 result;
        float t = 1 - c;

        Point3 a = axis.normalize();

        result.m[0][0] = c + a.x * a.x * t;
        result.m[0][1] = a.x * a.y * t + a.z * s;
        result.m[0][2] = a.x * a.z * t - a.y * s;
        result.m[0][3] = 0.0f;

        result.m[1][0] = a.y * a.x * t - a.z * s;
        result.m[1][1] = c + a.y * a.y * t;
        result.m[1][2] = a.y * a.z * t + a.x * s;
        result.m[1][3] = 0.0f;

        result.m[2][0] = a.z * a.x * t + a.y * s;
        result.m[2][1] = a.z * a.y * t - a.x * s;
        result.m[2][2] = c + a.z * a.z * t;
        result.m[2][3] = 0.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    Matrix4x4 transpose() {
        Matrix4x4 result;

        for (size_t i = 0; i < 4; i++) {
            for (size_t j = 0; j < 4; j++) {
                result.m[i][j] = m[j][i];
            }
        }

        return result;
    }
};


#endif // !MATRIX4X4_H
