//#version 330 core
//
//in vec2 uv;
//out vec4 FragColor;
//
//uniform vec4 fillColor;
//uniform vec4 borderColor;
//uniform vec2 rectSize;   // rectangle size in pixels
//uniform float borderPx;  // e.g. 1.0
//
//void main()
//{
//    // Convert pixel border thickness to UV space per axis
//    float borderU = borderPx / rectSize.x;
//    float borderV = borderPx / rectSize.y;
//
//    bool isBorder =
//        uv.x < borderU || uv.x > 1.0 - borderU ||
//        uv.y < borderV || uv.y > 1.0 - borderV;
//
//    FragColor = isBorder ? borderColor : fillColor;
//}


#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform vec4 fillColor;
uniform vec4 borderColor;

uniform vec2  rectSize;   // rectangle size in pixels
uniform float radiusPx;   // corner radius in pixels
uniform float borderPx;   // border thickness in pixels

// Signed distance to a rounded rectangle centered at origin
float sdRoundRect(vec2 p, vec2 halfSize, float radius)
{
    vec2 q = abs(p) - (halfSize - vec2(radius));
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

void main()
{
    // Convert UV [0,1] => local pixel coordinates centered at (0,0)
    vec2 halfSize = rectSize * 0.5;
    vec2 p = (uv * rectSize) - halfSize;

    // Distance to outer rounded rect
    float distOuter = sdRoundRect(p, halfSize, radiusPx);

    // Distance to inner rounded rect (for border)
    float distInner = sdRoundRect(
        p,
        halfSize - vec2(borderPx),
        max(radiusPx - borderPx, 0.0)
    );

    // Anti-alias width (1 pixel)
    float aa = 1.0;

    float outerAlpha = 1.0 - smoothstep(0.0, aa, distOuter);
    float innerAlpha = 1.0 - smoothstep(0.0, aa, distInner);

    vec4 color = mix(borderColor, fillColor, innerAlpha);
    FragColor = color * outerAlpha;
}