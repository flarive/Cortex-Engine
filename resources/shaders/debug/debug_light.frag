#version 450

layout(location = 0) out vec4 FragColor;

#ifdef OPENGL
layout(std140, binding = 1) uniform ColorUBO
#else
layout(set = 0, binding = 1) uniform ColorUBO
#endif
{
    vec4 customColor;
} ubo;

void main()
{
    FragColor = ubo.customColor;
}
