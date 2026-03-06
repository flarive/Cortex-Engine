#version 410 core

in float Height;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_height;

    bool has_texture_diffuse_map;
    bool has_texture_specular_map;
    bool has_texture_normal_map;
    bool has_texture_height_map;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
//    float h = (Height + 16)/64.0f;
//    FragColor = vec4(h, h, h, 1.0);

    // Sample textures
    vec4 texDiffuse = material.has_texture_diffuse_map ? texture(material.texture_diffuse, TexCoords) : vec4(1.0f);
    vec4 texSpecular = material.has_texture_specular_map ? texture(material.texture_specular, TexCoords) : vec4(0.5f);
    vec3 texNormal = material.has_texture_normal_map ? texture(material.texture_normal, TexCoords).xyz : vec3(0.0f);

    // Convert the sampled normal from [0, 1] to [-1, 1]
    texNormal = normalize(texNormal * 2.0 - 1.0);

    // Create a TBN matrix for transforming the normal from tangent to world space
    // For simplicity, assume the tangent and bitangent are already calculated in the vertex shader
    // and passed as interpolated variables (not shown here for brevity)
    // If you don't have a TBN matrix, you can use the interpolated normal directly
    vec3 normal = normalize(Normal);

    // If you have a TBN matrix, you would transform the sampled normal like this:
    // normal = normalize(TBN * texNormal);

    // Calculate lighting components
    vec3 ambient = light.ambient * texDiffuse.rgb;

    // Diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texDiffuse.rgb;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec * texSpecular.rgb;

    // Combine lighting
    vec3 result = ambient + diffuse + specular;

    // Use height to modify the final color (e.g., grayscale overlay)
    float h = (Height + 16.0) / 64.0;
    vec3 heightEffect = vec3(h, h, h);

    // Blend the height effect with the lighting result
    FragColor = vec4(mix(result, heightEffect, 0.3), 1.0);


//    vec4 texDiffuse2 = texture(material.texture_diffuse, TexCoords);
//    FragColor = vec4(texDiffuse2.rgb, 1.0); // Use the texture color

    //FragColor = vec4(TexCoords, 0.0, 1.0); // Debug texture coordinates
}