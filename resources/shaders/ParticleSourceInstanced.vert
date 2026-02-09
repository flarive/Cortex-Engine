#version 330 core

layout (location = 0) in vec2 aPos;
layout(location = 1) in vec3 aTexCoord;
layout(location = 2) in vec3 aOffset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * model * (vec4(aPos.x, aPos.y, 0, 1.0)+vec4(aOffset.xyz,0));
	TexCoords = vec2(aTexCoord.x, aTexCoord.y);
}