#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform vec4 topcolor;
uniform vec4 bottomcolor;
uniform float ySplit; // Uniform for controlling the split point

void main()
{
    // Remap TexCoords.y so 75% of the screen is top color
    float y = smoothstep(0.0, 1.0, TexCoords.y * ySplit + (1.0 - ySplit));
    
    // Vertical gradient: top to bottom
    FragColor = mix(topcolor, bottomcolor, y);
}
