#version 330 core
        in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 VertexColor;
in vec4 FragPosLightSpace;
in vec3 ReflectDir;

out vec4 FragColor;

// стандартные uniform'ы
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture1;

uniform bool isLight;
uniform vec3 emissiveColor;

uniform bool useVertexColor;
uniform bool hasTexture;
uniform vec3 objectColor;

// тени
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

// отражения
uniform bool enableReflection;
uniform samplerCube cubeMap;

// для отражений по граням (только для room)
uniform bool faceReflectionFlags[6]; // +X, -X, +Y, -Y, +Z, -Z

// Простая функция расчёта теней (PCF)
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // за границами shadowMap
    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||
    projCoords.y < 0.0 || projCoords.y > 1.0 ||
    projCoords.z < 0.0 || projCoords.z > 1.0)
    return 0.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
            if(projCoords.z - bias > closestDepth)
            shadow += 1.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main() {
    // если это «источник света», рисуем просто эмиссивный цвет
    if (isLight) {
        FragColor = vec4(emissiveColor, 1.0);
        return;
    }

    // ambient
    vec3 ambient = 0.2 * vec3(1.0);

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // specular
    float specularStrength = 2.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDirSpec = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDirSpec), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    // затухание
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.0001 * (distance * distance));
    diffuse *= attenuation;
    specular *= attenuation;

    // тень
    float shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);

    // цвет текстуры
    vec3 textureColor = hasTexture ? texture(texture1, TexCoord).rgb : vec3(1.0);

    // итоговый цвет без отражения
    vec3 finalColor = objectColor;
    if(useVertexColor) {
        finalColor *= VertexColor;
    }
    finalColor *= textureColor;

    // результирующее освещение без учёта отражений
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * finalColor;

    // добавим отражение
    if(enableReflection) {
        // Проверим, есть ли какие-то флаги (для комнаты)
        bool anyFlag = false;
        for(int i = 0; i < 6; ++i) {
            if(faceReflectionFlags[i]) {
                anyFlag = true;
                break;
            }
        }

        vec3 reflection = vec3(0.0);
        reflection = texture(cubeMap, ReflectDir).rgb;
        // Смешиваем отражение и основной цвет
        // (число 0.3 означает «насколько сильно» подмешиваем отражение)
        lighting = mix(lighting, reflection, 0.3);
    }

    FragColor = vec4(lighting, 1.0);
}