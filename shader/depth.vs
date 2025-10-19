#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in mat4 aInstanceTransform;

uniform mat4 lightSpaceMatrix;

out vec3 vFragPos;
out vec2 vUV;

void main()
{
    vUV = aUV;

    vFragPos = vec3(aInstanceTransform * vec4(aPosition, 1.0));
    gl_Position = lightSpaceMatrix * vec4(vFragPos, 1.0);
}