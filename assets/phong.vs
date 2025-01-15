#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in mat4 a_InstanceTransform;

uniform mat4 u_ViewProjection;

out vec2 v_UV;
out vec3 FragPos;
out vec3 v_Normal;
out float v_TextureID;

void main()
{
    v_UV = a_UV;

    v_Normal = mat3(a_InstanceTransform) * a_Normal;

    // Calculate the fragment position in world space
    FragPos = vec3(a_InstanceTransform * vec4(a_Position, 1.0));
    gl_Position = u_ViewProjection * vec4(FragPos, 1.0);
}
