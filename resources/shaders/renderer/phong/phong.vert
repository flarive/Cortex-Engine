#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;



#ifdef OPENGL
layout(std140, binding = 0) uniform PhongCameraUBO
#else
layout(set = 0, binding = 0) uniform PhongCameraUBO
#endif
{
    mat4 view;
    mat4 projection;
} camera;


#ifdef OPENGL
layout(std140, binding = 1) uniform PhongModelUBO
#else
layout(set = 0, binding = 1) uniform PhongModelUBO
#endif
{
    mat4 model;
} model;


void main()
{
    FragPos = vec3(model.model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model.model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = camera.projection * camera.view * vec4(FragPos, 1.0);
}