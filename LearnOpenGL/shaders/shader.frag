#version 330 core

out vec4 FragColor;
  

in vec3 FragPos; // coming from vertex shader
in vec3 Normal; // coming from vertex shader
in vec2 TexCoords; // coming from vertex shader

//uniform sampler2D texture1;
//uniform sampler2D texture2;

//uniform vec3 lightColor;
//uniform vec3 lightPos;

uniform vec3 viewPos;


struct Material {
    vec3 ambient;
    //vec3 diffuse;
    sampler2D diffuse;
    //vec3 specular;
    sampler2D specular;
    float shininess;
};

struct Light {
    //vec3 position; // no longer necessary when using directional lights.

    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform Material material;
uniform Light light;


void main()
{
    // diffuse 
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(light.position - FragPos);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
 
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));  
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    FragColor = vec4(ambient + diffuse + specular, 1.0);   

}