#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform bool useHDR;

// Resolved, non-MSAA floating-point texture (RGBA16F or RGBA32F)
uniform sampler2D screenTexture;

// Exposure control from CPU (e.g., 0.5–4.0)
uniform float exposure;

// ACES filmic tone mapper (Narkowicz 2015)
vec3 ACESFilm(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 color = vec3(0);
//    
//    if (useHDR)
//    {
//        vec3 hdr = texture(screenTexture, TexCoords).rgb;
//
//        // 1) Exposure
//        vec3 color = hdr * exposure;
//
//        // 2) Tone map (choose one curve)
//        color = ACESFilm(color);
//        // Alternative simple Reinhard:
//        // color = color / (color + vec3(1.0));
//
//        // 3) Gamma correct if your default framebuffer is *linear* (not sRGB)
//        // Remove this pow if you enable GL_FRAMEBUFFER_SRGB on the default framebuffer.
//        color = pow(color, vec3(1.0/2.2));
//    }
//    else
//    {
        color = texture(screenTexture, TexCoords).rgb;
//    }

    FragColor = vec4(color, 1.0);
        
    // simple post processing effect to invert colors
    //FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}