#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in mat4 aInstanceTransform;

uniform mat4 uViewProjection; // lookAt(lightPos, lightPos + faceDir, faceUp)
uniform mat4 u_Proj;          // perspective(90°, 1, near, far)

out vec3 vWorldPos;

void main()
{
    vec4 worldPos = aInstanceTransform * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;                          // WORLD space for distance
    gl_Position = u_Proj * uViewProjection * worldPos; // rasterize with face VP
}
