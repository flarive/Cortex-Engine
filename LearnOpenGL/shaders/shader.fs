#version 330 core

out vec4 FragColor;

in vec3 ourColor; // coming from vertex shader

uniform vec4 rotatingColor; // we set this variable in the OpenGL code.

void main()
{
    FragColor = vec4(ourColor.x + rotatingColor.x, ourColor.y + rotatingColor.y, ourColor.z + rotatingColor.z, 1.0);
};