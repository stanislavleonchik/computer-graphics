#include"backface_calling.h"
#include"view_vector.h"

bool is_not_set_vector = true;

void backface_calling(Mesh& mesh) {

	set_vector_view();

	if (mesh.indices.empty() || mesh.vertices.empty())
		return;

	Mesh m;
	m.vertices = mesh.vertices;

	for (size_t i = 1; i < mesh.indices.size()-1; i++)
	{
		Point3 v1 = mesh.vertices[mesh.indices[i - 1]],
			   v2 = mesh.vertices[mesh.indices[i	]],
			   v3 = mesh.vertices[mesh.indices[i + 1]];
		Point3 normal = (v2 - v1).cross(v3 - v1);

		if (normal.dot(view_vector) > 0.0000001) {
			m.indices.push_back(mesh.indices[i - 1]);
			m.indices.push_back(mesh.indices[i]);
			m.indices.push_back(mesh.indices[i + 1]);
		}
	}

	//докручиваем последние
	unsigned i = mesh.indices.size() - 1;
	Point3 v1 = mesh.vertices[mesh.indices[i - 1]],
		v2 = mesh.vertices[mesh.indices[i]],
		v3 = mesh.vertices[mesh.indices[0]];
	Point3 normal = (v2 - v1).cross(v3 - v1);

	if (normal.dot(view_vector) > 0.0000001) {
		m.indices.push_back(mesh.indices[i - 1]);
		m.indices.push_back(mesh.indices[i]);
		m.indices.push_back(mesh.indices[0]);
	}

	v1 = mesh.vertices[mesh.indices[i]];
	v2 = mesh.vertices[mesh.indices[0]];
	v3 = mesh.vertices[mesh.indices[1]];
	normal = (v2 - v1).cross(v3 - v1);

	if (normal.dot(view_vector) > 0.0000001) {
		m.indices.push_back(mesh.indices[i]);
		m.indices.push_back(mesh.indices[0]);
		m.indices.push_back(mesh.indices[1]);
	}

	mesh = m;
}
