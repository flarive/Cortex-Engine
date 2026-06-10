#version 440 core

// coming from previous shader
// pbr.frag if no tessellation (VS => FS)
// height.tes if tesselation (VS => TCS => TES => FS)
in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 FragPosLightSpace;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;


in float Height;

// material parameters
struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_normal;
    sampler2D texture_metalness;
    sampler2D texture_roughness;
    sampler2D texture_ao;
    sampler2D texture_emissive;
    

    sampler2D texture_metalness_from_combined;

    int shadowCalculationMethod;
    float shadowIntensity; // Adjust to make shadows darker
    float shadowMapsBias; // Offset to reduce shadow acne
    float shadowMapsBlur;
    float normalMapIntensity;
    float emissiveIntensity;
    float parallaxMapIntensity;

    vec3 ambient_color;
    float ambient_intensity;



    float iblDiffuseIntensity;  // New uniform for diffuse IBL intensity
    float iblSpecularIntensity; // New uniform for specular IBL intensity

    // IBL
    samplerCube texture_irradiance; // 7
    samplerCube texture_prefilter; // 8
    sampler2D texture_brdfLUT; // 9

    vec4 albedoRoughness; // (x,y,z) = color, w = roughness (for area light only)

    bool has_texture_diffuse_map;
    bool has_texture_normal_map;
    bool has_texture_metalness_map;
    bool has_texture_roughness_map;
    bool has_texture_metalness_from_combined_map;
    bool has_texture_ao_map;
    bool has_texture_emissive_map;

    bool canCastShadows;
    bool canReceiveShadows;

    bool useParallaxMapping;
};

struct MaterialHeight {
    sampler2D texture_height;
    bool has_texture_height_map;
}; 

struct DirLight {
    bool use;
    
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float intensity;
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

    float intensity;
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
    
    float intensity;
};

struct AreaLight {
    bool use;

    float intensity;
	vec3 color;
    vec3 points[4];
	bool twoSided;
};

// coming from code
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform float far_plane;
uniform bool enableShadows;
uniform bool hasTangents; // does the primitive to render has tangents and bitangents ?
uniform Material material;
uniform MaterialHeight materialHeight;
uniform mat4 lightSpaceMatrix;

uniform bool isTessellated;

uniform sampler2D texture_shadowMap;
uniform samplerCube texture_shadowMapCube;

// area light only
uniform sampler2D LTC1; // 20 (for inverse M)
uniform sampler2D LTC2; // 21 (GGX norm, fresnel, 0(unused), sphere)

const float LUT_SIZE  = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;

// shader output
out vec4 FragColor;


// lights
#define NBR_MAX_LIGHTS 16

uniform int pointLightsCount;
uniform int dirLightsCount;
uniform int spotLightsCount;
uniform int areaLightsCount;

uniform PointLight pointLights[NBR_MAX_LIGHTS];
uniform DirLight dirLights[NBR_MAX_LIGHTS];
uniform SpotLight spotLights[NBR_MAX_LIGHTS];
uniform AreaLight areaLights[NBR_MAX_LIGHTS];



const float PI = 3.14159265359;


// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 geoNormal, vec3 fragPos, vec2 texCoords, vec3 viewDir, vec3 color);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 geoNormal, vec3 fragPos, vec2 texCoords, vec3 viewDir, vec3 albedo, float metallic, float roughness);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 geoNormal, vec3 fragPos, vec2 texCoords, vec3 viewDir, vec3 albedo, float metallic, float roughness);
vec3 CalcAreaLight(AreaLight light, vec3 normal, vec3 N, vec3 V, vec3 P, mat3 Minv, vec4 t1, vec4 t2, vec3 mDiffuse, vec3 mSpecular);





const int POISSON_SAMPLES = 4;

const vec2 poissonDisk4[4] = vec2[](
    vec2(0.130, 0.870),
    vec2(0.570, 0.430),
    vec2(0.930, 0.720),
    vec2(0.420, 0.920)
);

const vec2 poissonDisk8[8] = vec2[](
    vec2(0.14, 0.43),
    vec2(0.65, 0.15),
    vec2(0.33, 0.87),
    vec2(0.76, 0.75),
    vec2(0.19, 0.62),
    vec2(0.51, 0.33),
    vec2(0.83, 0.44),
    vec2(0.25, 0.98)
);

