#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform vec4 fillColor;
uniform vec4 borderColor;
uniform float borderThickness;

void main()
{
    // uv in [0,1]
    float left   = uv.x;
    float right  = 1.0 - uv.x;
    float bottom = uv.y;
    float top    = 1.0 - uv.y;

    float edgeDist = min(min(left, right), min(top, bottom));

    if (edgeDist < borderThickness)
        FragColor = borderColor;
    else
        FragColor = fillColor;
}