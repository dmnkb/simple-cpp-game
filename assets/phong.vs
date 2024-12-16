#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in float a_TextureID;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
out vec2 v_UV;
out vec3 FragPos;
out vec3 v_Normal;
out float v_TextureID;
void main()
{
    v_UV = a_UV;
    v_Normal = mat3(u_Transform) * a_Normal; // Transform normal to world space
    v_TextureID = a_TextureID;

    // Transform position to world space and pass it to fragment shader
    FragPos = vec3(u_Transform * vec4(a_Position, 1.0));

    gl_Position = u_ViewProjection * vec4(FragPos, 1.0);
}
