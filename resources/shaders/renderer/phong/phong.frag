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

#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoords;
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;


uniform sampler2D texture_diffuse;
uniform bool hasDiffuseTexture;

uniform vec3 objectColor;

void main()
{
    // base material color (Albedo)
    vec3 baseColor = objectColor;

    if (hasDiffuseTexture)
        baseColor *= texture(texture_diffuse, TexCoords).rgb;

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    // Final result
    vec3 result = (ambient + diffuse) * baseColor + specular;

    FragColor = vec4(result, 1.0);
} 