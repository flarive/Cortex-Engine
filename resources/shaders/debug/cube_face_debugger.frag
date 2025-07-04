#version 330 core

out vec4 FragColor;

uniform samplerCube uCubeMap;
uniform int uFaceIndex; // 0 to 5

vec3 getDirection(int faceIndex, vec2 uv) {
    uv = uv * 2.0 - 1.0; // Map from [0,1] to [-1,1]
    if (faceIndex == 0) return normalize(vec3( 1.0, -uv.y, -uv.x)); // +X
    if (faceIndex == 1) return normalize(vec3(-1.0, -uv.y,  uv.x)); // -X
    if (faceIndex == 2) return normalize(vec3( uv.x,  1.0,  uv.y)); // +Y
    if (faceIndex == 3) return normalize(vec3( uv.x, -1.0, -uv.y)); // -Y
    if (faceIndex == 4) return normalize(vec3( uv.x, -uv.y,  1.0)); // +Z
    if (faceIndex == 5) return normalize(vec3(-uv.x, -uv.y, -1.0)); // -Z
    return vec3(0.0);
}

in vec2 vUV; // Passed from vertex shader (range [0,1])

void main() {
    vec3 dir = getDirection(uFaceIndex, vUV);
    vec3 color = texture(uCubeMap, dir).rgb;
    FragColor = vec4(color, 1.0);
}
