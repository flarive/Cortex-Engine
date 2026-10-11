#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float bloomThreshold;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    float contribution = max(brightness - bloomThreshold, 0.0);

    FragColor = vec4(color * contribution, 1.0);
}