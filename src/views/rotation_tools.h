#ifndef ROTATION_TOOLS_H
#define ROTATION_TOOLS_H

#include "imgui.h"
#include "Mesh.h"
#include "Matrix4x4.h"
#include<vector>

#define _USE_MATH_DEFINES
#include<math.h>

using std::vector;

size_t axis = 0;
float rotation_value = 0.0;
bool show_custom_vec_window = false;
Point3 x, y;

void show_create_custom_vec() {
	ImGui::Begin("Custom Vector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("First point");

	ImGui::PushItemWidth(50);
	ImGui::InputFloat("x1", &x.x);
	ImGui::SameLine();
	ImGui::InputFloat("y1", &x.y);
	ImGui::SameLine();
	ImGui::InputFloat("z1", &x.z);
	ImGui::PopItemWidth();
	
	ImGui::Text("Second point");

	ImGui::PushItemWidth(50);
	ImGui::InputFloat("x2", &y.x);
	ImGui::SameLine();
	ImGui::InputFloat("y2", &y.y);
	ImGui::SameLine();
	ImGui::InputFloat("z2", &y.z);
	ImGui::PopItemWidth();

	if (ImGui::Button("Ok"))
		show_custom_vec_window = false;

	ImGui::End();
}

void reset(Matrix4x4& transform) {
	transform = Matrix4x4();
}

Point3 CalculateMeshCenter(const Mesh& mesh) {
	Point3 center = { 0.0f, 0.0f, 0.0f };
	for (const auto vertex : mesh.vertices) {
		center.x += vertex.x;
		center.y += vertex.y;
		center.z += vertex.z;
	}
	float numVertices = static_cast<float>(mesh.vertices.size());
	center.x /= numVertices;
	center.y /= numVertices;
	center.z /= numVertices;
	return center;
}

void make_axis_rotation(Point3 center, Matrix4x4& transforms)
{
	Point3 A; Point3 vec; //(l, m, n)
	if (axis == 0) {
		A = { 0.0, 0.0, 0.0 };
		vec = { 1.0, 0.0, 0.0 };
	}
	else if (axis == 1) {
		A = { 0.0, 0.0, 0.0 };
		vec = { 0.0, 1.0, 0.0 };
	}
	else if (axis == 2) {
		A = { 0.0, 0.0, 0.0 };
		vec = { 0.0, 0.0, 1.0 };
	}
	else if (axis == 3) {
		A = x;
		vec = x - y;
		vec = vec.normalize();
	}
	else
		return;

	
	transforms = Matrix4x4::translate(Point3(-A.x, -A.y, -A.z)) * transforms;
	transforms = Matrix4x4::rotation(rotation_value * M_PI / 180.0f, vec) * transforms;
	transforms = Matrix4x4::translate(A) * transforms;

}


void draw_axes(Point3 center, float axisLength = 1.0f)
{
}

void show_rotation_tools(Mesh& mesh, Matrix4x4& model) {

	static Matrix4x4 transforms;

	ImGui::Begin("Rotation tools");

	Point3 center = { model.m[0][3], model.m[1][3], model.m[2][3] };
	draw_axes(center);

	ImGui::Text("Choose an axis");
	if (ImGui::RadioButton("x", axis == 0)) axis = 0;
	ImGui::SameLine();
	if (ImGui::RadioButton("y", axis == 1)) axis = 1;
	ImGui::SameLine();
	if (ImGui::RadioButton("z", axis == 2)) axis = 2;
	if (ImGui::RadioButton("custom vector", axis == 3)) {
		show_custom_vec_window = true;
		axis = 3;
	}

	ImGui::Text("Rotation value: ");
	ImGui::SliderFloat("Roatation value", &rotation_value, 0.0, 360.0);
	
	if (show_custom_vec_window)
		show_create_custom_vec();

	if (ImGui::Button("Ok")) {
		make_axis_rotation(center, transforms);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		transforms = Matrix4x4();

	ImGui::End();

	model = transforms * model;
}

#endif // !ROTATION_TOOLS
