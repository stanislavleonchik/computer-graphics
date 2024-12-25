#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Убираем трансляцию из view-матрицы, чтобы «куб» оставался вокруг камеры
    mat4 rotView = mat4(mat3(view));
    vec4 pos = projection * rotView * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // z = w &rarr; depth = 1.0 (на far-plane)
    TexCoords = aPos;
}