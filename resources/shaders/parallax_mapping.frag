#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

//uniform sampler2D diffuseMap;
//uniform sampler2D normalMap;
//uniform sampler2D depthMap;


// material parameters
struct Material {
    sampler2D texture_diffuse; // 0
    sampler2D texture_specular; // 1
    sampler2D texture_normal; // 2
    sampler2D texture_metallic; // 3
    sampler2D texture_roughness; // 4
    sampler2D texture_ao; // 5
    sampler2D texture_height; // 6
    sampler2D texture_shadowMap; // 10
    float heightScale;
    float shadowIntensity; // Adjust to make shadows darker
    float normalMapIntensity;

    float iblDiffuseIntensity;  // New uniform for diffuse IBL intensity
    float iblSpecularIntensity; // New uniform for specular IBL intensity

    // IBL
    samplerCube texture_irradiance;
    samplerCube texture_prefilter;
    sampler2D texture_brdfLUT;

    bool has_texture_diffuse_map;
    bool has_texture_specular_map;
    bool has_texture_normal_map;
    bool has_texture_metalness_map;
    bool has_texture_roughness_map;
    bool has_texture_ao_map;
    bool has_texture_height_map;
};

uniform Material material;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(material.texture_height, texCoords).r;     
    return texCoords - viewDir.xy * (height * heightScale);        
}

void main()
{           
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    
    texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);       
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // obtain normal from normal map
    vec3 normal = texture(material.texture_normal, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    // get diffuse color
    vec3 color = texture(material.texture_diffuse, texCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}