#version 330 core
layout(location = 0) in vec3 a_Position;     // Vertex position
layout(location = 1) in vec2 a_UV;           // Texture coordinates
layout(location = 2) in vec3 a_Normal;       // Vertex normal
layout(location = 3) in vec4 a_InstancePos;  // First column of mat4
layout(location = 4) in vec4 a_InstanceCol2; // Second column of mat4
layout(location = 5) in vec4 a_InstanceCol3; // Third column of mat4
layout(location = 6) in vec4 a_InstanceCol4; // Fourth column of mat4

uniform mat4 u_ViewProjection; // View + projection matrix

out vec2 v_UV;
out vec3 FragPos;
out vec3 v_Normal;

void main()
{
    // Combine the instance matrix (4 columns)
    mat4 instanceTransform = mat4(a_InstancePos, a_InstanceCol2, a_InstanceCol3, a_InstanceCol4);

    // Apply the instance transformation to the vertex position
    vec4 worldPos = instanceTransform * vec4(a_Position, 1.0);

    // Pass data to the fragment shader
    v_UV = a_UV;
    v_Normal = normalize(mat3(instanceTransform) * a_Normal); // Transform normal with instance matrix (rotation part)
    FragPos = worldPos.xyz;                                   // Position in world space

    // Final transformation (view + projection)
    gl_Position = u_ViewProjection * worldPos;
}
