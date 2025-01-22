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
uniform float colorMapScale;

uniform sampler2D colorMap;
uniform sampler2D shadowMaps[7];
uniform mat4 lightSpaceMatrices[8];

out vec4 FragColor;

#define NUM_LIGHTS 4
layout(std140) uniform LightsBlock
{
    Light lights[NUM_LIGHTS];
};

// Function to calculate shadow factor
float calculateShadow(int lightIndex, vec4 fragPosLightSpace)
{
    // Perform perspective divide to get normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0, 1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Check if the fragment is outside the light's frustum
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return 0.0; // Fully lit if outside
    }

    // Sample the shadow map
    float closestDepth = texture(shadowMaps[lightIndex], projCoords.xy).r;

    // Compare depth values
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = (currentDepth - 0.0001) > closestDepth ? 1.0 : 0.0;

    return shadow;
}

// Function to calculate the contribution of a point light
vec3 calculatePointLight(vec3 lightPos, vec3 fragPos, vec3 viewPos, vec3 normal, vec3 color)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 100);

    vec3 diffuse = color * diff;
    vec3 specular = color * spec;

    return diffuse + specular;
}

// Function to calculate the contribution of a spot light
vec3 calculateSpotLight(vec3 lightPos, vec3 lightDir, vec3 fragPos, vec3 viewPos, vec3 normal, vec3 color,
                        float innerCone, float outerCone, float shadow)
{
    vec3 resultColor = vec3(0.0);

    vec3 fragToLight = normalize(lightPos - fragPos);
    float theta = dot(fragToLight, normalize(-lightDir));

    float cosInnerCone = cos(radians(innerCone));
    float cosOuterCone = cos(radians(outerCone));

    float epsilon = cosInnerCone - cosOuterCone;
    float intensity = clamp((theta - cosOuterCone) / epsilon, 0.0, 1.0);

    if (intensity > 0.0)
    {
        float diff = max(dot(normal, fragToLight), 0.0);

        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 halfwayDir = normalize(fragToLight + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 100);

        vec3 diffuse = color * diff;
        vec3 specular = color * spec;

        resultColor = (diffuse + specular) * intensity * shadow;
    }

    return resultColor;
}

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 ambient = vec3(0.75);
    vec3 resultColor = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 lightPos = lights[i].position;
        vec3 color = lights[i].color;

        // Transform fragment position into light space
        vec4 fragPosLightSpace = lightSpaceMatrices[i] * vec4(FragPos, 1.0);

        // Calculate shadow factor
        float shadow = 1 - calculateShadow(i, fragPosLightSpace);

        if (lights[i].lightType == ELT_POINT)
        {
            resultColor += calculatePointLight(lightPos, FragPos, viewPos, norm, color);
        }
        else if (lights[i].lightType == ELT_SPOT)
        {
            vec3 lightDir = lights[i].rotation;
            resultColor += calculateSpotLight(lightPos, lightDir, FragPos, viewPos, norm, color, lights[i].innerCone,
                                              lights[i].outerCone, shadow);
        }
    }

    vec4 baseColor = texture(colorMap, v_UV * colorMapScale);
    FragColor = vec4(resultColor + ambient, 1.0) * baseColor;
}
