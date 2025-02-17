#version 330 core

out vec4 FragColor; // Output to the color attachment (debug)

void main()
{
    const float nearPlane = 0.1f;
    const float farPlane = 50.0f; // (Not accurate, but clamps the colors to something more visible)
    // Depth in [0, 1] range from the depth buffer
    float depth = gl_FragCoord.z;
    // Linearize depth using near and far planes
    float linearDepth = (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));
    // Scale linear depth to [0, 1] for visualization
    linearDepth = linearDepth / farPlane;
    // Output linear depth to the red channel
    FragColor = vec4(vec3(linearDepth), 1.0);
}
