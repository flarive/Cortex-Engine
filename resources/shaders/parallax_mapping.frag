#version 330 core

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_height;

    vec3 diffuse_color;
    vec3 specular_color;


    float shininess;

    bool has_texture_diffuse_map;
    bool has_texture_specular_map;
    bool has_texture_normal_map;
    bool has_texture_height_map;


    int shadowCalculationMethod;
    float shadowIntensity; // Adjust to make shadows darker
    float shadowMapsBias; // Offset to reduce shadow acne
    float shadowMapsBlur;
    float normalMapIntensity;



    vec4 albedoRoughness; // (x,y,z) = color, w = roughness (for area light only)

    bool canCastShadows;
    bool canReceiveShadows;
}; 

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;



uniform Material material;



uniform float heightScale;

//vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
//{ 
//    // number of depth layers
//    const float minLayers = 8;
//    const float maxLayers = 32;
//    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
//    // calculate the size of each layer
//    float layerDepth = 1.0 / numLayers;
//    // depth of current layer
//    float currentLayerDepth = 0.0;
//    // the amount to shift the texture coordinates per layer (from vector P)
//    vec2 P = viewDir.xy / viewDir.z * heightScale; 
//    vec2 deltaTexCoords = P / numLayers;
//  
//    // get initial values
//    vec2  currentTexCoords     = texCoords;
//    float currentDepthMapValue = material.has_texture_height_map ? texture(material.texture_height, currentTexCoords).r : 0.0;
//      
//    while(currentLayerDepth < currentDepthMapValue)
//    {
//        // shift texture coordinates along direction of P
//        currentTexCoords -= deltaTexCoords;
//        // get depthmap value at current texture coordinates
//        currentDepthMapValue = material.has_texture_height_map ? texture(material.texture_height, currentTexCoords).r : 0.0;  
//        // get depth of next layer
//        currentLayerDepth += layerDepth;  
//    }
//    
//    // get texture coordinates before collision (reverse operations)
//    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
//
//    // get depth after and before collision for linear interpolation
//    float afterDepth  = currentDepthMapValue - currentLayerDepth;
//    float beforeDepth = (material.has_texture_height_map ? texture(material.texture_height, prevTexCoords).r : 0.0) - currentLayerDepth + layerDepth;
// 
//    // interpolation of texture coordinates
//    float weight = afterDepth / (afterDepth - beforeDepth);
//    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
//
//    return finalTexCoords;
//}

vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // Prevent division instability at grazing angles
    viewDir.z = max(viewDir.z, 0.05);

    // Number of layers
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(
        maxLayers,
        minLayers,
        abs(dot(vec3(0.0, 0.0, 1.0), viewDir))
    );

    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    // Direction & per-layer texcoord shift
    vec2 P = viewDir.xy / viewDir.z * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float depthMapValue = material.has_texture_height_map
        ? texture(material.texture_height, currentTexCoords).r
        : 0.0;

    // --- Steep Parallax Loop ---
    for (int i = 0; i < int(numLayers); ++i)
    {
        if (currentLayerDepth >= depthMapValue)
            break;

        currentTexCoords -= deltaTexCoords;
        depthMapValue = material.has_texture_height_map
            ? texture(material.texture_height, currentTexCoords).r
            : 0.0;

        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}

void main()
{           
    // offset texture coordinates with Parallax Mapping
//    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
//    vec2 texCoords = fs_in.TexCoords;
    
    //texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);    
//    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//        discard;

    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = SteepParallaxMapping(fs_in.TexCoords, viewDir);
    
    if (texCoords.x < 0.0 || texCoords.x > 1.0 || texCoords.y < 0.0 || texCoords.y > 1.0)
    {
        discard;
    }



    // obtain normal from normal map
    vec3 normal = material.has_texture_normal_map ? texture(material.texture_normal, texCoords).rgb : vec3(0);
    normal = normalize(normal * 2.0 - 1.0);   
   
    // get diffuse color
    vec3 color = material.has_texture_diffuse_map ? texture(material.texture_diffuse, texCoords).rgb : vec3(0);
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