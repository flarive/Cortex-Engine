#version 330 core

out vec4 FragColor;

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

    vec3 ambient_color;
    float shininess;

    bool has_texture_diffuse_map;
    bool has_texture_specular_map;
    bool has_texture_normal_map;
    bool has_texture_metalness_map;
    bool has_texture_roughness_map;
    bool has_texture_ao_map;
    bool has_texture_height_map;
    bool has_texture_emissive_map;

    float shadowIntensity; // Adjust to make shadows darker
}; 

struct DirLight {
    bool use;
    
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    bool use;

    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    bool use;
    
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};



// coming from vertex shader
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;

    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform vec3 viewPos;

uniform Material material;




// lights
#define NBR_MAX_LIGHTS 10

uniform int pointLightsCount;
uniform int dirLightsCount;
uniform int spotLightsCount;

uniform PointLight pointLights[NBR_MAX_LIGHTS];
uniform DirLight dirLights[NBR_MAX_LIGHTS];
uniform SpotLight spotLights[NBR_MAX_LIGHTS];



vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);

const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845),
    vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790)
);

float ShadowCalculationSlower(vec4 fragPosLightSpace, vec3 lightPos)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    float shadow = 0.0;
    float bias = max(0.0005 * (1.0 - dot(normalize(fs_in.Normal), normalize(lightPos - fs_in.FragPos))), 0.0001);

    vec2 texelSize = 1.0 / textureSize(material.texture_shadowMap, 0);
    float diskRadius = 1.0 * texelSize.x; // Tweak diskRadius to control softness

    // Combine Poisson disk with 3x3 PCF sampling
    for (int i = 0; i < 16; ++i)
    {
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                vec2 offset = poissonDisk[i] * diskRadius + vec2(x, y) * texelSize;
                float closestDepth = texture(material.texture_shadowMap, projCoords.xy + offset).r;
                shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
            }
        }
    }
    shadow /= (16.0 * 9.0); // Average over all samples

    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}



void main()
{
    vec3 norm;
    if (material.has_texture_normal_map)
    {
        // Sample the normal map texture
        norm = texture(material.texture_normal, fs_in.TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0); // Transform from [0,1] to [-1,1]

        // Transform normal from tangent space to world space
        vec3 T = normalize(fs_in.Tangent);
        vec3 B = normalize(fs_in.Bitangent);
        vec3 N = normalize(fs_in.Normal);
        mat3 TBN = mat3(T, B, N);
        norm = normalize(TBN * norm);
    }
    else
    {
        norm = normalize(fs_in.Normal); // Use the geometry normal as a fallback
    }

    //vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec3 V = normalize(viewPos - fs_in.FragPos); // View direction


    // material properties
    vec3 albedo = material.has_texture_diffuse_map ? pow(texture(material.texture_diffuse, fs_in.TexCoords).rgb, vec3(2.2)) : vec3(0.5); // A neutral gray color
        
    //vec3 color = texture(material.texture_diffuse, fs_in.TexCoords).rgb;
    
    vec3 result = vec3(0.0);

    // Lighting
    for (int i = 0; i < spotLightsCount; i++)
    {
        if (spotLights[i].use)
            result += CalcSpotLight(spotLights[i], norm, fs_in.FragPos, viewDir, albedo);
    }

    // Sample the alpha value from the diffuse texture
    float alpha = texture(material.texture_diffuse, fs_in.TexCoords).a;


    // gamma correct
    //result = pow(result, vec3(1.0/2.2));

    // Set the fragment color with the alpha channel
    FragColor = vec4(result, 1.0);

    
    


    // Discard transparent fragments (optional)
    if (alpha < 0.1)
        discard;
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color)
{
    // Direction from fragment to light
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading (Blinn-Phong or Phong based on 'blinn' flag)
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Attenuation based on distance
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    

    vec3 L = normalize(light.position - fs_in.FragPos);
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;

    // Spotlight intensity based on angle between light direction and fragment (smooth blurry cutoff)
    float intensity = pow(smoothstep(light.outerCutOff, light.cutOff, theta), 2.0);
    
//
//    float theta = dot(lightDir, normalize(-light.direction));
//    float intensity = pow(smoothstep(light.cutOff, light.outerCutOff, theta), 2.0);


    // Ambient, Diffuse, and Specular components
    //vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    float ambientStrength = 0.5;
    vec3 ambient = light.ambient * mix(material.ambient_color, vec3(texture(material.texture_diffuse, fs_in.TexCoords)), ambientStrength);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular, fs_in.TexCoords));

    // Apply attenuation and spotlight intensity
    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    // Shadow calculation (using the light's position for shadow mapping)
    float shadow = ShadowCalculationSlower(fs_in.FragPosLightSpace, light.position);                      

    // Final lighting with shadow applied
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    // debug spot light cut off
    //FragColor = vec4(vec3(intensity), 1.0);

    return lighting;

}
