#version 330 core
out vec4 FragColor;
uniform sampler2D texture_diffuse;

in Outputs{
 vec2 TexCoords;
}GeoOut;

void main()
{
	FragColor = texture(texture_diffuse, GeoOut.TexCoords);
};