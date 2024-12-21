#include"light_tools.h"
#include"imgui.h"
#include<algorithm>
#include<math.h>
using std::vector;

vector<Point3> compute_vertex_normals(const Mesh& mesh);
Point3 eval_toon_color(const Point3& normal, const Point3& lightDir, const Point3& viewDir);


Point3 Light::position = Point3(-3, 2, 0);
Point3 Light::light_color = Point3(1, 1, 1); 
Point3 Light::object_color = Point3(0, 0, 1);
float Light::intensive = 1; 
float Light::ambient = 0.8;
float Light::specular = 0.5;

void create_light_tools(bool& is_shown, Mesh& mesh, bool& light) {
	ImGui::Begin("Light tools", &is_shown, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("Light on"))
        light = true;
    ImGui::SameLine();
    if (ImGui::Button("Light off"))
        light = false;

    ImGui::Text("Position"); // Сдвиг
    ImGui::SliderFloat("Shift X", &Light::position.x, -7.0f, 7.0f);
    ImGui::SliderFloat("Shift Y", &Light::position.y, -7.0f, 7.0f);
    ImGui::SliderFloat("Shift Z", &Light::position.z, -7.0f, 7.0f);
    ImGui::Separator();

    ImGui::ColorEdit3("Light color", (float*)&Light::light_color);
    ImGui::Separator();
    ImGui::ColorEdit3("Object color", (float*)&Light::object_color);

	ImGui::End();
}

vector<Point3> light_eval(Mesh& mesh, Point3 camera_pos, const Matrix4x4 model)
{
    vector<Point3> normals = compute_vertex_normals(mesh);

    vector<Point3> vertecies_color;
    for (int i = 0; i < mesh.vertices.size(); i++) {
        Point3 vertex = mesh.vertices[i];
        Point3 lightDir = Light::position - model * vertex;
        Point3 viewDir = camera_pos - vertex;
        vertecies_color.push_back(eval_toon_color(normals[i], lightDir, viewDir));
    }
    return vertecies_color;
}

vector<Point3> compute_vertex_normals(const Mesh& mesh) {
    vector<Point3> normals(mesh.vertices.size());
    vector<int> vertex_triangle_count(mesh.vertices.size());

	for (const auto& polygon : mesh.polygons)
	{
		if (polygon.vertex_indices.size() < 3) continue;

		const Point3& v0 = mesh.vertices[polygon.vertex_indices[0]];
		const Point3& v1 = mesh.vertices[polygon.vertex_indices[1]];
		const Point3& v2 = mesh.vertices[polygon.vertex_indices[2]];

		Point3 normal = ((v1 - v0).cross(v2 - v0)).normalize();

		for (auto i : polygon.vertex_indices)
		{
			normals[i] = normals[i] + normal;
			vertex_triangle_count[i] += 1;
		}
	}

	for (int i = 0; i < mesh.vertices.size(); i++) {
		if (vertex_triangle_count[i] > 0)
			normals[i] = Point3(
				normals[i].x / vertex_triangle_count[i],
				normals[i].y / vertex_triangle_count[i],
				normals[i].z / vertex_triangle_count[i]
			).normalize();
	}
	return normals;
}

Point3 eval_toon_color(const Point3& normal, const Point3& lightDir, const Point3& viewDir) {

    
    // Нормализуем входные векторы
    Point3 N = normal.normalize();
    Point3 L = lightDir.normalize();
    Point3 V = viewDir.normalize();
    Point3 R = -L.reflect(N);

    // Компоненты освещения
    Point3 Ambient = Light::light_color * Light::ambient;

    // Диффузная компонента
    float diff = std::max(N.dot(L), 0.0f);
    // Тонизация диффуза
    if (diff > 0.95f) diff = 1.0f;
    else if (diff > 0.5f) diff = 0.7f;
    else if (diff > 0.25f) diff = 0.4f;
    else diff = 0.1f;
    Point3 Diffuse = Light::light_color * diff;

    // Спекулярная компонента
    float spec = std::pow(std::max(V.dot(R), 0.0f), 32);
    // Тонизация спекуляра
    if (spec > 0.95f) spec = 1.0f;
    else if (spec > 0.5f) spec = 0.7f;
    else if (spec > 0.25f) spec = 0.4f;
    else spec = 0.0f;
    Point3 Specular = Light::light_color * spec * Light::specular;

    // Итоговый цвет
    Point3 summary = (Ambient + Diffuse + Specular) * Light::object_color;
    if (summary.len() < 0)
        return { 0, 0, 0 };
    else if (summary.len() > 1)
        return { 1, 1, 1 };
    else
        return summary;
}