#version 330 core

// Output of the fragment shader
out float FragDepth;

// Depth value from the vertex shader
void main()
{
    // gl_FragCoord.z contains the depth in clip space [0, 1]
    FragDepth = gl_FragCoord.z;
}
