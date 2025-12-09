#version 410 core
in vec3 vWorldPos;

uniform vec3 uLightPosWS; // point light position in WORLD space
uniform float uShadowFar; // same value you put into p_ranges[i].x

void main()
{
    float dist = length(vWorldPos - uLightPosWS);
    gl_FragDepth = clamp(dist / uShadowFar, 0.0, 1.0);
}
