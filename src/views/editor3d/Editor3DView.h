// Editor3DView.h
#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "../../models/3D/Mesh.h"

// Define matrix and vector types
typedef float Mat4[4][4];
typedef float Vec3[3];

class Editor3D {
public:
    float translation[3] = {0.0f, 0.0f, 0.0f};
    float rotation[3] = {0.0f, 0.0f, 0.0f}; // In degrees
    float scale[3] = {1.0f, 1.0f, 1.0f};
    Mesh current_mesh;
    int selected_polyhedron = 0;

    Editor3D(GLFWwindow* window) : window(window) {
        initialize();
        current_mesh = create_tetrahedron();
        setup_mesh_data();
    }

    void add_editor_window() {
        ImGui::Begin("3D Editor");

        // Controls
        ImGui::Text("Select Polyhedron:");
        const char* polyhedra[] = { "Tetrahedron", "Hexahedron", "Octahedron", "Icosahedron", "Dodecahedron" };
        if (ImGui::Combo("Polyhedron", &selected_polyhedron, polyhedra, IM_ARRAYSIZE(polyhedra))) {
            switch (selected_polyhedron) {
                case 0: current_mesh = create_tetrahedron(); break;
                case 1: current_mesh = create_hexahedron(); break;
                case 2: current_mesh = create_octahedron(); break;
                case 3: current_mesh = create_icosahedron(); break;
                case 4: current_mesh = create_dodecahedron(); break;
            }
            setup_mesh_data();
        }

        // Transformation Controls
        ImGui::Separator();
        ImGui::Text("Affine Transformations");
        ImGui::DragFloat3("Translation", translation, 0.01f);
        ImGui::DragFloat3("Rotation (Degrees)", rotation, 0.5f);
        ImGui::DragFloat3("Scale", scale, 0.01f, 0.1f, 10.0f);

        // Rendering
        ImGui::Separator();
        ImGui::Text("3D Viewport:");
        ImVec2 window_size = ImGui::GetContentRegionAvail();
        framebuffer_width = static_cast<int>(window_size.x);
        framebuffer_height = static_cast<int>(window_size.y);
        render_scene(framebuffer_width, framebuffer_height);
        ImGui::Image((void*)(intptr_t)framebuffer_texture, window_size);

        ImGui::End();
    }

private:
    GLFWwindow* window;
    GLuint vao = 0, vbo = 0;
    GLuint framebuffer = 0, framebuffer_texture = 0;
    GLuint rbo = 0;
    GLuint shader_program = 0;

    void initialize() {
        // Initialize OpenGL settings
        glEnable(GL_DEPTH_TEST);
        // Compile shader program
        shader_program = create_shader_program(vertex_shader_source, fragment_shader_source);
        // Generate framebuffer and associated objects
        glGenFramebuffers(1, &framebuffer);
        glGenTextures(1, &framebuffer_texture);
        glGenRenderbuffers(1, &rbo);
    }

    GLuint create_shader_program(const char* vertex_source, const char* fragment_source) {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertex_source, NULL);
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, "VERTEX");

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragment_source, NULL);
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, "FRAGMENT");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << error << std::endl;
        }

        return program;
    }

    void checkCompileErrors(GLuint shader, const std::string& type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
            }
        }
    }

    void setup_mesh_data() {
        if (vao != 0) {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
        }

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        std::vector<float> vertices;
        for (const auto& polygon : current_mesh.polygons) {
            for (size_t i = 1; i + 1 < polygon.vertex_indices.size(); ++i) {
                int idx0 = polygon.vertex_indices[0];
                int idx1 = polygon.vertex_indices[i];
                int idx2 = polygon.vertex_indices[i + 1];

                Point3 v0 = current_mesh.vertices[idx0];
                Point3 v1 = current_mesh.vertices[idx1];
                Point3 v2 = current_mesh.vertices[idx2];

                vertices.insert(vertices.end(), {v0.x, v0.y, v0.z});
                vertices.insert(vertices.end(), {v1.x, v1.y, v1.z});
                vertices.insert(vertices.end(), {v2.x, v2.y, v2.z});
            }
        }
        std::cout << "Количество вершин: " << vertex_count << std::endl;
        if (!vertices.empty()) {
            std::cout << "Первая вершина: (" << vertices[0] << ", " << vertices[1] << ", " << vertices[2] << ")" << std::endl;
        }

        // Создание и заполнение VBO для вершин
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        GLuint vbo_colors = 0;
        glGenBuffers(1, &vbo_colors);

        vertex_count = static_cast<GLsizei>(vertices.size() / 3);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void render_scene(int fb_width, int fb_height) {
        if (fb_width <= 0 || fb_height <= 0) {
            return;
        }
        resize_framebuffer(fb_width, fb_height);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, fb_width, fb_height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the mesh
        draw_mesh();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void resize_framebuffer(int width, int height) {
        // Resize framebuffer and associated objects
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // Resize texture
        glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Resize renderbuffer
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        // Attach buffers
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void draw_mesh() {
        glUseProgram(shader_program);

        // Настройка матриц проекции и вида
        float aspect_ratio = static_cast<float>(framebuffer_width) / static_cast<float>(framebuffer_height);
        Mat4 projection;
        mat4_perspective(projection, 45.0f * (M_PI / 180.0f), aspect_ratio, 0.1f, 100.0f);

        Vec3 eye = {0.0f, 0.0f, 5.0f};
        Vec3 center = {0.0f, 0.0f, 0.0f};
        Vec3 up = {0.0f, 1.0f, 0.0f};
        Mat4 view;
        mat4_look_at(view, eye, center, up);

        // Вычисление матрицы модели
        Mat4 model;
        mat4_identity(model);

        // Применение масштабирования
        Mat4 scale_mat;
        mat4_scale(scale_mat, scale[0], scale[1], scale[2]);
        mat4_multiply(model, model, scale_mat);

        // Применение вращения
        Mat4 rot_x_mat, rot_y_mat, rot_z_mat;
        mat4_rotate_x(rot_x_mat, rotation[0] * (M_PI / 180.0f));
        mat4_rotate_y(rot_y_mat, rotation[1] * (M_PI / 180.0f));
        mat4_rotate_z(rot_z_mat, rotation[2] * (M_PI / 180.0f));

        mat4_multiply(model, model, rot_x_mat);
        mat4_multiply(model, model, rot_y_mat);
        mat4_multiply(model, model, rot_z_mat);

        // Применение переноса
        Mat4 translation_mat;
        mat4_translate(translation_mat, translation[0], translation[1], translation[2]);
        mat4_multiply(model, model, translation_mat);

        // Передача матриц в шейдер
        GLint model_loc = glGetUniformLocation(shader_program, "model");
        GLint view_loc  = glGetUniformLocation(shader_program, "view");
        GLint proj_loc  = glGetUniformLocation(shader_program, "projection");

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);

        // Отрисовка
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        glBindVertexArray(0);
    }

    GLsizei vertex_count = 0;
    int framebuffer_width = 800;
    int framebuffer_height = 600;

    // Shader sources
    const char* vertex_shader_source = R"(
        // Вершинный шейдер
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // Новый атрибут цвета

out vec3 fragColor; // Передаем цвет во фрагментный шейдер

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragColor = aColor; // Присваиваем цвет
}
    )";

    const char* fragment_shader_source = R"(
        // Фрагментный шейдер
