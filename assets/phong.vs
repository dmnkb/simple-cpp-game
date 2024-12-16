#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
out vec2 v_UV;
out vec3 FragPos;

void main()
{
    v_UV = a_UV;
    FragPos = a_Position;
    gl_Position = u_ViewProjection * u_Transform * vec4(FragPos, 1.0);
}