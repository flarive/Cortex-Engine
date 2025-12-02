#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse; // 0
    sampler2D texture_specular; // 1
    sampler2D texture_normal; // 2
    sampler2D texture_metallic; // 3
    sampler2D texture_roughness; // 4
    sampler2D texture_ao; // 5
    sampler2D texture_height; // 6
    sampler2D texture_emissive; // 7
    sampler2D texture_shadowMap; // 10
    samplerCube texture_shadowMapCube; // 11

    sampler2D texture_metalness_from_combined;
    sampler2D texture_roughness_from_combined;

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

    float shininess;

    bool has_texture_diffuse_map;
    bool has_texture_specular_map;
    bool has_texture_normal_map;
    bool has_texture_metalness_map;
    bool has_texture_roughness_map;
    bool has_texture_metalness_from_combined_map;
    bool has_texture_ao_map;
    bool has_texture_height_map;
    bool has_texture_emissive_map;

    int shadowCalculationMethod;
    float shadowIntensity; // Adjust to make shadows darker
    float shadowMapsBias; // Offset to reduce shadow acne
    float shadowMapsBlur;

    vec4 albedoRoughness; // (x,y,z) = color, w = roughness (for area light only)

    bool canCastShadows;
    bool canReceiveShadows;
}; 




uniform Material material;

void main()
{    
    FragColor = texture(material.texture_diffuse, TexCoords);
}