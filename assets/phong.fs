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

// Function to debug shadow
vec3 debugShadow(int lightIndex, vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return vec3(0, 0, 0);
    }

    return texture(shadowMaps[lightIndex], projCoords.xy).rgb;
}

// 16-sample Poisson disk offsets
const vec2 poissonDisk[16] =
    vec2[](vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725), vec2(-0.094184101, -0.92938870),
           vec2(0.34495938, 0.29387760), vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
           vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379), vec2(0.44323325, -0.97511554),
           vec2(0.53742981, -0.47373420), vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
           vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437599), vec2(0.19984126, 0.78641367),
           vec2(0.14383161, -0.14100790));

// Poisson disk sampling for shadow filtering
float calculatePoissonShadow(int lightIndex, vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[lightIndex], 0);
    float radius = 1.0; // Adjust for blur strength

    for (int i = 0; i < 16; i++)
    {
        vec2 offset = poissonDisk[i] * texelSize * radius;
        float closestDepth = texture(shadowMaps[lightIndex], projCoords.xy + offset).r;
        shadow += (currentDepth > closestDepth) ? 1.0 : 0.0;
    }

    return shadow / 16.0; // Average the shadow samples
}

// Function to calculate the contribution of a point light
vec3 calculatePointLight(vec3 lightPos, vec3 fragPos, vec3 viewPos, vec3 normal, vec3 color)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    vec3 diffuse = color * diff;
    vec3 specular = color * spec * specularIntensity;

    return diffuse + specular;
}

// Function to calculate the contribution of a directional light
vec3 calculateDirectionalLight(vec3 lightDir, vec3 viewPos, vec3 fragPos, vec3 normal, vec3 color, float shadow)
{
    lightDir = normalize(-lightDir); // Directional lights have rotation as direction

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    vec3 diffuse = color * diff;
    vec3 specular = color * spec * specularIntensity;

    return (diffuse + specular) * shadow;
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
        float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

        vec3 diffuse = color * diff;
        vec3 specular = color * spec * specularIntensity;

        resultColor = (diffuse + specular) * intensity * shadow;
    }

    return resultColor;
}

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 ambient = vec3(0.2f, 0.25f, 0.3f);
    vec3 resultColor = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 lightPos = lights[i].position;
        vec3 lightDir = lights[i].rotation;
        vec3 color = lights[i].color;

        vec4 fragPosLightSpace = lightSpaceMatrices[i] * vec4(FragPos, 1.0);
        float shadow = 1 - calculatePoissonShadow(i, fragPosLightSpace);

        if (lights[i].lightType == ELT_POINT)
        {
            resultColor += calculatePointLight(lightPos, FragPos, viewPos, norm, color);
        }
        else if (lights[i].lightType == ELT_DIRECTIONAL)
        {
            resultColor += calculateDirectionalLight(lightDir, viewPos, FragPos, norm, color, shadow);
        }
        else if (lights[i].lightType == ELT_SPOT)
        {
            resultColor += calculateSpotLight(lightPos, lightDir, FragPos, viewPos, norm, color, lights[i].innerCone,
                                              lights[i].outerCone, shadow);
        }

        // resultColor += debugShadow(i, fragPosLightSpace);
    }

    vec2 scaledUV = v_UV * textureRepeat;
    vec4 baseColor = texture(diffuseMap, scaledUV);
    FragColor = vec4(resultColor + ambient, 1.0) * baseColor;
}
