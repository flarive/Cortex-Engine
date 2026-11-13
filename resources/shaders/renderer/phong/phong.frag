//#version 440 core
//
//layout(location = 0) in vec3 FragPos;
//layout(location = 1) in vec3 Normal;
//layout(location = 2) in vec2 TexCoords;
//
//
//layout(location = 0) out vec4 FragColor;
//  
//layout(set = 0, binding = 0) uniform PhongUBO
//{
//    vec3 lightPos;
//    vec3 viewPos;
//    vec3 lightColor;
//    vec3 objectColor;
//} ubo;
//
//void main()
//{
//    // ambient
//    float ambientStrength = 0.1;
//    vec3 ambient = ambientStrength * ubo.lightColor;
//  	
//    // diffuse 
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(ubo.lightPos - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * ubo.lightColor;
//    
//    // specular
//    float specularStrength = 0.5;
//    vec3 viewDir = normalize(ubo.viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);  
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = specularStrength * spec * ubo.lightColor;  
//        
//    vec3 result = (ambient + diffuse + specular) * ubo.objectColor;
//    FragColor = vec4(result, 1.0);
//}

#version 450

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 Normal;
layout(location = 1) in vec3 FragPos;
layout(location = 2) in vec2 TexCoords;

#ifdef VULKAN
// use Uniform Buffer Object for Vulkan
layout(set = 0, binding = 1) uniform PhongUBO
{
    vec3 lightPos;
    vec3 viewPos;
    vec3 lightColor;

    vec3 objectColor;
    bool hasDiffuseTexture;
} material;

layout(set = 0, binding = 2) uniform sampler2D texture_diffuse;

#define LIGHT_POS   material.lightPos
#define VIEW_POS    material.viewPos
#define LIGHT_COLOR material.lightColor
#define OBJECT_COLOR material.objectColor
#define HAS_TEXTURE material.hasDiffuseTexture

#else
// use simple uniforms for OpenGL
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform sampler2D texture_diffuse;
uniform bool hasDiffuseTexture;

uniform vec3 objectColor;

#define LIGHT_POS    lightPos
#define VIEW_POS     viewPos
#define LIGHT_COLOR  lightColor
#define OBJECT_COLOR objectColor
#define HAS_TEXTURE  hasDiffuseTexture

#endif

void main()
{
    vec3 baseColor = OBJECT_COLOR;

    if (HAS_TEXTURE)
        baseColor *= texture(texture_diffuse, TexCoords).rgb;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LIGHT_COLOR;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LIGHT_POS - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LIGHT_COLOR;

    float specularStrength = 0.5;

    vec3 viewDir = normalize(VIEW_POS - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 specular = specularStrength * spec * LIGHT_COLOR;

    vec3 result =
        (ambient + diffuse) * baseColor +
        specular;

    FragColor = vec4(result, 1.0);
}