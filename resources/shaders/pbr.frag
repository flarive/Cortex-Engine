#version 330 core

out vec4 FragColor;

// coming from vertex shader
in VS_OUT {
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;

} fs_in;

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


uniform Material material;

// lights
#define NBR_MAX_LIGHTS 10

uniform int pointLightsCount;
uniform int dirLightsCount;
uniform int spotLightsCount;

uniform PointLight pointLights[NBR_MAX_LIGHTS];
uniform DirLight dirLights[NBR_MAX_LIGHTS];
uniform SpotLight spotLights[NBR_MAX_LIGHTS];

uniform bool hasTangents; // does the primitive to render has tangents and bitangents ?

uniform vec3 camPos;
uniform mat4 lightSpaceMatrix;

const float PI = 3.14159265359;



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



uniform vec3 viewPos;


// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness);


// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anyways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
//vec3 getNormalFromMapOld()
//{
//    vec3 tangentNormal = texture(material.texture_normal, fs_in.TexCoords).xyz * 2.0 - 1.0;
//
//    vec3 Q1  = dFdx(fs_in.WorldPos);
//    vec3 Q2  = dFdy(fs_in.WorldPos);
//    vec2 st1 = dFdx(fs_in.TexCoords);
//    vec2 st2 = dFdy(fs_in.TexCoords);
//
//    vec3 N   = normalize(fs_in.Normal);
//    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
//    vec3 B  = -normalize(cross(N, T));
//    mat3 TBN = mat3(T, B, N);
//
//    return normalize(TBN * tangentNormal);
//}

