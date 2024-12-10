#version 330 core

const int POINT_LIGHT = 0;
const int SPOT_LIGHT = 1;

struct Light
{
    vec3 position;
    vec3 color;
    vec3 rotation;
    int lightType;
    float innerCone;
    float outerCone;
};

out vec4 FragColor;
in vec2 v_UV;
in vec3 v_Normal;
in float v_TextureID;
in vec3 FragPos;
uniform sampler2D u_Textures[16];
uniform vec3 viewPos;

#define NUM_LIGHTS 256
layout(std140) uniform LightsBlock
{
    Light lights[NUM_LIGHTS];
};

vec3 calculatePointLight(vec3 lightPos, vec3 fragPos, vec3 viewPos, vec3 normal, vec3 color)
{
    vec3 resultColor = vec3(0.0);

    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 100);

    vec3 diffuse = color * diff;
    vec3 specular = color * spec;

    return diffuse + specular;
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
    }

    int index = int(v_TextureID);
    vec4 texColor = texture(u_Textures[index], v_UV);
    FragColor = texColor * vec4(resultColor + ambient, 1.0);
}