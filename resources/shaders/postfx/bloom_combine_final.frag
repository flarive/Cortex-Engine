#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;

uniform bool useBloom;
uniform bool useToneMapping;
uniform bool useGamma;

uniform float bloomStrength;
uniform float exposure;

void main()
{
    vec3 hdrColor =
        texture(screenTexture, TexCoords).rgb;

    if (useBloom)
    {
        vec3 bloom =
            texture(bloomTexture, TexCoords).rgb;

        hdrColor += bloom * bloomStrength;
    }

    vec3 result = hdrColor;

    // Reinhard tone mapping
    if (useToneMapping)
    {
        result = vec3(1.0) - exp(-result * exposure);
    }

    // Gamma correction
    if (useGamma)
    {
        result = pow(result, vec3(1.0 / 2.2));
    }

    FragColor = vec4(result, 1.0);
}