const vec2 poissonDisk16[16] = vec2[](
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

// Vector form without project to the plane (dot with the normal)
// Use for proxy sphere clipping
vec3 IntegrateEdgeVec(vec3 v1, vec3 v2)
{
    // Using built-in acos() function will result flaws
    // Using fitting result for calculating acos()
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

// P is fragPos in world space (LTC distribution)
vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4], bool twoSided, vec3 mDiffuse)
{
    // construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N * dot(V, N));
    T2 = cross(N, T1);

    // rotate area light in (T1, T2, N) basis
    Minv = Minv * transpose(mat3(T1, T2, N));
	//Minv = Minv * transpose(mat3(N, T2, T1));

    // polygon (allocate 4 vertices for clipping)
    vec3 L[4];
    // transform polygon from LTC back to origin Do (cosine weighted)
    L[0] = Minv * (points[0] - P);
    L[1] = Minv * (points[1] - P);
    L[2] = Minv * (points[2] - P);
    L[3] = Minv * (points[3] - P);

    // use tabulated horizon-clipped sphere
    // check if the shading point is behind the light
    vec3 dir = points[0] - P; // LTC space
    vec3 lightNormal = cross(points[1] - points[0], points[3] - points[0]);
    bool behind = (dot(dir, lightNormal) < 0.0);

    // cos weighted space
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);

	// integrate
    vec3 vsum = vec3(0.0);
    vsum += IntegrateEdgeVec(L[0], L[1]);
    vsum += IntegrateEdgeVec(L[1], L[2]);
    vsum += IntegrateEdgeVec(L[2], L[3]);
    vsum += IntegrateEdgeVec(L[3], L[0]);

    // form factor of the polygon in direction vsum
    float len = length(vsum);

    float z = vsum.z/len;
    if (behind)
        z = -z;

    vec2 uv = vec2(z*0.5f + 0.5f, len); // range [0, 1]
    uv = uv*LUT_SCALE + LUT_BIAS;

    // Fetch the form factor for horizon clipping
    float scale = texture(LTC2, uv).w;

    float sum = len*scale;
    if (!behind && !twoSided)
        sum = 0.0;

    // Outgoing radiance (solid angle) for the entire polygon
    // vec3 Lo_i = vec3(sum, sum, sum);
    // return Lo_i;

    vec3 Lo_i = vec3(sum, sum, sum) * mDiffuse; // Apply diffuse albedo here
    return Lo_i;
}

// PBR-maps for roughness (and metallic) are usually stored in non-linear
// color space (sRGB), so we use these functions to convert into linear RGB.
vec3 PowVec3(vec3 v, float p)
{
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

const float gamma = 2.2;
vec3 ToLinear(vec3 v) { return PowVec3(v, gamma); }
vec3 ToSRGB(vec3 v)   { return PowVec3(v, 1.0/gamma); }

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anyways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap(vec2 texCoords)
{
    // Sample the normal map and convert the range from [0, 1] to [-1, 1]
    vec3 tangentNormal = material.has_texture_normal_map ? texture(material.texture_normal, texCoords).xyz * 2.0 - 1.0 : normalize(Normal) * material.normalMapIntensity;; // caca

    // Blend towards (0,0,1) instead of (0,0,0)
    tangentNormal = mix(vec3(0.0, 0.0, 1.0), tangentNormal, material.normalMapIntensity);
    //tangentNormal.z = -tangentNormal.z;

    // Compute the TBN matrix using either precomputed tangents or derivatives
    vec3 N = normalize(Normal);
    vec3 T, B;

    if (hasTangents) { // If tangents exist, use them
        T = normalize(Tangent);
        B = normalize(Bitangent);
    } else { // Otherwise, compute them using screen-space derivatives
        vec3 Q1  = dFdx(FragPos);
        vec3 Q2  = dFdy(FragPos);
        vec2 st1 = dFdx(TexCoords);
        vec2 st2 = dFdy(TexCoords);
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

// simple hard edged shadow calculation
float ShadowCalculationSimple(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Reject fragments outside light frustum / shadow map
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 0.0;
    }

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(texture_shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

// PCF soft shadows
float ShadowCalculationSoft(vec4 fragPosLightSpace, vec3 geoNormal, vec3 lightPos)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Reject fragments outside light frustum / shadow map
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 0.0;
    }

    vec3 N = normalize(geoNormal);
    vec3 L = normalize(lightPos - FragPos);

    float currentDepth = projCoords.z;

    // Bias slope-scaled
    float bias = max(0.001 * (1.0 - dot(N, L)), 0.0002);

    vec2 texelSize = 1.0 / vec2(textureSize(texture_shadowMap, 0));

    float shadowSoftness = material.shadowMapsBlur;
    vec2 diskRadius = shadowSoftness * texelSize;

    float shadow = 0.0;
    int totalSamples = 0;

    for (int i = 0; i < 16; ++i)
    {
        // Suppose poissonDisk16 déjà centré autour de (0,0)
        vec2 poissonOffset = poissonDisk16[i] * diskRadius;

        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                vec2 offset = poissonOffset + vec2(x, y) * texelSize;
                float closestDepth = texture(texture_shadowMap, projCoords.xy + offset).r;

                shadow += ((currentDepth - bias) > closestDepth) ? 1.0 : 0.0;
                totalSamples++;
            }
        }
    }

    shadow /= float(totalSamples);
    return clamp(shadow * material.shadowIntensity, 0.0, 1.0);
}

