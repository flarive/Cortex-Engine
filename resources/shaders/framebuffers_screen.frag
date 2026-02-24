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
vec3 apply_fx();
vec3 apply_fx_invert_colors();
vec3 apply_fx_grayscale();
vec3 apply_fx_sepia();
vec3 apply_fx_blur();
vec3 apply_fx_edge_detection();
vec3 apply_fx_vignette();
vec3 apply_fx_chromatic_aberration();
vec3 apply_fx_pixelate();
vec3 apply_fx_scanlines();
vec3 apply_fx_bloom();



void main()
{
    vec3 color = apply_fx();
    
//    if (applyPostProcessFx > 0)
//    {
//        // simple post processing effect to invert colors
//        color = apply_fx_invert_colors();
//    }
//    else
//    {
//        color = texture(screenTexture, TexCoords).rgb;
//    }
    

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

vec3 apply_fx()
{
    switch (applyPostProcessFx)
    {
        case 1: return apply_fx_invert_colors();
        case 2: return apply_fx_grayscale();
        case 3: return apply_fx_sepia();
        case 4: return apply_fx_blur();
        case 5: return apply_fx_edge_detection();
        case 6: return apply_fx_vignette();
        case 7: return apply_fx_chromatic_aberration();
        case 8: return apply_fx_pixelate();
        case 9: return apply_fx_scanlines();
        case 10: return apply_fx_bloom();
        default: return texture(screenTexture, TexCoords).rgb;
    }
}

vec3 apply_fx_invert_colors()
{
    return vec3(1.0 - texture(screenTexture, TexCoords));
}

vec3 apply_fx_grayscale()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    float gray = dot(col, vec3(0.299, 0.587, 0.114));
    return vec3(gray);
}

vec3 apply_fx_sepia()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    vec3 sepia = vec3(
        dot(col, vec3(0.393, 0.769, 0.189)),
        dot(col, vec3(0.349, 0.686, 0.168)),
        dot(col, vec3(0.272, 0.534, 0.131))
    );
    return sepia;
}

vec3 apply_fx_blur()
{
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    vec3 col = vec3(0.0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            col += texture(screenTexture, TexCoords + vec2(x, y) * texelSize).rgb;
        }
    }
    return col / 9.0;
}

vec3 apply_fx_edge_detection()
{
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    float gx = -texture(screenTexture, TexCoords + vec2(-texelSize.x, -texelSize.y)).r - 2.0 * texture(screenTexture, TexCoords + vec2(-texelSize.x, 0.0)).r - texture(screenTexture, TexCoords + vec2(-texelSize.x, texelSize.y)).r
               + texture(screenTexture, TexCoords + vec2(texelSize.x, -texelSize.y)).r + 2.0 * texture(screenTexture, TexCoords + vec2(texelSize.x, 0.0)).r + texture(screenTexture, TexCoords + vec2(texelSize.x, texelSize.y)).r;
    float gy = -texture(screenTexture, TexCoords + vec2(-texelSize.x, -texelSize.y)).r - 2.0 * texture(screenTexture, TexCoords + vec2(0.0, -texelSize.y)).r - texture(screenTexture, TexCoords + vec2(texelSize.x, -texelSize.y)).r
               + texture(screenTexture, TexCoords + vec2(-texelSize.x, texelSize.y)).r + 2.0 * texture(screenTexture, TexCoords + vec2(0.0, texelSize.y)).r + texture(screenTexture, TexCoords + vec2(texelSize.x, texelSize.y)).r;
    float edge = sqrt(gx * gx + gy * gy);
    return vec3(edge);
}

vec3 apply_fx_vignette()
{
    vec2 uv = TexCoords;
    vec2 center = vec2(0.5);
    float dist = distance(uv, center);
    float vignette = 1.0 - (dist * 0.5);
    return texture(screenTexture, TexCoords).rgb * vignette;
}

vec3 apply_fx_chromatic_aberration()
{
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    float offset = 0.002;
    vec3 col;
    col.r = texture(screenTexture, TexCoords + vec2(offset, 0.0)).r;
    col.g = texture(screenTexture, TexCoords).g;
    col.b = texture(screenTexture, TexCoords - vec2(offset, 0.0)).b;
    return col;
}

vec3 apply_fx_pixelate()
{
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    vec2 pixelSize = vec2(10.0) * texelSize; // Adjust pixel size
    vec2 uv = floor(TexCoords / pixelSize) * pixelSize;
    return texture(screenTexture, uv).rgb;
}

vec3 apply_fx_scanlines()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    float scanline = sin(TexCoords.y * 1000.0) * 0.2 + 0.8;
    return col * scanline;
}

vec3 apply_fx_bloom()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    vec3 bloom = texture(screenTexture, TexCoords).rgb * 0.5; // Simplified
    return col + bloom;
}