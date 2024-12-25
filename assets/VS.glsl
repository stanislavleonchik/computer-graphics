#version 330 core
        layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aColor;

uniform mat4 uMVP;
uniform mat4 uModel;

// Для расчёта теней:
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 VertexColor;
out vec4 FragPosLightSpace;
out vec3 ReflectDir;

uniform vec3 viewPos; // позиция камеры

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);

    // мировая позиция
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    // нормаль
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    // UV
    TexCoord = aTexCoord;
    // цвет вершин
    VertexColor = aColor;

    // для теней
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    // вычислим направление отражённого вектора (от камеры к фрагменту)
    // I — вектор (FragPos - viewPos), но удобнее взять наоборот (viewPos - FragPos),
    //   смотря как хотим интерпретировать.
    // Тут берём I = normalize(FragPos - viewPos),
    // потом reflect(I, N).
    vec3 I = normalize(FragPos - viewPos);
    ReflectDir = reflect(I, normalize(Normal));
}