float ShadowCalculationPCF(vec4 fragPosLightSpace, vec3 geoNormal, vec3 lightDir)
{
    // Transform fragment position to light space
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Reject fragments outside light frustum / shadow map
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 0.0;
    }

    // Normal-offset bias: move fragment slightly along normal in light space
    vec3 normal = normalize(geoNormal);
    vec3 offsetPos = FragPos + normal * 0.005; // tweak offset for acne reduction
    vec4 offsetLightSpace = lightSpaceMatrix * vec4(offsetPos, 1.0);
    vec3 offsetProjCoords = offsetLightSpace.xyz / offsetLightSpace.w;
    offsetProjCoords = offsetProjCoords * 0.5 + 0.5;

    // Bias based on angle
    float bias = max(0.005 * (1.0 - dot(normal, normalize(lightDir))), 0.0005);

    // Shadow map texel size
    vec2 texelSize = 1.0 / textureSize(texture_shadowMap, 0);

    // Fixed blur radius and Gaussian sigma
    int radius = int(material.shadowMapsBlur);        // e.g., 2 for 5x5 kernel
    float sigma = float(radius) * 1.0;

    float shadow = 0.0;
    float totalWeight = 0.0;

    for (int x = -radius; x <= radius; ++x) {
        for (int y = -radius; y <= radius; ++y) {
            // Gaussian weight
            float weight = exp(-(x*x + y*y) / (2.0 * sigma * sigma));

            // Sample shadow map
            float pcfDepth = texture(texture_shadowMap, offsetProjCoords.xy + vec2(x, y) * texelSize).r;

            shadow += weight * ((offsetProjCoords.z - bias > pcfDepth) ? 1.0 : 0.0);
            totalWeight += weight;
        }
    }

    shadow /= totalWeight; // Normalize by total weight
    shadow = clamp(shadow * material.shadowIntensity, 0.0, 1.0);

    return shadow;
}

