#version 330 core
out vec4 FragColor;

struct MaterialHeight {
    sampler2D texture_height;
    bool has_texture_height_map;
}; 

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_emissive;
    sampler2D texture_opacity;

    bool has_texture_diffuse_map;
    bool has_texture_specular_map;
    bool has_texture_normal_map;
    bool has_texture_emissive_map;
    bool has_texture_opacity_map;
};

uniform Material material;
uniform MaterialHeight materialHeight;

in Outputs{
 vec2 TexCoords;
}GeoOut;

void main()
{
    FragColor = material.has_texture_diffuse_map ? texture(material.texture_diffuse, GeoOut.TexCoords) : vec4(1, 0, 0, 1);
};