#ifndef VECTOR_VIEW_H
#define VECTOR_VIEW_H

#include"imgui.h"
#include"Mesh.h"

Point3 view_vector = { 0, 0, -1 };

void set_vector_view() {

    static Point3 local_view_vector;

	ImGui::Begin("Set vector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushItemWidth(50);
    ImGui::InputFloat("x", &local_view_vector.x);
    ImGui::SameLine();
    ImGui::InputFloat("y", &local_view_vector.y);
    ImGui::SameLine();
    ImGui::InputFloat("z", &local_view_vector.z);
    ImGui::PopItemWidth();

    if (ImGui::Button("Accept"))
        view_vector = local_view_vector;
    
	ImGui::End();
}

#endif // !VECTOR_VIEW_H