#version 410 core
in vec3 fragColor; // Принимаем цвет из вершинного шейдера

out vec4 FragColor;

void main() {
    FragColor = vec4(fragColor, 1.0); // Используем цвет
}
    )";

    // Custom matrix and vector functions
    void mat4_identity(Mat4 mat) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                mat[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    void mat4_translate(Mat4 mat, float x, float y, float z) {
        mat4_identity(mat);
        mat[0][3] = x;
        mat[1][3] = y;
        mat[2][3] = z;
    }

    void mat4_scale(Mat4 mat, float sx, float sy, float sz) {
        mat4_identity(mat);
        mat[0][0] = sx;
        mat[1][1] = sy;
        mat[2][2] = sz;
    }

    void mat4_rotate_x(Mat4 mat, float angle_rad) {
        mat4_identity(mat);
        float c = cosf(angle_rad);
        float s = sinf(angle_rad);
        mat[1][1] = c;
        mat[1][2] = -s;
        mat[2][1] = s;
        mat[2][2] = c;
    }

    void mat4_rotate_y(Mat4 mat, float angle_rad) {
        mat4_identity(mat);
        float c = cosf(angle_rad);
        float s = sinf(angle_rad);
        mat[0][0] = c;
        mat[0][2] = s;
        mat[2][0] = -s;
        mat[2][2] = c;
    }

    void mat4_rotate_z(Mat4 mat, float angle_rad) {
        mat4_identity(mat);
        float c = cosf(angle_rad);
        float s = sinf(angle_rad);
        mat[0][0] = c;
        mat[0][1] = -s;
        mat[1][0] = s;
        mat[1][1] = c;
    }

    void mat4_perspective(Mat4 mat, float fov_rad, float aspect, float z_near, float z_far) {
        float f = 1.0f / tanf(fov_rad / 2.0f);
        mat4_identity(mat);
        mat[0][0] = f / aspect;
        mat[1][1] = f;
        mat[2][2] = (z_far + z_near) / (z_near - z_far);
        mat[2][3] = (2.0f * z_far * z_near) / (z_near - z_far);
        mat[3][2] = -1.0f;
        mat[3][3] = 0.0f;
    }

    void mat4_look_at(Mat4 mat, const Vec3 eye, const Vec3 center, const Vec3 up) {
        Vec3 f, s, u;
        // Compute forward vector f = normalize(center - eye)
        for (int i = 0; i < 3; ++i)
            f[i] = center[i] - eye[i];
        float f_mag = sqrtf(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
        for (int i = 0; i < 3; ++i)
            f[i] /= f_mag;
        // Compute s = normalize(cross(f, up))
        s[0] = f[1]*up[2] - f[2]*up[1];
        s[1] = f[2]*up[0] - f[0]*up[2];
        s[2] = f[0]*up[1] - f[1]*up[0];
        float s_mag = sqrtf(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
        for (int i = 0; i < 3; ++i)
            s[i] /= s_mag;
        // Compute u = cross(s, f)
        u[0] = s[1]*f[2] - s[2]*f[1];
        u[1] = s[2]*f[0] - s[0]*f[2];
        u[2] = s[0]*f[1] - s[1]*f[0];
        // Fill the matrix
        mat4_identity(mat);
        mat[0][0] = s[0];
        mat[0][1] = s[1];
        mat[0][2] = s[2];
        mat[1][0] = u[0];
        mat[1][1] = u[1];
        mat[1][2] = u[2];
        mat[2][0] = -f[0];
        mat[2][1] = -f[1];
        mat[2][2] = -f[2];
        mat[0][3] = - (s[0]*eye[0] + s[1]*eye[1] + s[2]*eye[2]);
        mat[1][3] = - (u[0]*eye[0] + u[1]*eye[1] + u[2]*eye[2]);
        mat[2][3] = f[0]*eye[0] + f[1]*eye[1] + f[2]*eye[2];
        mat[3][3] = 1.0f;
    }

    void mat4_multiply(Mat4 result, Mat4 a, Mat4 b) {
        Mat4 temp;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                temp[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    temp[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        // Copy the result back to result matrix
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result[i][j] = temp[i][j];
    }
};
