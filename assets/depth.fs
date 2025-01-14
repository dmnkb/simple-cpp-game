#version 330 core

// Output of the fragment shader
out float FragDepth;

// Depth value from the vertex shader
void main()
{
    const float nearPlane = 0.1f;
    const float farPlane = 100.0f;

    // Depth in [0, 1] range from the depth buffer
    float depth = gl_FragCoord.z;

    // Linearize depth using near and far planes
    float linearDepth = (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - depth * (farPlane - nearPlane));

    // Scale linear depth to [0, 1] for visualization
    linearDepth = linearDepth / farPlane;

    // Output linear depth to the red channel
    // gl_FragCoord.z contains the depth in clip space [0, 1]
    FragDepth = linearDepth;
}
