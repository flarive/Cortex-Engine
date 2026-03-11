#version 410 core

layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal; // the normal variable has attribute position 1
layout (location = 2) in vec2 aTexCoords; // the uv variable has attribute position 2
layout (location = 3) in vec3 aTangents; // the tangent variable has attribute position 3
layout (location = 4) in vec3 aBitangents; // the bitangent variable has attribute position 4
layout (location = 5) in ivec4 aBoneIds;  // Only used for animated models
layout (location = 6) in vec4 aWeights;    // Only used for animated models

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out VS_OUT {
    vec3 worldPos;
} vs_out;

void main()
{
    // Handle bone animation if the model is animated
    vec4 objPos = vec4(aPos, 1.0);
    vec3 totalNormal = aNormal;
    
    // Pass world-space positions to tessellation
    vs_out.worldPos = vec3(model * objPos);
}