layout(location = 0) in vec3 aPos;

#ifdef OPENGL
layout(std140, binding = 0) uniform CameraUBO
#else
layout(set = 0, binding = 0) uniform CameraUBO
#endif
{
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

void main()
{
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(aPos, 1.0);
}