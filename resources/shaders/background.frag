#version 330 core

#define pow2(x) (x * x)



out vec4 FragColor;
in vec3 WorldPos;
in vec2 v_texcoord;

uniform samplerCube environmentMap;

const float pi = 3.14159265359;
const int samples = 2;
const float sigma = float(samples) * 0.25;

uniform vec2 u_resolution;


float gaussian(vec2 i) {
    return 1.0 / (2.0 * pi * pow2(sigma)) * exp(-((pow2(i.x) + pow2(i.y)) / (2.0 * pow2(sigma))));
}

vec3 blur(samplerCube cubeMap, vec3 dir, float spread) {
    vec3 col = vec3(0.0);
    float accum = 0.0;
    float weight;
    vec2 offset;

    // Create an orthonormal basis around the direction
    vec3 up = abs(dir.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(dir, up));
    vec3 top = normalize(cross(right, dir));

    for (int x = -samples / 2; x < samples / 2; ++x) {
        for (int y = -samples / 2; y < samples / 2; ++y) {
            offset = vec2(x, y);
            weight = gaussian(offset);

            // Offset the direction slightly
            vec3 sampleDir = normalize(dir + spread * (offset.x * right + offset.y * top));
            col += texture(cubeMap, sampleDir).rgb * weight;
            accum += weight;
        }
    }

    return col / accum;
}


void main()
{		
    //vec3 envColor = textureLod(environmentMap, WorldPos, 0.0).rgb;
    vec3 envColor = texture(environmentMap, normalize(WorldPos)).rgb;

    //vec3 envColor = blur(environmentMap, normalize(WorldPos), 0.1);
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);
}