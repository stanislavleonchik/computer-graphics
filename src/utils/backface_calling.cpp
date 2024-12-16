#include"backface_calling.h"
#include <vector>

using std::vector;

bool is_not_set_vector = true;


void backface_culling_axon(Mesh& mesh, const Point3 camera_dir) {

	Point3 vv = { 0, 0, -2 };

	if (mesh.indices.empty() || mesh.vertices.empty())
		return;

	vector<unsigned int> visiuble_inds;

	for (const auto& polygon : mesh.polygons)
	{
		if (polygon.vertex_indices.size() < 3) continue;

		const Point3& v0 = mesh.vertices[polygon.vertex_indices[0]];
		const Point3& v1 = mesh.vertices[polygon.vertex_indices[1]];
		const Point3& v2 = mesh.vertices[polygon.vertex_indices[2]];

		Point3 normal = ((v1 - v0).cross(v2 - v0)).normalize();

		if (normal.dot(camera_dir) < 0) {
			visiuble_inds.insert(visiuble_inds.end(), polygon.vertex_indices.begin(), polygon.vertex_indices.end());
		}
	}

	mesh.indices = visiuble_inds;
}

void backface_culling_pers(Mesh& mesh, const Point3 camera_pos) {
    
	vector<unsigned int> visible_indices;

    for (const auto& polygon : mesh.polygons) {
        if (polygon.vertex_indices.size() < 3) continue;

        const Point3& v0 = mesh.vertices[polygon.vertex_indices[0]];
        const Point3& v1 = mesh.vertices[polygon.vertex_indices[1]];
        const Point3& v2 = mesh.vertices[polygon.vertex_indices[2]];

        Point3 edge1 = v1 - v0;
        Point3 edge2 = v2 - v0;
        Point3 normal = edge1.cross(edge2).normalize();

        Point3 to_camera = (camera_pos - v0).normalize();

        if (normal.dot(to_camera) < 0) {
            visible_indices.insert(visible_indices.end(), polygon.vertex_indices.begin(), polygon.vertex_indices.end());
        }
    }

    mesh.indices = visible_indices;
}
