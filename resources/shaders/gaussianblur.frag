#version 460 core
out float FragColor;

in vec2 TexCoords;
uniform sampler2D image;
uniform bool horizontal;
uniform float weight[5] = float[](0.227027, 0.316216, 0.070270, 0.016216, 0.004229);

void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0); // pixel size
    float result = texture(image, TexCoords).r * weight[0];
    for(int i = 1; i < 5; ++i) {
        if(horizontal) {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).r * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).r * weight[i];
        } else {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).r * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).r * weight[i];
        }
    }
    //FragColor = result;

    FragColor = 0.0;
}
