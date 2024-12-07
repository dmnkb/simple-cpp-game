#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in float a_TextureID;
uniform mat4 u_ViewProjection;
out vec2 v_UV;
out vec3 FragPos;
out vec3 v_Normal;
out float v_TextureID;
void main()
{
    v_UV = a_UV;
    v_Normal = normalize(a_Normal);
    v_TextureID = a_TextureID;
    FragPos = a_Position;
    gl_Position = u_ViewProjection * vec4(FragPos, 1.0);
}