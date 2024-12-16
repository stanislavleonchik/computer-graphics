#include "rotation_figure_creator.h"
#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include<vector>

#define _USE_MATH_DEFINES
#include<math.h>

using std::vector; using std::array;

vector<Point3> points; 
bool is_not_all_points = false;

void rf_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
   
    if (ImGui::GetIO().WantCaptureMouse) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        points.push_back(Point3(xpos, ypos, 0.0f));
    }	    
}

void draw_rotation_body() {
    if (points.empty())
        return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    // ���������� �������
    for (size_t i = 0; i < points.size() - 1; i++) {
        drawList->AddLine(
            ImVec2(points[i].x, points[i].y),
            ImVec2(points[i + 1].x, points[i + 1].y),
            IM_COL32(255, 255, 255, 255),
            2.0f // ������� �����
        );
    }

    drawList->AddLine(
        ImVec2(points[points.size() - 1].x, points[points.size() - 1].y),
        ImVec2(points[0].x, points[0].y),
        IM_COL32(255, 255, 255, 255),
        2.0f // ������� �����
    );

    for (const auto& point : points) {
        drawList->AddCircleFilled(
            ImVec2(point.x, point.y),
            10.0f, // ������ �����
            IM_COL32(66, 170, 255, 255)
        );
    }
}

void draw_axes(GLFWwindow* window) {

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    drawList->AddLine(ImVec2(0, height / 2), ImVec2(width, height / 2), IM_COL32(255, 0, 0, 255));
    drawList->AddLine(ImVec2(width/2, 0), ImVec2(width/2, height), IM_COL32(0, 255, 0, 255));

}

void points_await(bool& is_not_all_points, GLFWwindow* window) {
    
	ImGui::Begin("Points await");
	ImGui::Text("Please set the points");

	if (ImGui::Button("Cencel")) {
		is_not_all_points = false;
		points.clear();
        glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
	}
    ImGui::SameLine();
    if (ImGui::Button("Ok")) {
        is_not_all_points = false;
        glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
    }

	ImGui::End();

    draw_axes(window);
}

Mesh create_figure(unsigned axis, int np, int win_width, int win_height) {

    Mesh mesh;
    if (np < 1 || points.empty()) {
        return mesh;
    }
    float angle_step = 2 * M_PI / np;

    mesh.polygons.resize(np);
    // ��������� ������
    for (int i = 0; i < np; ++i) {
        float angle = i * angle_step;
        float cos_angle = std::cos(angle);
        float sin_angle = std::sin(angle);
        
        for (const auto& p : points) {
            Point3 rotated;
            // ����������� � ��������������� ����������
            float xNormalized = (2.0f * static_cast<float>(p.x) / win_width) - 1.0f;
            float yNormalized =  1.0f - (2.0f * static_cast<float>(p.y) / win_height);
            if (axis == 2) {
                rotated = {
                    xNormalized* cos_angle - yNormalized * sin_angle,
                    xNormalized* sin_angle + yNormalized * cos_angle,
                    p.z
                };
            }
            else if (axis == 1) {
                rotated = {
                    xNormalized* cos_angle + p.z * sin_angle,
                    yNormalized,
                    -xNormalized * sin_angle + p.z * cos_angle
                };
            }
            else if (axis == 0) {
                rotated = {
                    xNormalized,
                    yNormalized* cos_angle - p.z * sin_angle,
                    yNormalized* sin_angle + p.z * cos_angle
                };
            }
            mesh.vertices.push_back(rotated);
            mesh.polygons[i].vertex_indices.push_back(mesh.vertices.size() - 1);
        }

    }

    // ��������� ��������
    mesh.edgeIndices.clear(); // �������� ������ ��������
    for (const auto& poly : mesh.polygons) {
        for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
            int idx0 = poly.vertex_indices[i];
            int idx1 = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];
            mesh.edgeIndices.push_back(idx0);
            mesh.edgeIndices.push_back(idx1);
        }
    }
    points.clear();
    return mesh;
}


// ������� ����� ������ ���
Point3 rotatePoint(const Point3& point, const Point3& axis, float angle) {
    Point3 normalizedAxis = axis.normalize();
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    return point * cosAngle +
        normalizedAxis.cross(point) * sinAngle +
        normalizedAxis * (normalizedAxis.dot(point) * (1 - cosAngle));
}

// ���������� ������ ��������
Mesh generateRevolvedMesh(const Point3& axis, unsigned int np) {
    Mesh mesh;

    // ��� ����
    float angle_step = 2 * M_PI / np;

    // ��������� ������
    for (unsigned int i = 0; i <= np; ++i) {
        float cur_angle = angle_step * i;
        for (const auto& point : points) {
            mesh.vertices.push_back(rotatePoint(point, axis, cur_angle));
        }
    }

    // ��������� ���������
    for (size_t i = 0; i < np; i++) {
        for (size_t j = 0; j < points.size() - 1; j++) {
            int current = i * points.size() + j;
            int next = (i + 1) * points.size() + j;

            // ��������� ��� ������������ �� ������ ���� �������� ���������
            mesh.polygons.push_back({ { current, next, current + 1 } });
            mesh.polygons.push_back({ { current + 1, next, next + 1 } });
        }
    }

    mesh.init_edges_faces();

    return mesh;

}

void rf_tools(bool& is_shown, GLFWwindow* window, Mesh& mesh)
{
    static Point3 axis = { 0, 1, 0 };
	static int np = 20;
	ImGui::Begin("Rotation figure creator");
	
    if (is_not_all_points) {
        glfwSetMouseButtonCallback(window, rf_mouse_button_callback);
        points_await(is_not_all_points, window);
    }

    draw_rotation_body();

	ImGui::Text("Set the axis of rotation");

    ImGui::PushItemWidth(50);
    ImGui::InputFloat("X", &axis.x);
    ImGui::SameLine();
    ImGui::InputFloat("Y", &axis.y);
    ImGui::SameLine();
    ImGui::InputFloat("Z", &axis.z);
    ImGui::PopItemWidth();

	ImGui::Separator();
	ImGui::InputInt("Number of partitions", &np);

	ImGui::Separator();
	if (ImGui::Button("Set points")) {
        mesh = Mesh();
		points.clear(); 
		is_not_all_points = true;
	}

    if (ImGui::Button("Cancel")) {
        is_shown = false;
        points.clear();
    }
	ImGui::SameLine();
	if (ImGui::Button("Revolve") && !is_not_all_points && !points.empty()) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        for (auto& p : points) {
            p.x = (6.0f * static_cast<float>(p.x) / width) - 3.0f;
            p.y = 2.0f - (4.0f * static_cast<float>(p.y) / height);
        }
        mesh = generateRevolvedMesh(axis, np);
        is_shown = false;
	}

	ImGui::End();
}
