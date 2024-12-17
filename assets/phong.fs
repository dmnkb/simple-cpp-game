#version 330 core

const int POINT_LIGHT = 0;
const int SPOT_LIGHT = 1;

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

out vec4 FragColor;
in vec2 v_UV;
in vec3 v_Normal;
in vec3 FragPos;
uniform sampler2D u_Texture;
uniform vec3 viewPos;

#define NUM_LIGHTS 256
layout(std140) uniform LightsBlock
{
    Light lights[NUM_LIGHTS];
};

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
                        float innerCone, float outerCone)
{
    vec3 resultColor = vec3(0.0);

    // Compute the direction from the light to the fragment
    vec3 fragToLight = normalize(lightPos - fragPos);

    // Angle between the light's direction and the fragment's direction
    float theta = dot(fragToLight, normalize(-lightDir));

    // Compute spotlight intensity based on degree angles
    float cosInnerCone = cos(radians(innerCone)); // Convert inner cone angle to radians and take cosine
    float cosOuterCone = cos(radians(outerCone)); // Convert outer cone angle to radians and take cosine

    float epsilon = cosInnerCone - cosOuterCone; // Smooth edge transition
    float intensity = clamp((theta - cosOuterCone) / epsilon, 0.0, 1.0);

    if (intensity > 0.0)
    {
        float diff = max(dot(normal, fragToLight), 0.0);

        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 halfwayDir = normalize(fragToLight + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 100);

        vec3 diffuse = color * diff;
        vec3 specular = color * spec;

        resultColor = (diffuse + specular) * intensity;
    }

    return resultColor;
}

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 ambient = vec3(0.5);
    vec3 resultColor = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 lightPos = lights[i].position;
        vec3 color = lights[i].color;

        if (lights[i].lightType == POINT_LIGHT)
        {
            resultColor += calculatePointLight(lightPos, FragPos, viewPos, norm, color);
        }
        else if (lights[i].lightType == SPOT_LIGHT)
        {

            vec3 lightDir = lights[i].rotation;
            resultColor += calculateSpotLight(lightPos, lightDir, FragPos, viewPos, norm, color, lights[i].innerCone,
                                              lights[i].outerCone);
        }
    }

    vec4 texColor = texture(u_Texture, v_UV);
    FragColor = texColor * vec4(resultColor + ambient, 1.0);
}
