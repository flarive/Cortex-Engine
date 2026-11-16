layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 Normal;
layout(location = 1) in vec3 FragPos;
layout(location = 2) in vec2 TexCoords;


#ifndef OPENGL
#error OPENGL_NOT_DEFINED
#endif


#ifdef OPENGL
layout(std140, binding = 2) uniform PhongUBO
#else
layout(set = 0, binding = 2) uniform PhongUBO
#endif
{
    vec4 lightPos;
    vec4 viewPos;
    vec4 lightColor;
    vec4 objectColor;

    uint hasDiffuseTexture;
} phong;

#ifdef OPENGL
uniform sampler2D texture_diffuse;
#else
layout(set = 0, binding = 3) uniform sampler2D texture_diffuse;
#endif



void main()
{
    vec3 baseColor = phong.objectColor.xyz;

    if (phong.hasDiffuseTexture != 0)
        baseColor *= texture(texture_diffuse, TexCoords).rgb;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * phong.lightColor.xyz;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(phong.lightPos.xyz - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * phong.lightColor.xyz;

    float specularStrength = 0.5;

    vec3 viewDir = normalize(phong.viewPos.xyz - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 specular = specularStrength * spec * phong.lightColor.xyz;

    vec3 result = (ambient + diffuse) * baseColor + specular;

    FragColor = vec4(result, 1.0);
}