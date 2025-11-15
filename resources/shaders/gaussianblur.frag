#version 460 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;
uniform int kernelRadius; // actual radius in use
const int MAX_RADIUS = 10; // must match C++ MAX_RADIUS
uniform float weights[2 * MAX_RADIUS + 1];
uniform float blurScale; // optional - how far to sample relative to texel

void main()
{
    ivec2 texSize = textureSize(image, 0);
    vec2 texel = 1.0 / vec2(texSize);
    float result = 0.0;

    // center index in weights array
    int center = kernelRadius;

    for (int i = -kernelRadius; i <= kernelRadius; ++i)
    {
        int idx = i + center; // 0 .. 2*kernelRadius
        vec2 offset = horizontal ? vec2(texel.x * float(i) * blurScale, 0.0)
                                 : vec2(0.0, texel.y * float(i) * blurScale);
        result += texture(image, TexCoords + offset).r * weights[idx];
    }

    FragColor = result;
}
