#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D shadowMap;
    float shininess;

    bool has_diffuse_map;
    bool has_specular_map;
    bool has_normal_map;
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
} fs_in;

uniform vec3 viewPos;
uniform bool blinn;

uniform Material material;



#define NBR_MAX_LIGHTS 4

uniform PointLight pointLights[NBR_MAX_LIGHTS];
uniform DirLight dirLight[NBR_MAX_LIGHTS];
uniform SpotLight spotLight[NBR_MAX_LIGHTS];

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(material.shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


void main()
{
    vec3 norm;
    if (material.has_normal_map)
    {
        // Sample the normal map
        norm = texture(material.texture_normal1, fs_in.TexCoords).rgb;
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

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

        
    vec3 color = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================

    vec3 result = vec3(0.0);

    // phase 1: directional lighting
    for (int i = 0; i < NBR_MAX_LIGHTS; i++)
    {
        if (dirLight[i].use)
            result += CalcDirLight(dirLight[i], norm, viewDir);
    }
    
    // phase 2: point lights
    for (int i = 0; i < NBR_MAX_LIGHTS; i++)
    {
        if (pointLights[i].use)
            result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir, color);
    }
    
    // phase 3: spot light
    for (int i = 0; i < NBR_MAX_LIGHTS; i++)
    {
        if (spotLight[i].use)
            result += CalcSpotLight(spotLight[i], norm, fs_in.FragPos, viewDir);
    }

    // Sample the alpha value from the diffuse texture
    float alpha = texture(material.texture_diffuse1, fs_in.TexCoords).a;

    // Set the fragment color with the alpha channel
    FragColor = vec4(result, alpha);

    // Discard transparent fragments (optional)
    if (alpha < 0.1)
        discard;
}


// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    
    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    

    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }


    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    return lighting;

    //return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading



    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }


    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}