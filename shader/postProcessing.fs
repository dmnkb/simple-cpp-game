#version 330 core

in vec2 v_UV;
out vec4 FragColor;

uniform sampler2D renderTargetTexture;

void main()
{
    FragColor = vec4(texture(renderTargetTexture, v_UV).rgb, 1.0);
}
