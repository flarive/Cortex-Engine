#version 330 core

out vec4 FragColor;
uniform sampler2D texture1;
in vec2 TexCoords;

void main()
{
	FragColor = vec4(1.0, 0.0, 0.0, 1.0); // texture(texture1, TexCoords);
};