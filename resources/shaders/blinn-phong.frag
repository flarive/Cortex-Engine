#version 330 core

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

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

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

    vec4 albedoRoughness; // (x,y,z) = color, w = roughness (for area light only)
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

struct AreaLight
{
    bool use;

    float intensity;
	vec3 color;
    vec3 points[4];
	bool twoSided;
};




// coming from vertex shader
in VS_OUT {
    vec3 FragPos; // same as worldPosition
    vec3 Normal; // same as worldNormal
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;

    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

// coming from code
uniform vec3 viewPos;
uniform float far_plane;
uniform bool enableShadows;
uniform bool hasTangents; // does the primitive to render has tangents and bitangents ?
uniform Material material;
uniform mat4 lightSpaceMatrix;

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


// area light only
uniform sampler2D LTC1; // 20 (for inverse M)
uniform sampler2D LTC2; // 21 (GGX norm, fresnel, 0(unused), sphere)

const float LUT_SIZE  = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;


// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcAreaLight(AreaLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 N, vec3 V, vec3 P, mat3 Minv, vec4 t1, vec4 t2, vec3 mDiffuse, vec3 mSpecular);




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

// array of offset direction for sampling
vec3 gridSamplingDiskOld[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);



// Poisson disk (example with 20 samples)
vec3 gridSamplingDisk[19] = vec3[]
(
    vec3(0.1379, 0.2486, 0.4430),
    vec3(0.3371, 0.5679, -0.0057),
    vec3(-0.6999, -0.0451, -0.0019),
    vec3(0.0689, -0.1598, -0.8547),
    vec3(0.0560, 0.0069, -0.1843),
    vec3(-0.0146, 0.1402, 0.0762),
    vec3(0.0100, -0.1924, -0.0344),
    vec3(-0.3577, -0.5301, -0.4358),
    vec3(-0.3169, 0.1063, 0.0158),
    vec3(0.0103, -0.5869, 0.0046),
    vec3(-0.0897, -0.4940, 0.3287),
    vec3(0.7119, -0.0154, -0.0918),
    vec3(-0.0533, 0.0596, -0.5411),
    vec3(0.0352, -0.0631, 0.5460),
    vec3(-0.4776, 0.2847, -0.0271),
    vec3(-0.0631, 0.5460, 0.0352),
    vec3(0.2847, -0.0271, -0.4776),
    vec3(0.0596, -0.5411, -0.0533),
    vec3(-0.5869, 0.0046, 0.0103)
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


vec2 rand(vec2 co)
{
    return fract(sin(vec2(dot(co, vec2(12.9898, 78.233)), dot(co, vec2(39.3468, 11.135)))) * 43758.5453);
}


float ShadowCalculationFaster(vec4 fragPosLightSpace, vec3 lightPos)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(material.texture_shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    //float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float bias = clamp(0.0005 * tan(acos(dot(normal, lightDir))), 0.0001, 0.01);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF (shadow anti aliasing))
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(material.texture_shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(material.texture_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


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


float ShadowCalculationCubeMap(vec3 fragPos, vec3 lightPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;

    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(material.texture_shadowMapCube, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    float bias = 0.001;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;


     
     // PCF
//     float shadow = 0.0;
//     float bias = 0.05; 
//     float samples = 4.0;
//     float offset = 0.1;
//     for(float x = -offset; x < offset; x += offset / (samples * 0.5))
//     {
//         for(float y = -offset; y < offset; y += offset / (samples * 0.5))
//         {
//             for(float z = -offset; z < offset; z += offset / (samples * 0.5))
//             {
//                 float closestDepth = texture(material.texture_shadowMapCube, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
//                 closestDepth *= far_plane;   // Undo mapping [0;1]
//                 if(currentDepth - bias > closestDepth)
//                     shadow += 1.0;
//             }
//         }
//     }
//     shadow /= (samples * samples * samples);


//    float shadow = 0.0;
//    float bias = 0.15;
//    int samples = 20;
//    float viewDistance = length(viewPos - fragPos);
//    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
//    for(int i = 0; i < samples; ++i)
//    {
//        float closestDepth = texture(material.texture_shadowMapCube, fragToLight + gridSamplingDisk[i] * diskRadius).r;
//        closestDepth *= far_plane;   // undo mapping [0;1]
//        if(currentDepth - bias > closestDepth)
//            shadow += 1.0;
//    }
//    shadow /= float(samples);


//    float shadow = 0.0;
//    float bias = max(0.05 * (1.0 - dot(fs_in.Normal, normalize(lightPos - fs_in.FragPos))), 0.005); // angle-dependent bias
//    int samples = 20;
//    float viewDistance = length(viewPos - fragPos);
//    float diskRadius = (1.0 + (viewDistance / far_plane)) / 30.0; // slightly smaller radius
//
//    for (int i = 0; i < samples; ++i)
//    {
//        vec3 sampleOffset = gridSamplingDisk[i] * diskRadius;
//        float closestDepth = texture(material.texture_shadowMapCube, fragToLight + sampleOffset).r;
//        closestDepth *= far_plane; // undo [0,1] mapping
//
//        if (currentDepth - bias > closestDepth)
//            shadow += 1.0;
//    }
//
//    shadow /= float(samples);



        
    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0); 
        
    return shadow;
}


float ShadowCalculationCubeMap2(vec3 fragPos, vec3 lightPos, vec3 normal, vec3 lightDir, vec2 screenSize)
{
    float shadow = 0.0;
	int samples = 19;

    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;

    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(material.texture_shadowMapCube, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);


	// Angle-dependent bias
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float bias = 0.001;

	// View-dependent disk radius
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 30.0;

	// Procedural noise for jitter
	vec2 noise = rand(gl_FragCoord.xy / screenSize);
	float jitterStrength = 0.1;

	for (int i = 0; i < samples; ++i)
	{
		vec3 offset = gridSamplingDisk[i];
		offset.xy += noise * jitterStrength;
		vec3 sampleDir = fragToLight + offset * diskRadius;

		float closestDepth = texture(material.texture_shadowMapCube, sampleDir).r;
		closestDepth *= far_plane;

		if (currentDepth - bias > closestDepth)
		shadow += 1.0;
	}

    shadow /= float(samples);
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

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================

    vec3 result = vec3(0.0);

    
	
    // Lighting calculations must be done in linear space for physical correctness.
    // Most image formats (like PNG, JPEG) store colors in sRGB, so you need to linearize them before use.
    //vec3 mDiffuse = ToLinear(texture(material.texture_diffuse, fs_in.TexCoords).rgb); // gamma correction
    //vec3 mSpecular = ToLinear(vec3(0.23, 0.23, 0.23)); // gamma correction
    
    vec3 mDiffuse = texture(material.texture_diffuse, fs_in.TexCoords).rgb;
    vec3 mSpecular = vec3(0.23, 0.23, 0.23); // ???????????


	vec3 N = normalize(fs_in.Normal);
	vec3 V = normalize(viewPos - fs_in.FragPos);
	vec3 P = fs_in.FragPos;
	float dotNV = clamp(dot(N, V), 0.0, 1.0);

    // use roughness and sqrt(1-cos_theta) to sample M_texture
    vec2 uv = vec2(material.albedoRoughness.w, sqrt(1.0 - dotNV));
    uv = uv * LUT_SCALE + LUT_BIAS;

    // get 4 parameters for inverse_M
    vec4 t1 = texture(LTC1, uv);

    // Get 2 parameters for Fresnel calculation
    vec4 t2 = texture(LTC2, uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    // Lighting
    for (int i = 0; i < pointLightsCount; i++)
    {
        if (pointLights[i].use)
            result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
    }

    for (int i = 0; i < dirLightsCount; i++)
    {
        if (dirLights[i].use)
            result += CalcDirLight(dirLights[i], norm, fs_in.FragPos, viewDir);
    }

    for (int i = 0; i < spotLightsCount; i++)
    {
        if (spotLights[i].use)
            result += CalcSpotLight(spotLights[i], norm, fs_in.FragPos, viewDir);
    }

    for (int i = 0; i < areaLightsCount; i++)
    {
        if (areaLights[i].use)
            result += CalcAreaLight(areaLights[i], norm, fs_in.FragPos, viewDir, N, V, P, Minv, t1, t2, mDiffuse, mSpecular);
    }

    // Sample the alpha value from the diffuse texture
    float alpha = texture(material.texture_diffuse, fs_in.TexCoords).a;

    //FragColor = vec4(ToSRGB(result), alpha);
    FragColor = vec4(result, alpha);

    // Discard transparent fragments (optional)
    if (alpha < 0.1)
        discard;
}




// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Light direction (inverted because light.direction points where the light is going)
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading (Blinn-Phong or Phong based on 'blinn' flag)
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Ambient, Diffuse, and Specular components
    float ambientStrength = 0.5;
    vec3 ambient = light.ambient * (material.has_texture_diffuse_map ? (mix(material.ambient_color, vec3(texture(material.texture_diffuse, fs_in.TexCoords)), ambientStrength)) : material.ambient_color);
    vec3 diffuse = light.diffuse * diff * (material.has_texture_diffuse_map ? (vec3(texture(material.texture_diffuse, fs_in.TexCoords)).rgb) : material.diffuse_color);
    vec3 specular = light.specular * spec * (material.has_texture_specular_map ? (vec3(texture(material.texture_specular, fs_in.TexCoords)).rgb) : material.specular_color);

    // Shadow Calculation (no light position needed)
    float shadow = enableShadows ? ShadowCalculationSlower(fs_in.FragPosLightSpace, lightDir) : 0.0;

    // Apply shadow intensity for darker/lighter shadows
    shadow = clamp(shadow * material.shadowIntensity, 0.0, 10.0);
    
    // Final lighting with shadow applied
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));// * color;

    return lighting;
}


// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Ambient, Diffuse, and Specular components
    float ambientStrength = 0.5;
    vec3 ambient = light.ambient * (material.has_texture_diffuse_map ? (mix(material.ambient_color, vec3(texture(material.texture_diffuse, fs_in.TexCoords)), ambientStrength)) : material.ambient_color);
    vec3 diffuse = light.diffuse * max(diff, 0.1) * (material.has_texture_diffuse_map ? vec3(texture(material.texture_diffuse, fs_in.TexCoords)) : material.diffuse_color);
    vec3 specular = light.specular * spec * (material.has_texture_specular_map ? (vec3(texture(material.texture_specular, fs_in.TexCoords)).rgb) : material.specular_color);

    float intensity = 1.0;
    // apply attenuation
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    // calculate shadow
    //vec2 screenSize = vec2(1280, 720);
    float shadow = enableShadows ? ShadowCalculationCubeMap(fragPos, light.position) : 0.0;
    //float shadow = shadows ? ShadowCalculationCubeMap2(fragPos, light.position, normal, lightDir, screenSize) : 0.0;

    // Apply shadow intensity for darker/lighter shadows
    shadow = clamp(shadow * material.shadowIntensity, 0.0, 10.0);
    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    return lighting;
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Direction from fragment to light
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, light.direction), 0.0);

    // Specular shading (Blinn-Phong or Phong based on 'blinn' flag)
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Attenuation based on distance
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    
    float theta = dot(lightDir, normalize(-light.direction));

    // Spotlight intensity based on angle between light direction and fragment (sharp cutoff)
    //float epsilon = light.cutOff - light.outerCutOff; // Smooth edge
    //float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Spotlight intensity based on angle between light direction and fragment (smooth blurry cutoff)
    float intensity = pow(smoothstep(light.outerCutOff, light.cutOff, theta), 2.0);

    // Ambient, Diffuse, and Specular components
    //vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, fs_in.TexCoords)).rgb;
    float ambientStrength = 0.5;
    vec3 ambient = light.ambient * (material.has_texture_diffuse_map ? (mix(material.ambient_color, vec3(texture(material.texture_diffuse, fs_in.TexCoords)), ambientStrength)) : material.ambient_color);
    vec3 diffuse = light.diffuse * diff * (material.has_texture_diffuse_map ? (vec3(texture(material.texture_diffuse, fs_in.TexCoords)).rgb) : material.diffuse_color);
    vec3 specular = light.specular * spec * (material.has_texture_specular_map ? (vec3(texture(material.texture_specular, fs_in.TexCoords)).rgb) : material.specular_color);

    // Apply attenuation and spotlight intensity
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    // Shadow calculation (using the light's position for shadow mapping)
    float shadow = enableShadows ? ShadowCalculationSlower(fs_in.FragPosLightSpace, light.position) : 0.0;
    
    // Apply shadow intensity for darker/lighter shadows
    shadow = clamp(shadow * material.shadowIntensity, 0.0, 10.0);

    // Final lighting with shadow applied
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    // debug shadows
    //FragColor = vec4(vec3(shadow), 1.0);

    return lighting;
}

// Calculates the color when using an area light.
vec3 CalcAreaLight(AreaLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 N, vec3 V, vec3 P, mat3 Minv, vec4 t1, vec4 t2, vec3 mDiffuse, vec3 mSpecular)
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