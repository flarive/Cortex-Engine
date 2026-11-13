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

#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal; // the normal variable has attribute position 1
layout (location = 2) in vec2 aTexCoords; // the uv variable has attribute position 2


out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}