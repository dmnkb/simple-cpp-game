#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;
layout(location = 3) in mat4 a_InstanceTransform;

uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec2 v_UV;

void main()
{
    v_UV = a_UV;

    FragPos = vec3(a_InstanceTransform * vec4(a_Position, 1.0));
    gl_Position = lightSpaceMatrix * vec4(FragPos, 1.0);
}