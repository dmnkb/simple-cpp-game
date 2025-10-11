#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;
layout(location = 3) in mat4 a_InstanceTransform; // model

uniform mat4 u_View; // lookAt(lightPos, lightPos + faceDir, faceUp)
uniform mat4 u_Proj; // perspective(90°, 1, near, far)

out vec3 vWorldPos;

void main()
{
    vec4 worldPos = a_InstanceTransform * vec4(a_Position, 1.0);
    vWorldPos = worldPos.xyz;                 // WORLD space for distance
    gl_Position = u_Proj * u_View * worldPos; // rasterize with face VP
}
