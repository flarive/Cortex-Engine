#version 330 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float depth = texture(depthMap, TexCoords).r;
    float linearDepth = LinearizeDepth(depth) / far_plane; // normalize 0..1
    FragColor = linearDepth;
}
