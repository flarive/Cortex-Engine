#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec4 FragPosLightSpace;

void main()
{
    vec4 totalPosition = vec4(aPos, 1.0);
    vec3 totalNormal   = aNormal;

    vec3 worldPos = vec3(model * totalPosition);
    FragPos = worldPos;

    vec3 worldNormal = normalMatrix * totalNormal;

    Normal = normalize(worldNormal);
    TexCoords = aTexCoords;

    // If tangents are object-space in the mesh, transform them to world space
    Tangent   = normalize(mat3(model) * aTangents);
    Bitangent = normalize(mat3(model) * aBitangents);

    FragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);

    // pass OBJECT space to tessellation
    gl_Position = vec4(aPos, 1.0);
    //gl_Position = projection * view * model * totalPosition;
}