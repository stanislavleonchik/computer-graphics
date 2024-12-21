#ifndef LIGHT_TOOLS_H
#define LIGHT_TOOLS_H

#include"Mesh.h"
#include<vector>
#include"Matrix4x4.h"

struct Light {
    static Point3 position;
    static Point3 light_color;
    static Point3 object_color;
    static float intensive, ambient, specular;

    Light() {
    }
};

void create_light_tools(bool& is_shown, Mesh& mesh, bool& light);

std::vector<Point3> light_eval(Mesh& mesh, Point3 camera_pos, const Matrix4x4 model);

std::vector<Point3> compute_vertex_normals(const Mesh& mesh);

#endif // !LIGHT_TOOLS_H
