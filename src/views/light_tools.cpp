#include"light_tools.h"
#include"imgui.h"

static Point3 position = { -3, 2, 0 };
static Point3 color = Point3(1, 1, 1);
static float intensive = 1;
static bool light;

void create_light_tools(bool& is_shown, Mesh& mesh) {
	ImGui::Begin("Light tools", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("Light on"))
        light = true;
    ImGui::SameLine();
    if (ImGui::Button("Light off"))
        light = false;

    ImGui::Text("Position"); // Сдвиг
    ImGui::SliderFloat("Shift X", &position.x, -7.0f, 7.0f);
    ImGui::SliderFloat("Shift Y", &position.y, -7.0f, 7.0f);
    ImGui::SliderFloat("Shift Z", &position.z, -7.0f, 7.0f);
    ImGui::Separator();

    ImGui::ColorEdit3("clear color", (float*)&color);

    ImGui::SliderFloat("Intensive", &intensive, 0.0, 3.0);

	ImGui::End();
}