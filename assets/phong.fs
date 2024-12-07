#version 330 core

struct Light
{
    vec3 position;
    vec3 color;
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

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 ambient = vec3(0.1, 0.1, 0.1);
    vec3 resultColor = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 100);

        vec3 diffuse = lights[i].color * diff;
        vec3 specular = lights[i].color * spec;

        resultColor += diffuse + specular;
    }

    int index = int(v_TextureID);
    vec4 texColor = texture(u_Textures[index], v_UV);
    FragColor = texColor * vec4(resultColor + ambient, 1.0);
}