//#version 330 core
//
//out vec4 FragColor;
//
//uniform vec4 customColor;
//
//void main()
//{
//    FragColor = customColor;
//}

#version 450

layout(location = 0) out vec4 FragColor;

#ifdef VULKAN

layout(set = 0, binding = 1) uniform ColorUBO
{
    vec4 customColor;
} ubo;

#define COLOR ubo.customColor

#else

uniform vec4 customColor;

#define COLOR customColor

#endif

void main()
{
    FragColor = COLOR;
}