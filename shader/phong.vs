#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in mat4 aInstanceTransform;

uniform mat4 uTime;
uniform mat4 uViewProjection;

out vec2 vUV;
out vec3 vNormal;
out vec3 vFragPos;

void main()
{
    vUV = aUV;

    vNormal = mat3(aInstanceTransform) * aNormal;

    vFragPos = vec3(aInstanceTransform * vec4(aPosition, 1.0));
    gl_Position = uViewProjection * vec4(vFragPos, 1.0);
}
