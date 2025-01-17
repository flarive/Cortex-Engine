#version 330 core

out vec4 FragColor;
  

in vec3 FragPos; // coming from vertex shader
in vec3 Normal; // coming from vertex shader
in vec2 TexCoords; // coming from vertex shader

uniform sampler2D texture1;
uniform sampler2D texture2;

//uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 viewPos;


struct Material {
    vec3 ambient;
    //vec3 diffuse;
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform Material material;
uniform Light light;


void main()
{
    //FragColor = texture(ourTexture, TexCoord);
    //FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);


    //vec4 rgba_texture1 = texture(texture1, TexCoord);
    //vec4 rgba_texture2 = texture(texture2, TexCoord);

    //float mix_value = rgba_texture2.a * 0.2;

    //FragColor = mix( vec4(rgba_texture1.r, rgba_texture1.g, rgba_texture1.b, rgba_texture1.a), vec4(rgba_texture2.r, rgba_texture2.g, rgba_texture2.b, rgba_texture2.a), mix_value);

    // linearly interpolate between both textures (80% container, 20% awesomeface)
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);





    // ambient (low impact)
    //vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  



    // phong combining ambient+diffuse+specular
    vec3 result = ambient + diffuse + specular;



    FragColor = vec4(result, 1.0);
}