#version 330 core

in vec2 v_UV;
in vec3 v_Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D diffuseMap;

#define MAX_SHADOW_MAPS 7
uniform sampler2D shadowMaps[MAX_SHADOW_MAPS];
uniform mat4 lightSpaceMatrices[MAX_SHADOW_MAPS];

uniform int u_numLights;

#define MAX_SPOT_LIGHTS 16

layout(std140) uniform SpotLights
{
    vec4 positionsWS[MAX_SPOT_LIGHTS];     // xyz pos, w=1
    vec4 directionsWS[MAX_SPOT_LIGHTS];    // xyz dir (norm), w=0
    vec4 colorsIntensity[MAX_SPOT_LIGHTS]; // rgb color, a=intensity
    vec4 conesRange[MAX_SPOT_LIGHTS];      // x=innerCos, y=outerCos, z=range, w=0
    vec4 attenuations[MAX_SPOT_LIGHTS];    // x=kc, y=kl, z=kq, w=0
};

layout(std140) uniform MaterialPropsBlock
{
    float textureRepeat;     // UV tiling
    float shininess;         // spec power
    float specularIntensity; // spec visibility
};

// MARK: Shadow factor
float shadowFactor(int lightIndex, vec3 N, vec3 L, vec4 fragPosLightSpace)
{
    if (lightIndex < 0 || lightIndex >= MAX_SHADOW_MAPS)
        return 1.0;

    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj = proj * 0.5 + 0.5;

    if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0)
        return 1.0;

    // Value to play with
    float bias = 0;
    float ref = proj.z - bias;

    vec2 texSize = vec2(textureSize(shadowMaps[0], 0)); // assume square maps

    // Align to bilinear footprint (texel centers)
    vec2 st = proj.xy * texSize - 0.5;
    vec2 iuv = floor(st);
    vec2 f = fract(st);
    vec2 base = iuv / texSize;
    vec2 texel = 1.0 / texSize;

    // Neighboring depths
    float d00 = texture(shadowMaps[lightIndex], base).r;
    float d10 = texture(shadowMaps[lightIndex], base + vec2(texel.x, 0.0)).r;
    float d01 = texture(shadowMaps[lightIndex], base + vec2(0.0, texel.y)).r;
    float d11 = texture(shadowMaps[lightIndex], base + vec2(texel.x, texel.y)).r;

    // Compare (lit if depth >= ref)
    float c00 = step(ref, d00);
    float c10 = step(ref, d10);
    float c01 = step(ref, d01);
    float c11 = step(ref, d11);

    // Bilinear weights
    float w00 = (1.0 - f.x) * (1.0 - f.y);
    float w10 = f.x * (1.0 - f.y);
    float w01 = (1.0 - f.x) * f.y;
    float w11 = f.x * f.y;

    return c00 * w00 + c10 * w10 + c01 * w01 + c11 * w11;
}

// MARK: Spot light shading
vec3 shadeSpotLight(int i, vec3 N, vec3 V)
{
    vec3 Lpos = positionsWS[i].xyz;
    vec3 Ldir = normalize(directionsWS[i].xyz);

    vec3 color = colorsIntensity[i].rgb;
    float intensity = colorsIntensity[i].a;

    float innerCos = conesRange[i].x;
    float outerCos = conesRange[i].y;
    float range = conesRange[i].z; // light range (we packed it here)

    float kc = attenuations[i].x;
    float kl = attenuations[i].y;
    float kq = attenuations[i].z;

    vec3 Lvec = Lpos - FragPos;
    float dist = length(Lvec);
    if (dist <= 0.0001)
        return vec3(0.0);

    vec3 L = Lvec / dist;
    float theta = dot(L, -Ldir);

    float eps = max(innerCos - outerCos, 1e-5);
    float spot = clamp((theta - outerCos) / eps, 0.0, 1.0);

    float inRange = step(dist, range);
    float atten = 1.0 / (kc + kl * dist + kq * dist * dist);

    vec4 fragPosLightSpace = lightSpaceMatrices[i] * vec4(FragPos, 1.0);
    float shadow = shadowFactor(i, N, L, fragPosLightSpace);

    float NdotL = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, shininess) * specularIntensity;

    vec3 radiance = color * intensity * atten * spot * inRange * shadow;
    return radiance * (NdotL + spec);
}

// MARK: Main
void main()
{
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec2 scaledUV = v_UV * textureRepeat;
    vec4 albedo = texture(diffuseMap, scaledUV);

    vec3 ambient = vec3(0.5, 0.55, 0.6);

    int count = clamp(u_numLights, 0, MAX_SPOT_LIGHTS);
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < count; ++i)
        lighting += shadeSpotLight(i, N, V);

    vec4 color = vec4(ambient + lighting, 1.0) * albedo;
    if (color.a < 0.5)
        discard;
    FragColor = color;
}
