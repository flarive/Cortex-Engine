//#version 440 core
//
//layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
//layout (location = 1) in vec3 aNormal; // the normal variable has attribute position 1
//layout (location = 2) in vec2 aTexCoords; // the uv variable has attribute position 2
//
//layout (location = 0) out vec3 FragPos;
//layout (location = 1) out vec3 Normal;
//layout (location = 2) out vec2 TexCoords;
//
//layout(set = 0, binding = 0) uniform UBO
//{
//    mat4 model;
//    mat4 view;
//    mat4 projection;
//} ubo;
//
//void main()
//{
//    FragPos = vec3(ubo.model * vec4(aPos, 1.0));
//    Normal = mat3(transpose(inverse(ubo.model))) * aNormal;
//    TexCoords = aTexCoords;
//    
//    gl_Position = ubo.projection * ubo.view * vec4(FragPos, 1.0);
//}

#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;

#ifdef VULKAN
// use Uniform Buffer Object for Vulkan
layout(set = 0, binding = 0) uniform CameraUBO
{
    mat4 model;
    mat4 view;
    mat4 projection;
} camera;

#define MODEL      camera.model
#define VIEW       camera.view
#define PROJECTION camera.projection

#else
// use simple uniforms for OpenGL
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define MODEL      model
#define VIEW       view
#define PROJECTION projection

#endif

void main()
{
    FragPos = vec3(MODEL * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(MODEL))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = PROJECTION * VIEW * vec4(FragPos, 1.0);
}