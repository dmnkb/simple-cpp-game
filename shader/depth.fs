#version 410 core

in vec2 vUV;

uniform sampler2D uDiffuseMap;
uniform float uTextureRepeat;

const float ALPHA_CUTOFF = 0.5;

void main()
{
    vec4 tex = texture(uDiffuseMap, vUV * uTextureRepeat);
    if (tex.a < ALPHA_CUTOFF)
        discard;
}