vec3 getNormalFromMap()
{
    // Sample the normal map and convert the range from [0, 1] to [-1, 1]
    vec3 tangentNormal = texture(material.texture_normal, fs_in.TexCoords).xyz * 2.0 - 1.0;

    // Scale the normal map values by the normal map intensity
    tangentNormal = mix(vec3(0.0), tangentNormal, material.normalMapIntensity);

    // Compute the TBN matrix using either precomputed tangents or derivatives
    vec3 N = normalize(fs_in.Normal);
    vec3 T, B;

    if (hasTangents) { // If tangents exist, use them
        T = normalize(fs_in.Tangent);
        B = normalize(fs_in.Bitangent);
    } else { // Otherwise, compute them using screen-space derivatives
        vec3 Q1  = dFdx(fs_in.WorldPos);
        vec3 Q2  = dFdy(fs_in.WorldPos);
        vec2 st1 = dFdx(fs_in.TexCoords);
        vec2 st2 = dFdy(fs_in.TexCoords);
        T  = normalize(Q1*st2.t - Q2*st1.t);
        B  = -normalize(cross(N, T));
    }

    // Construct the TBN matrix
    mat3 TBN = mat3(T, B, N);

    // Transform the normal from tangent space to world space
    return normalize(TBN * tangentNormal);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   
// ----------------------------------------------------------------------------
vec2 parallaxMapping(vec2 texCoords, vec3 viewDir) {
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 deltaTexCoords = viewDir.xy * material.heightScale / numLayers;
    vec2 currentTexCoords = texCoords;

    float heightFromTexture = texture(material.texture_height, currentTexCoords).r;

    while (currentLayerDepth < heightFromTexture) {
        currentTexCoords -= deltaTexCoords;
        heightFromTexture = texture(material.texture_height, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}

float ShadowCalculationPCF(vec4 fragPosLightSpace, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth = texture(material.texture_shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    //float bias = max(0.002 * (1.0 - dot(fs_in.Normal, lightDir)), 0.0005);
    float bias = max(0.0005 * (1.0 - dot(normalize(fs_in.Normal), normalize(lightDir - fs_in.WorldPos))), 0.0001);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(material.texture_shadowMap, 0);
    
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(material.texture_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    shadow = clamp(shadow * material.shadowIntensity, 0.0, 1.0);

    return shadow;
}

// ----------------------------------------------------------------------------
void main()
{		
    // input lighting data
    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - fs_in.WorldPos); // View direction
    vec3 R = reflect(-V, N); 



    // Modify TexCoords using Parallax Mapping
    vec2 modifiedTexCoords = parallaxMapping(fs_in.TexCoords, V);
    modifiedTexCoords = clamp(modifiedTexCoords, vec2(0.0), vec2(1.0));

    // material properties
    vec3 albedo = material.has_texture_diffuse_map ? pow(texture(material.texture_diffuse, fs_in.TexCoords).rgb, vec3(2.2)) : vec3(0.5); // A neutral gray color
    float metallic = material.has_texture_metalness_map ? texture(material.texture_metallic, fs_in.TexCoords).r : 0.0; // Non-metallic;
    float roughness = material.has_texture_roughness_map ? texture(material.texture_roughness, fs_in.TexCoords).r : 0.5; // Moderate roughness
    float ao = material.has_texture_ao_map ? texture(material.texture_ao, fs_in.TexCoords).r : 0.0; // Full ambient occlusion

//    vec3 albedo = vec3(0.5); // A neutral gray color
//    float metallic = 0.0;    // Non-metallic
//    float roughness = 0.5;   // Moderate roughness
//    float ao = 0.0;          // Full ambient occlusion


    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < spotLightsCount; i++)
    {
        if (spotLights[i].use)
            Lo += CalcSpotLight(spotLights[i], N, V, albedo, metallic, roughness);
    }

    for (int i = 0; i < pointLightsCount; i++)
    {
        if (pointLights[i].use)
            Lo += CalcPointLight(pointLights[i], N, fs_in.WorldPos, V, albedo, metallic, roughness);
    }

    for (int i = 0; i < dirLightsCount; i++)
    {
        if (dirLights[i].use)
            Lo += CalcDirLight(dirLights[i], N, fs_in.WorldPos, V, vec3(1.0));
    }


    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = 1.0 - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(material.texture_irradiance, N).rgb;
    vec3 diffuse = irradiance * albedo * material.iblDiffuseIntensity; // Apply iblDiffuseIntensity

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(material.texture_prefilter, R, roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(material.texture_brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y) * material.iblSpecularIntensity; // Apply iblSpecularIntensity

    vec3 ambient = (kD * diffuse + specular) * ao;




    // add light and shadow contribution
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));

    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color , 1.0);

    // debug ao map
    //FragColor = vec4(vec3(ao), 1.0);

    // debug shadows
//    float shadow = ShadowCalculationSlower(fs_in.FragPosLightSpace, spotLights[0].position);
//    FragColor = vec4(vec3(shadow), 1.0); // Debugging shadow output
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
    vec3 L = normalize(light.position - fs_in.WorldPos);
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;

    // Spotlight intensity based on angle between light direction and fragment (smooth blurry cutoff)
    float intensity = pow(smoothstep(light.outerCutOff, light.cutOff, theta), 2.0);

    if (theta < light.outerCutOff)
        return vec3(0.0);
    
    vec3 H = normalize(viewDir + L);
    float NdotL = max(dot(normal, L), 0.0);
    
    float distance = length(light.position - fs_in.WorldPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 radiance = light.diffuse * attenuation * intensity;

    // Compute shadow factor
    float shadow = ShadowCalculationPCF(fs_in.FragPosLightSpace, light.direction);

    // Apply shadow factor to the light intensity
    radiance *= (1.0 - shadow * material.shadowIntensity);  
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(NdotL, 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);

    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
    vec3 L = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 radiance = light.diffuse * attenuation;

    // Cook-Torrance BRDF
    vec3 H = normalize(viewDir + L);
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);


    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color)
{
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);
    
    float NdotL = max(dot(normal, L), 0.0);

    vec3 radiance = light.diffuse;

    // Shadow calculation
    float shadow = ShadowCalculationPCF(fs_in.FragPosLightSpace, light.direction);
    radiance *= (1.0 - shadow);  

    vec3 F0 = mix(vec3(0.04), color, 1.0);  
    vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);
    float NDF = DistributionGGX(normal, H, 1.0);
    float G = GeometrySmith(normal, viewDir, L, 1.0);
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(NdotL, 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (1.0 - kS);

    return (kD * color / PI + specular) * radiance * NdotL;
}

