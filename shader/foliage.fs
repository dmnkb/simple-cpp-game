#version 330 core

const int ELT_POINT = 0;
const int ELT_DIRECTIONAL = 1;
const int ELT_SPOT = 2;

struct Light
{
    vec3 position;   // Offset 0, size 12
    float padding1;  // Offset 12, size 4
    vec3 color;      // Offset 16, size 12
    float padding2;  // Offset 28, size 4
    vec3 rotation;   // Offset 32, size 12
    float padding3;  // Offset 44, size 4
    int lightType;   // Offset 48, size 4
    float innerCone; // Offset 52, size 4
    float outerCone; // Offset 56, size 4
    float padding4;  // Offset 60, size 4
};                   // Total size: 64 bytes (aligned to 16 bytes)

in vec2 v_UV;
in vec3 v_Normal;
in vec3 FragPos;

uniform vec3 viewPos;

uniform sampler2D diffuseMap;
uniform sampler2D shadowMaps[7];
uniform mat4 lightSpaceMatrices[8];

out vec4 FragColor;

#define NUM_LIGHTS 4
layout(std140) uniform LightsBlock
{
    Light lights[NUM_LIGHTS];
};

layout(std140) uniform MaterialPropsBlock
{
    float textureRepeat;     // Controls UV scaling
    float shininess;         // Controls the sharpness of specular highlights
    float specularIntensity; // Controls the visibility of specular highlights
};

void main()
{

    vec2 scaledUV = v_UV * textureRepeat;
    vec2 texSize = vec2(textureSize(diffuseMap, 0));

    vec4 baseColor = texture(diffuseMap, scaledUV);

    vec4 finalColor = baseColor;
    if (finalColor.a < .5)
    {
        discard;
    }

    FragColor = finalColor;
}