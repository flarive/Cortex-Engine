#version 440 core

layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal; // the normal variable has attribute position 1
layout (location = 2) in vec2 aTexCoords; // the uv variable has attribute position 2
layout (location = 3) in vec3 aTangents; // the tangent variable has attribute position 3
layout (location = 4) in vec3 aBitangents; // the bitangent variable has attribute position 4
layout (location = 5) in ivec4 aBoneIds;  // Only used for animated models
layout (location = 6) in vec4 aWeights;    // Only used for animated models

out VS_OUT {
    //vec3 FragPos; // vertex position in world space, same as worldPosition
    //vec3 Normal; // same as worldNormal when reverse_normals is false
    //vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;

    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

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
    vs_out.FragPos = vec3(model * totalPosition);

    // Normal handling
    if (reverse_normals)
        //vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0 * totalNormal);
        vs_out.Normal = normalMatrix * (-1.0 * totalNormal);
    else
        //vs_out.Normal = transpose(inverse(mat3(model))) * totalNormal;
        vs_out.Normal = normalMatrix * totalNormal;

    // Pass through other attributes
    vs_out.TexCoords = aTexCoords;
    vs_out.Tangent = aTangents;
    vs_out.Bitangent = aBitangents;

    // Light space position for shadow mapping
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    // TBN matrix for normal mapping
    vec3 T = normalize(mat3(model) * aTangents);
    vec3 B = normalize(mat3(model) * aBitangents);
    vec3 N = normalize(mat3(model) * totalNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    // Tangent space positions
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;

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