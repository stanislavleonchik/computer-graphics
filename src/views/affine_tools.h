#include "../models/AffineMatrix.h"
#include "../models/Polygon.h"
#include "../models/Tool.h"

using std::vector, std::string;

bool show_offset_window = false;
bool show_turning_window = false;
bool show_scaling_window = false;
bool is_setpoint = false;

int offset_x = 0;
int offset_y = 0;
int turning_value = 0;
float scaling_value_x = 0;
float scaling_value_y = 0;

string scaling_opt = "Scaling Options";
string turning_opt = "Turning Options";

AffineMatrix amatrix;
ImVec2 center_point;

void popup(const string& opt_name, Tool& current_tool) {
    if (ImGui::BeginPopup(opt_name.c_str())) { // �������� �������� ����������� ������
        if (ImGui::Selectable("Center")) {
            if (!show_offset_window) {
                amatrix.around_center = true;
                if (opt_name == turning_opt)
                    show_turning_window = true;
                else
                    show_scaling_window = true;
            }
        }
        if (ImGui::Selectable("Set a point")) {

            if (!show_offset_window) {
                current_tool = Tool::affine;
                amatrix.around_center = false;
                if (opt_name == turning_opt)
                    show_turning_window = true;
                else
                    show_scaling_window = true;

            }
        }
        ImGui::EndPopup();
    }

}

void offset_window(vector<Polygon>& polygons) {
    ImGui::Begin("Offset Settings", &show_offset_window);
    ImGui::InputInt("Offset X", &offset_x);
    ImGui::InputInt("Offset Y", &offset_y);

    if (ImGui::Button("Apply")) {
        amatrix.make_offset(offset_x, offset_y, polygons);
        show_offset_window = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        show_offset_window = false;
    }

    ImGui::End();
}

void turning_window(vector<Polygon>& polygons) {
    ImGui::Begin("Turning Settings", &show_turning_window);

    ImGui::InputInt("The angle in degree", &turning_value);


    if (ImGui::Button("Apply")) {
        if (is_setpoint || amatrix.around_center) {
            amatrix.make_turning(turning_value, polygons);
            show_turning_window = false;
            is_setpoint = false;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        show_turning_window = false;
    }

    ImGui::End();
}


void scaling_window(vector<Polygon>& polygons) {
    ImGui::Begin("Scaling Settings", &show_scaling_window);
    ImGui::InputFloat("The scaling X", &scaling_value_x);
    ImGui::InputFloat("The scaling Y", &scaling_value_y);

    if (ImGui::Button("Apply")) {
        if (is_setpoint || amatrix.around_center) {
            amatrix.make_scaling(scaling_value_x, scaling_value_y, polygons);
            show_scaling_window = false;
            is_setpoint = false;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        show_scaling_window = false;
    }

    ImGui::End();
}

void create_affine_tools(vector<Polygon>& polygons, Tool& current_tool) {
    ImGui::Begin("Affine tools");

    if (ImGui::Button("offset")) {
        show_offset_window = true;
    }
    if (ImGui::Button("Turning")) {
        ImGui::OpenPopup(turning_opt.c_str());

    }

    if (ImGui::Button("Scaling")) {
        ImGui::OpenPopup(scaling_opt.c_str());
    }

    popup(scaling_opt, current_tool);
    popup(turning_opt, current_tool);

    if (ImGui::IsItemClicked()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        amatrix.set_center_point(mousePos);
    }

    ImGui::End();


    if (show_offset_window) {
        offset_window(polygons);
    }

    if (show_turning_window) {
        turning_window(polygons);
    }

    if (show_scaling_window)
        scaling_window(polygons);
}
