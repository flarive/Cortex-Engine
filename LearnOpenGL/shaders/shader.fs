#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord; // coming from vertex shader

uniform sampler2D texture1;
uniform sampler2D texture2;

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
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}