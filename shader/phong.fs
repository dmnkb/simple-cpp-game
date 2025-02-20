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

// Blocker Search - Finds average blocker depth
float findBlockerDepth(int lightIndex, vec2 uv, float zReceiver, int searchSamples)
{
    vec2 texelSize = 1.0 / textureSize(shadowMaps[lightIndex], 0);
    float blockerSum = 0.0;
    int blockers = 0;

    for (int x = -searchSamples; x <= searchSamples; x++)
    {
        for (int y = -searchSamples; y <= searchSamples; y++)
        {
            vec2 offset = vec2(x, y) * texelSize;
            float depth = texture(shadowMaps[lightIndex], uv + offset).r;
            if (depth < zReceiver)
            {
                blockerSum += depth;
                blockers++;
            }
        }
    }

    return (blockers > 0) ? (blockerSum / blockers) : 1.0;
}

// Percentage Closer Soft Shadows (PCSS)
float calculatePCSSShadow(int lightIndex, vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return 1.0;
    }

    vec2 texelSize = 1.0 / textureSize(shadowMaps[lightIndex], 0);
    float zReceiver = projCoords.z;

    // Step 1: Find blocker depth
    float avgBlockerDepth = findBlockerDepth(lightIndex, projCoords.xy, zReceiver, 3);
    if (avgBlockerDepth == 1.0)
        return 1.0;

    // Step 2: Compute penumbra size
    const float LIGHT_SIZE = 10;

    float penumbraSize = LIGHT_SIZE * (zReceiver - avgBlockerDepth) / avgBlockerDepth * 5;
    penumbraSize = clamp(penumbraSize, 1.0, 15.0);

    // Step 3: Percentage Closer Filtering (PCF) with variable kernel size
    float shadow = 0.0;
    int pcfSamples = int(penumbraSize);
    for (int x = -pcfSamples; x <= pcfSamples; x++)
    {
        for (int y = -pcfSamples; y <= pcfSamples; y++)
        {
            vec2 offset = vec2(x, y) * texelSize;
            float closestDepth = texture(shadowMaps[lightIndex], projCoords.xy + offset).r;
            shadow += (zReceiver > closestDepth) ? 1.0 : 0.0;
        }
    }

    return 1.0 - (shadow / float((pcfSamples * 2 + 1) * (pcfSamples * 2 + 1)));
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

// Referencing: https://www.emutalk.net/threads/emulating-nintendo-64-3-sample-bilinear-filtering-using-shaders.54215/
vec4 N64_3Point_Filter(sampler2D tex, vec2 uv, vec2 texSize)
{
    vec2 texelSize = 1.0 / texSize;
    vec2 half_tex = texelSize * 0.5;
    vec2 UVCentered = uv - half_tex;

    vec2 pixelPos = UVCentered * texSize;
    vec2 texelPos = floor(pixelPos);
    vec2 f = fract(pixelPos);

    if (pixelPos.x < 0)
        f.x = 1 - f.x;
    if (pixelPos.y < 0)
        f.y = 1 - f.y;

    vec2 uv00 = (texelPos + vec2(0.0, 0.0)) / texSize;
    vec2 uv10 = (texelPos + vec2(1.0, 0.0)) / texSize;
    vec2 uv01 = (texelPos + vec2(0.0, 1.0)) / texSize;
    vec2 uv11 = (texelPos + vec2(1.0, 1.0)) / texSize;

    vec4 t00 = texture(tex, uv00);
    vec4 t10 = texture(tex, uv10);
    vec4 t01 = texture(tex, uv01);
    vec4 t11 = texture(tex, uv11);

    vec4 diffuseColor = t00 + f.x * (t10 - t00) + f.y * (t01 - t00);
    diffuseColor *= (1.0 - step(1.0, f.x + f.y));
    diffuseColor += (t11 + (1.0 - f.x) * (t01 - t11) + (1.0 - f.y) * (t10 - t11)) * step(1.0, f.x + f.y);

    return diffuseColor;
}

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 ambient = vec3(0.2f, 0.25f, 0.3f);
    vec3 resultColor = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 lightPos = lights[i].position;
        vec3 lightDir = normalize(lights[i].rotation);
        vec3 color = lights[i].color;

        vec4 fragPosLightSpace = lightSpaceMatrices[i] * vec4(FragPos, 1.0);
        float shadow = calculatePCSSShadow(i, fragPosLightSpace);

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
    vec2 texSize = vec2(textureSize(diffuseMap, 0));

    vec4 filteredColor = N64_3Point_Filter(diffuseMap, scaledUV, texSize);
    vec4 baseColor = texture(diffuseMap, scaledUV);

    vec4 finalColor = vec4(resultColor + ambient, 1.0) * filteredColor;
    if (finalColor.a < .5)
    {
        discard;
    }

    FragColor = finalColor;
}