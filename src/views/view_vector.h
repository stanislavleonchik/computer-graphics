#ifndef VECTOR_VIEW_H
#define VECTOR_VIEW_H

#include"imgui.h"
#include"Mesh.h"
#include"backface_calling.h"

void set_vector_view(Mesh& mesh, Point3 cameraPos, Point3 cameraDir, bool& is_shown) {

    static Point3 view_vector;
    static bool projection;

	ImGui::Begin("Set view vector", &is_shown, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushItemWidth(50);
    ImGui::InputFloat("x", &view_vector.x);
    ImGui::SameLine();
    ImGui::InputFloat("y", &view_vector.y);
    ImGui::SameLine();
    ImGui::InputFloat("z", &view_vector.z);
    ImGui::PopItemWidth();

    if(ImGui::Button("Camera position")) {
        view_vector = cameraPos;
    }

    if (ImGui::RadioButton("Perspective", projection == false)) { projection = false; }
    if (ImGui::RadioButton("Axonometric", projection == true)) { projection = true; }

    if (ImGui::Button("Accept")) {
        if (projection)
            backface_culling_axon(mesh, view_vector);
        else
            backface_culling_pers(mesh, view_vector);
        is_shown = false;
    }
    
	ImGui::End();
}

#endif // !VECTOR_VIEW_H
