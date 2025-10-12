#version 410 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 vUV;

void main()
{
    vUV = aTexCoords;
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}
