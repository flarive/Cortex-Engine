#version 440 core

layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal; // the normal variable has attribute position 1
layout (location = 2) in vec2 aTexCoords; // the uv variable has attribute position 2
layout (location = 3) in vec3 aTangents; // the tangent variable has attribute position 3
layout (location = 4) in vec3 aBitangents; // the bitangent variable has attribute position 4
layout (location = 5) in ivec4 aBoneIds;  // Only used for animated models
layout (location = 6) in vec4 aWeights;    // Only used for animated models


out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec4 FragPosLightSpace;
out vec3 TangentLightPos; // ????
out vec3 TangentViewPos; // ????
out vec3 TangentFragPos; // ????


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

uniform bool reverse_normals;

uniform vec3 lightPos;
uniform vec3 viewPos;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform bool isAnimated; // Flag to determine if the model is animated with bones animation
uniform bool isTessellated;

out float Height;

void main()
{
    vec4 totalPosition;
    vec3 totalNormal;

    // Handle bone animation if the model is animated
    if (isAnimated)
    {
        totalPosition = vec4(0.0);
        totalNormal = vec3(0.0);
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            if(aBoneIds[i] == -1)
                continue;

            if (aBoneIds[i] >= MAX_BONES)
            {
                totalPosition = vec4(aPos, 1.0);
                totalNormal = aNormal;
                break;
            }

            vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPos, 1.0);
            totalPosition += localPosition * aWeights[i];
            totalNormal += mat3(finalBonesMatrices[aBoneIds[i]]) * aNormal * aWeights[i];
        }
    }
    else
    {
        totalPosition = vec4(aPos, 1.0);
        totalNormal = aNormal;
    }

    // World space position
    FragPos = vec3(model * totalPosition);

    // Normal handling
    vec3 worldNormal;
    if (reverse_normals)
    {
        worldNormal = normalMatrix * (-1.0 * totalNormal);
    }
    else
    {
        worldNormal = normalMatrix * totalNormal;
    }

    Normal = worldNormal;
    TexCoords = aTexCoords;
    Tangent = aTangents;
    Bitangent = aBitangents;

    // Light space position for shadow mapping
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0); // possible optim !

    // Handle gl_Position based on tessellation mode
    if (isTessellated)
    {
        // For tessellation, output world space position
        gl_Position = vec4(aPos, 1.0);
    }
    else
    {
        // For standard rendering, output clip space position
        gl_Position = projection * view * model * totalPosition;
    }
}