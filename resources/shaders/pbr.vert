#version 330 core

layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal; // the normal variable has attribute position 1
layout (location = 2) in vec2 aTexCoords; // the uv variable has attribute position 2
layout (location = 3) in vec3 aTangents; // the tangent variable has attribute position 3
layout (location = 4) in vec3 aBitangents; // the bitangent variable has attribute position 4

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;

    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = normalMatrix * aNormal;
    vs_out.Tangent = aTangents;
    vs_out.Bitangent = aBitangents;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);


    vec3 T = normalize(mat3(model) * aTangents);
    vec3 B = normalize(mat3(model) * aBitangents);
    vec3 N = normalize(mat3(model) * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}