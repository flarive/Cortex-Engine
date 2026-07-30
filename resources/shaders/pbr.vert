#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;
layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4 aWeights;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec4 FragPosLightSpace;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;


// coming from code
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

uniform bool reverse_normals;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform bool isAnimated;
uniform bool isTessellated;

void main()
{
    vec4 totalPosition = vec4(aPos, 1.0);
    vec3 totalNormal   = aNormal;

    if (isAnimated)
    {
        totalPosition = vec4(0.0);
        totalNormal = vec3(0.0);

        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            if (aBoneIds[i] == -1)
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

    vec3 worldPos = vec3(model * totalPosition);
    FragPos = worldPos;

    vec3 worldNormal = reverse_normals
        ? normalMatrix * (-totalNormal)
        : normalMatrix * totalNormal;

    Normal = normalize(worldNormal);
    TexCoords = aTexCoords;

    // If tangents are object-space in the mesh, transform them to world space
    Tangent   = normalize(mat3(model) * aTangents);
    Bitangent = normalize(mat3(model) * aBitangents);

    FragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);


    // Usefull for parallax mapping
    vec3 N = normalize(Normal);
    vec3 T = normalize(Tangent);
    vec3 B = normalize(Bitangent);

    // Orthonormalize without destroying handedness
    T = normalize(T - N * dot(N, T));
    B = normalize(B - N * dot(N, B));


    // Columns = T, B, N => transpose to go world => tangent
    mat3 TBN = transpose(mat3(T, B, N));

    TangentLightPos = TBN * lightPos;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * worldPos;



    if (isTessellated)
    {
        // pass OBJECT space to tessellation
        gl_Position = vec4(aPos, 1.0);
    }
    else
    {
        gl_Position = projection * view * model * totalPosition;
    }
}