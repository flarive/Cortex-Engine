#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

// Resolved, non-MSAA floating-point texture (RGBA16F or RGBA32F)
uniform sampler2D screenTexture;

// Exposure control from CPU (e.g., 0.5–4.0)
uniform float exposure;
uniform bool useGamma;
uniform bool useToneMapping;
uniform int applyPostProcessFx;

// ACES filmic tone mapper (Narkowicz 2015)
vec3 ACESFilm(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}


// function prototypes
vec3 apply_fx_invert_colors();


void main()
{
    vec3 color = vec3(0);
    
    if (applyPostProcessFx > 0)
    {
        // simple post processing effect to invert colors
        color = apply_fx_invert_colors();
    }
    else
    {
        color = texture(screenTexture, TexCoords).rgb;
    }
    

    // Exposure
    color = color * exposure;

    // Tone map (for HDR)
    if (useToneMapping)
        color = ACESFilm(color);
        //Alternative simple Reinhard:
        //color = color / (color + vec3(1.0));

    // Gamma correct if your default framebuffer is *linear* (not sRGB)
    if (useGamma)
        color = pow(color, vec3(1.0/2.2));



    // no post process fx
    FragColor = vec4(color, 1.0);
}

vec3 apply_fx_invert_colors()
{
    return vec3(1.0 - texture(screenTexture, TexCoords));
}