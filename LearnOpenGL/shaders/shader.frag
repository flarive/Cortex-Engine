#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord; // coming from vertex shader
in vec3 Normal; // coming from vertex shader
in vec3 FragPos; // coming from vertex shader

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 viewPos;

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

    //FragColor = vec4(lightColor * objectColor, 1.0);





    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;


    // specular
    float specularStrength = 0.5;
    float shininess = 256.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;  



    // phong combining ambient+diffuse+specular
    vec3 result = (ambient + diffuse + specular) * objectColor;



    FragColor = vec4(result, 1.0);
}