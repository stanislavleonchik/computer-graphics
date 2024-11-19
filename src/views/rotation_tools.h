#ifndef ROTATION_TOOLS_H
#define ROTATION_TOOLS_H

#include "imgui.h"
#include "Mesh.h"

size_t axe = 0;
float rotation_value = 0.0;
bool show_custom_vec_window = false;
Point3 x, y;

void show_create_custom_vec() {
	ImGui::Begin("Custom Vector");

	ImGui::Text("First point");
	ImGui::InputFloat("x", &x.x);
	ImGui::SameLine();
	ImGui::InputFloat("y", &x.y);
	ImGui::SameLine();
	ImGui::InputFloat("z", &x.z);
	
	ImGui::Text("Second point");
	ImGui::InputFloat("x", &y.x);
	ImGui::SameLine();
	ImGui::InputFloat("y", &y.y);
	ImGui::SameLine();
	ImGui::InputFloat("z", &y.z);

	if (ImGui::Button("Ok"))
		show_custom_vec_window = false;

	ImGui::End();
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

void make_axe_rotation(Point3 center, Mesh& mesh)
{
	Point3 A; Point3 vec;
	if (axe == 0) {
		A = center;
		vec = { 1, 0, 0 };
	}
	else if (axe == 1) {
		A = center;
		vec = { 0, 1, 0 };
	}
	else if (axe == 2) {
		A = center;
		vec = { 0, 0, 1 };
	}
	else if (axe == 3) {
		A = x;
		vec = x - y;
		vec.normalize();
	}
	else
		return;


}


void draw_axes(Point3 center, float axisLength = 1.0f)
{
}

void show_rotation_tools(Mesh& mesh) {

	ImGui::Begin("Rotation tools");

	auto center = CalculateMeshCenter(mesh);
	draw_axes(center);

	ImGui::Text("Choose an axe");
	if (ImGui::RadioButton("x", axe == 0)) axe = 0;
	ImGui::SameLine();
	if (ImGui::RadioButton("y", axe == 1)) axe = 1;
	ImGui::SameLine();
	if (ImGui::RadioButton("z", axe == 2)) axe = 2;
	if (ImGui::RadioButton("custom vector", axe == 3)) {
		show_custom_vec_window = true;
		axe = 3;
	}

	ImGui::Text("Rotation value: ");
	ImGui::SliderFloat("Roatation value", &rotation_value, 0.0, 360.0);
	
	if (show_custom_vec_window)
		show_create_custom_vec();

	if(ImGui::Button("Ok"))
		make_axe_rotation(center, mesh);

	ImGui::End();
}

#endif // !ROTATION_TOOLS
