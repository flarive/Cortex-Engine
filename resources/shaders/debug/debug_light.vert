//#version 330 core
//layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//void main()
//{
//	gl_Position = projection * view * model * vec4(aPos, 1.0);
//}

#version 450

layout(location = 0) in vec3 aPos;

#ifdef VULKAN

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

#define MODEL ubo.model
#define VIEW ubo.view
#define PROJECTION ubo.projection

#else

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define MODEL model
#define VIEW view
#define PROJECTION projection

#endif

void main()
{
    gl_Position = PROJECTION * VIEW * MODEL * vec4(aPos, 1.0);
}