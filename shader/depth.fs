#version 330 core

in vec2 v_UV;

uniform sampler2D diffuseMap;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec4 baseColor = texture(diffuseMap, v_UV);

    if (baseColor.a < .5)
    {
        discard;
    }

    float depth = gl_FragCoord.z;

    // Linear depth
    const float nearPlane = 0.1f;
    const float farPlane = 1000.0f; // (Not accurate, but clamps the colors to something more visible)
    float linearDepth = (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
    linearDepth = linearDepth / farPlane;

    FragColor = vec4(baseColor);
}
