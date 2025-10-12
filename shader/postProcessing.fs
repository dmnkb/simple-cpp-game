#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D renderTargetTexture;

void main()
{
    FragColor = vec4(texture(renderTargetTexture, vUV).rgb, 1.0);
}
