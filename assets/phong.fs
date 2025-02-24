#version 330 core

in vec2 v_UV;
in vec3 v_Normal;
in vec3 FragPos;

uniform sampler2D diffuseMap;

out vec4 FragColor;

void main()
{
    FragColor = texture(diffuseMap, v_UV);
}