float rand2(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

mat2 rotationMatrix(float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

const int PCSS_SAMPLES = 16; // 16 = medium quality, 24 = high quality, 32 = ultra smooth

const vec2 poissonDiskPCSS[PCSS_SAMPLES] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2( 0.94558609, -0.76890725),
    vec2(-0.09418410, -0.92938870),
    vec2( 0.34495938,  0.29387760),
    vec2(-0.91588581,  0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543,  0.27676845),
    vec2( 0.97484398,  0.75648379),
    vec2( 0.44323325, -0.97511554),
    vec2( 0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2( 0.79197514,  0.19090188),
    vec2(-0.24188840,  0.99706507),
    vec2(-0.81409955,  0.91437590),
    vec2( 0.19984126,  0.78641367),
    vec2( 0.14383161, -0.14100790)
);


// PCSS (Percentage-Closer Soft Shadows) is a PCF better variant
// Crisp shadows (like Sun at noon):
// float searchRadius = 4.0 * texelSize.x;
// float filterRadius = penumbra * 18.0 * texelSize.x;
// Realistic daylight:
// float searchRadius = 5.0 * texelSize.x;
// float filterRadius = penumbra * 30.0 * texelSize.x;
// Very soft cloudy-day shadows:
// float searchRadius = 7.0 * texelSize.x;
// float filterRadius = penumbra * 55.0 * texelSize.x;
float ShadowCalculationPCSS(vec4 fragPosLightSpace, vec3 geoNormal)
{
    // Project fragment from light space to shadow map UV
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Reject fragments outside light frustum / shadow map
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 0.0;
    }

    float currentDepth = projCoords.z;

    // Bias
    float bias = material.shadowMapsBias; // 0.001

    // Shadow map texel size
    vec2 texelSize = 1.0 / textureSize(texture_shadowMap, 0);

    // =========================================================
    // 1. BLOCKER SEARCH
    // =========================================================
    float avgBlockerDepth = 0.0;
    int blockerCount = 0;

    //float searchRadius = 3.0 * texelSize.x; // tighter
    float searchRadius = 6.0 * texelSize.x; // broader penumbra

    for (int i = 0; i < PCSS_SAMPLES; ++i)
    {
        vec2 disk = poissonDiskPCSS[i] * searchRadius;
        float depthSample = texture(texture_shadowMap, projCoords.xy + disk).r;

        if (depthSample < currentDepth - bias)
        {
            avgBlockerDepth += depthSample;
            blockerCount++;
        }
    }

    if (blockerCount == 0)
        return 0.0; // No blocker = fully lit

    avgBlockerDepth /= float(blockerCount);

    // =========================================================
    // 2. PENUMBRA SIZE
    // =========================================================
    float receiver = currentDepth;
    float penumbra = (receiver - avgBlockerDepth) / max(avgBlockerDepth, 0.0001);

    //float filterRadius = penumbra * 15.0 * texelSize.x; // crisp
    //float filterRadius = penumbra * 25.0 * texelSize.x; // medium soft
    //float filterRadius = penumbra * 50.0 * texelSize.x; // very soft

    float filterRadius = penumbra * material.shadowMapsBlur * texelSize.x; // very soft

    // =========================================================
    // 3. PCF FILTERING w/ POISSON ROTATION
    // =========================================================

    float shadow = 0.0;

    // rotate Poisson disk - removes banding
    float angle = rand2(gl_FragCoord.xy) * 6.2831853;
    mat2 rot = rotationMatrix(angle);

    for (int i = 0; i < PCSS_SAMPLES; ++i)
    {
        vec2 disk = rot * poissonDiskPCSS[i];
        vec2 offset = disk * filterRadius;

        float sampleDepth = texture(texture_shadowMap, projCoords.xy + offset).r;

        shadow += currentDepth - bias > sampleDepth ? 1.0 : 0.0;
    }

    shadow /= float(PCSS_SAMPLES);

    return shadow;
}

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
    vec2 P = viewDir.xy / viewDir.z * material.parallaxMapIntensity;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float depthMapValue = materialHeight.has_texture_height_map
        ? texture(materialHeight.texture_height, currentTexCoords).r
        : 0.0;

    // --- Steep Parallax Loop ---
    for (int i = 0; i < int(numLayers); ++i)
    {
        if (currentLayerDepth >= depthMapValue)
            break;

        currentTexCoords -= deltaTexCoords;
        depthMapValue = materialHeight.has_texture_height_map
            ? texture(materialHeight.texture_height, currentTexCoords).r
            : 0.0;

        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}

// ----------------------------------------------------------------------------
void main()
{		
    // --------------------------------------------------------
    // 1. Base inputs
    // --------------------------------------------------------
    vec3 N = normalize(Normal);
    vec3 T = normalize(Tangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));

    // --------------------------------------------------------
    // 2. TBN construction (CORRECT, final stage)
    // --------------------------------------------------------
    mat3 TBN = mat3(T, B, N);

    // --------------------------------------------------------
    // 3. World space directions
    // --------------------------------------------------------
    vec3 lightDirWS = normalize(lightPos - FragPos);
    vec3 viewDirWS  = normalize(viewPos - FragPos);

    // --------------------------------------------------------
    // 4. Tangent space directions
    // --------------------------------------------------------
    // If you want tangent-space vectors, use transpose(TBN), not TBN
    vec3 lightDirTS = normalize(transpose(TBN) * lightDirWS);
    vec3 viewDirTS  = normalize(transpose(TBN) * viewDirWS);

    // --------------------------------------------------------
    // 5. Normal mapping
    // --------------------------------------------------------
    vec3 finalNormal;

    if (material.has_texture_normal_map && hasTangents)
    {
        vec3 normalSample = texture(material.texture_normal, TexCoords).rgb;
        normalSample = normalize(normalSample * 2.0 - 1.0);
        finalNormal = normalize(mix(N, normalize(TBN * normalSample), material.normalMapIntensity));
    }
    else
    {
        finalNormal = N;
    }




    //float dotNV = clamp(dot(N, viewDirWS), 0.0f, 1.0f);
    float dotNV = clamp(dot(finalNormal, viewDirWS), 0.0f, 1.0f);

    // --------------------------------------------------------
    // 6. Parallax mapping
    // --------------------------------------------------------
    vec2 texCoords = material.useParallaxMapping ? SteepParallaxMapping(TexCoords, viewDirTS) : TexCoords;



    // material properties
    vec4 diffuseSample = material.has_texture_diffuse_map ? texture(material.texture_diffuse, texCoords) : vec4(0.5, 0.5, 0.5, 1.0);

    vec3 albedo   = diffuseSample.rgb;
    vec3 mDiffuse = diffuseSample.rgb;
    vec3 mSpecular = vec3(0.23f, 0.23f, 0.23f);
    float alpha   = diffuseSample.a;
    float metallic = 0;
    float roughness = 0;

    if (material.has_texture_metalness_from_combined_map)
    {
        // 1 single combined texture for metalness and roughtness
        vec4 metalRoughness = texture(material.texture_metalness_from_combined, texCoords);
        metallic = metalRoughness.b; // Extract metallic from Blue channel
        roughness = metalRoughness.g; // Extract roughness from Green channel
    }
    else
    {
        // 2 distinct textures for metalness and roughtness
        metallic = material.has_texture_metalness_map ? texture(material.texture_metalness, texCoords).r : 0.0;
        roughness = material.has_texture_roughness_map ? texture(material.texture_roughness, texCoords).r : 0.5;
    }

    float ao = material.has_texture_ao_map ? texture(material.texture_ao, texCoords).r : 1.0;
    vec3 emissive = material.has_texture_emissive_map ? texture(material.texture_emissive, texCoords).rgb * material.emissiveIntensity : vec3(0.0);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(finalNormal, viewDirWS), 0.0), F0, roughness);
    
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

    vec3 irradiance = texture(material.texture_irradiance, finalNormal).rgb;
    vec3 diffuse = irradiance * albedo * material.iblDiffuseIntensity; // Apply iblDiffuseIntensity

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 R = reflect(-viewDirWS, finalNormal);
    vec3 prefilteredColor = textureLod(material.texture_prefilter, R, roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(material.texture_brdfLUT, vec2(max(dot(finalNormal, viewDirWS), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y) * material.iblSpecularIntensity; // Apply iblSpecularIntensity
    vec3 ambient = (kD * diffuse + specular) * ao * material.ambient_color * material.ambient_intensity;


    // lights
    for (int i = 0; i < pointLightsCount; i++)
    {
        if (pointLights[i].use)
            Lo += CalcPointLight(pointLights[i], finalNormal, N, FragPos, texCoords, viewDirWS, albedo, metallic, roughness);
    }

    for (int i = 0; i < dirLightsCount; i++)
    {
        if (dirLights[i].use)
            Lo += CalcDirLight(dirLights[i], finalNormal, N, FragPos, texCoords, viewDirWS, vec3(1.0));
    }

    for (int i = 0; i < spotLightsCount; i++)
    {
        if (spotLights[i].use)
            Lo += CalcSpotLight(spotLights[i], finalNormal, N, FragPos, texCoords, viewDirWS, albedo, metallic, roughness);
    }

    if (areaLightsCount > 0)
    {
        vec2 uv = vec2(material.albedoRoughness.w, sqrt(1.0f - dotNV));
        uv = uv * LUT_SCALE + LUT_BIAS;

        // get 4 parameters for inverse_M
        vec4 t1 = texture(LTC1, uv);

        // Get 2 parameters for Fresnel calculation
        vec4 t2 = texture(LTC2, uv);

        mat3 Minv = mat3(
            vec3(t1.x, 0, t1.y),
            vec3(   0, 1,    0),
            vec3(t1.z, 0, t1.w)
        );

        vec3 N = normalize(Normal);
        vec3 V = normalize(viewPos - FragPos); // View direction
        vec3 P = FragPos; // to remove !

        for (int i = 0; i < areaLightsCount; i++)
        {
            if (areaLights[i].use)
                Lo += CalcAreaLight(areaLights[i], finalNormal, N, V, P, Minv, t1, t2, mDiffuse, mSpecular);
        }
    }

    // add light and shadow contribution
    vec3 color = ambient + Lo;

    // Add emissive contribution before gamma correction
    color += emissive;


    // HDR tonemapping
    //color = color / (color + vec3(1.0));

    // gamma correction
    //color = pow(color, vec3(1.0/2.2));
    //color = vec3(ToSRGB(color)); // same as above


    FragColor = vec4(color, alpha);

    // Discard transparent fragments (optional)
    if (alpha < 0.1)
        discard;
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 geoNormal, vec3 fragPos, vec2 texCoords, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
    vec3 L = normalize(light.position - fragPos);
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;

    // Spotlight intensity based on angle between light direction and fragment (smooth blurry cutoff)
    float intensity = pow(smoothstep(light.outerCutOff, light.cutOff, theta), 2.0) * light.intensity;

    if (theta < light.outerCutOff)
        return vec3(0.0);
    
    vec3 H = normalize(viewDir + L);

    float NdotL = 0.0;
    if (isTessellated)
        NdotL = max(dot(normalize(normal), normalize(L)), 0.0);
    else
        NdotL = max(dot(normalize(normal), normalize(light.direction)), 0.0);

    //float NdotL = max(dot(normalize(normal), normalize(L)), 0.0);

    
    // Compute ligh attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    //float attenuation = 1.0 / (1.0 + 0.01 * distance + 0.0002 * distance * distance); // for large terrains

    vec3 radiance = light.diffuse * intensity * attenuation;

    // Compute shadow factor
    float shadow = 0.0;
    if (material.shadowCalculationMethod == 1)
        shadow = enableShadows && material.canCastShadows && material.canReceiveShadows ? ShadowCalculationPCF(FragPosLightSpace, geoNormal, light.direction) : 0.0;
    else if (material.shadowCalculationMethod == 2)
        shadow = enableShadows && material.canCastShadows && material.canReceiveShadows ? ShadowCalculationSoft(FragPosLightSpace, geoNormal, light.direction) : 0.0;
    else if (material.shadowCalculationMethod == 3)
        shadow = enableShadows && material.canCastShadows && material.canReceiveShadows ? ShadowCalculationPCSS(FragPosLightSpace, geoNormal) : 0.0;

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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 geoNormal, vec3 fragPos, vec2 texCoords, vec3 viewDir, vec3 albedo, float metallic, float roughness)
{
    vec3 L = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 radiance = light.diffuse * light.intensity * attenuation;

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

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 geoNormal, vec3 fragPos, vec2 texCoords, vec3 viewDir, vec3 color)
{
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);
    
    float NdotL = max(dot(normal, L), 0.0);

    vec3 radiance = light.diffuse * light.intensity;

    // Shadow calculation
    float shadow = 0.0;
    if (material.shadowCalculationMethod == 1)
        shadow = enableShadows && material.canCastShadows && material.canReceiveShadows ? ShadowCalculationPCF(FragPosLightSpace, geoNormal, light.direction) : 0.0;
    else if (material.shadowCalculationMethod == 2)
        shadow = enableShadows && material.canCastShadows && material.canReceiveShadows ? ShadowCalculationSoft(FragPosLightSpace, geoNormal, light.direction) : 0.0;
    else if (material.shadowCalculationMethod == 3)
        shadow = enableShadows && material.canCastShadows && material.canReceiveShadows ? ShadowCalculationPCSS(FragPosLightSpace, geoNormal) : 0.0;


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

// Calculates the color when using an area light.
vec3 CalcAreaLight(AreaLight light, vec3 normal, vec3 N, vec3 V, vec3 P, mat3 Minv, vec4 t1, vec4 t2, vec3 mDiffuse, vec3 mSpecular)
{
    // Evaluate LTC shading
    vec3 diffuse = LTC_Evaluate(N, V, P, mat3(1), light.points, light.twoSided, mDiffuse);
    vec3 specular = LTC_Evaluate(N, V, P, Minv, light.points, light.twoSided, vec3(1.0));

    // GGX BRDF shadowing and Fresnel
    // t2.x: shadowedF90 (F90 normally it should be 1.0)
    // t2.y: Smith function for Geometric Attenuation Term, it is dot(V or L, H).
    specular *= mSpecular * t2.x + (1.0 - mSpecular) * t2.y;

    // Add contribution
    vec3 lighting = light.color * light.intensity * (specular + diffuse);

    return lighting;
}