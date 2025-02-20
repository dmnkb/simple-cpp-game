#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;
layout(location = 3) in mat4 a_InstanceTransform;

uniform float u_Time;
uniform mat4 u_ViewProjection;

out vec2 v_UV;
out vec3 FragPos;
out vec3 v_Normal;

void main()
{
    v_UV = a_UV;
    float time = mod(u_Time, 6.283); // Loop at (2 * PI) / u_WindSpeed

    // Wind parameters
    const float u_WindStrength = 0.1;            // Controls intensity
    const float u_WindSpeed = 1.0;               // Controls speed
    const vec2 u_WindDirection = vec2(1.0, 1.5); // Direction (X,Z)

    // Transform normal to world space
    v_Normal = mat3(a_InstanceTransform) * a_Normal;

    // Wind effect based on world position
    float heightFactor = smoothstep(0.0, 1.0, a_Position.y); // Top moves more, bottom moves less
    float windPhase = dot(vec2(a_Position.x, a_Position.z), u_WindDirection) * 0.2 + time * u_WindSpeed;
    float windOffset = sin(windPhase) * u_WindStrength * heightFactor;

    // Apply wind sway effect (mostly affects the top parts)
    vec3 animatedPos = a_Position + vec3(windOffset * u_WindDirection.x, 0.0, windOffset * u_WindDirection.y);

    // Transform to world space
    FragPos = vec3(a_InstanceTransform * vec4(animatedPos, 1.0));

    // Final position in clip space
    gl_Position = u_ViewProjection * vec4(FragPos, 1.0);
}
