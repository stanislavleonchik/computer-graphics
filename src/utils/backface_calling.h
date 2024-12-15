#ifndef BACKFACE_CALLING_H
#define BACKFACE_CALLING_H

#include"Mesh.h"
void backface_culling_axon(Mesh& mesh, const Point3 camera_dir);

void backface_culling_pers(Mesh& mesh, const Point3 camera_pos);

#endif // !BACKFACE_CALLING_H
