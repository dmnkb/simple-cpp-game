#version 330 core

in vec2 v_UV;
in vec3 v_Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D diffuseMap;

#define MAX_SPOT_LIGHTS 16

uniform sampler2DArrayShadow uShadowMapArray;
uniform mat4 lightSpaceMatrices[MAX_SPOT_LIGHTS];
uniform int u_numLights;

// ---------------- UBOs ----------------
layout(std140) uniform SpotLights
{
    vec4 positionsWS[MAX_SPOT_LIGHTS];     // xyz pos, w=1
    vec4 directionsWS[MAX_SPOT_LIGHTS];    // xyz dir (***OUTWARD*** from the light), w=0
    vec4 colorsIntensity[MAX_SPOT_LIGHTS]; // rgb color, a=intensity
    vec4 conesRange[MAX_SPOT_LIGHTS];      // x=innerCos, y=outerCos, z=range, w=0
    vec4 attenuations[MAX_SPOT_LIGHTS];    // x=kc, y=kl, z=kq, w=0
};

layout(std140) uniform MaterialPropsBlock
{
    float textureRepeat;     // UV tiling
    float shininess;         // specular power (optional)
    float specularIntensity; // specular intensity (optional)
};

// ---------------- Tunables ----------------
const vec3 AMBIENT_COLOR = vec3(0.5, 0.55, 0.6);
const float ALPHA_CUTOFF = 0.5;

// ---------------- PCF controls ----------------
// 1 = enable PCF; 0 = single tap (still uses HW compare)
#ifndef SHADOW_PCF_ENABLED
#define SHADOW_PCF_ENABLED 3
#endif
// PCF radius in texels: 1 => 3x3, 2 => 5x5
#ifndef SHADOW_PCF_RADIUS
#define SHADOW_PCF_RADIUS 3
#endif

// ---------------- Helpers ----------------
float spotMask(int i, vec3 L, vec3 LdirOut)
{
    // UBO stores ***OUTWARD*** direction; for the cone use -LdirOut (into the cone)
    float innerCos = conesRange[i].x;
    float outerCos = conesRange[i].y;
    float theta = dot(L, -normalize(LdirOut));
    float eps = max(innerCos - outerCos, 1e-5);
    return clamp((theta - outerCos) / eps, 0.0, 1.0);
}

float attenuation(int i, float dist)
{
    float kc = attenuations[i].x;
    float kl = attenuations[i].y;
    float kq = attenuations[i].z;
    return 1.0 / (kc + kl * dist + kq * dist * dist);
}

float shadowPCF(int layer, vec2 uv, float ref, float bias)
{
#if SHADOW_PCF_ENABLED
    // Uniform-weight box PCF
    vec2 texel = 1.0 / vec2(textureSize(uShadowMapArray, 0));
    float sum = 0.0;
    int r = SHADOW_PCF_RADIUS;
    int taps = 0;
    for (int dy = -r; dy <= r; ++dy)
        for (int dx = -r; dx <= r; ++dx)
        {
            vec2 offs = vec2(dx, dy) * texel;
            sum += texture(uShadowMapArray, vec4(uv + offs, float(layer), ref - bias));
            ++taps;
        }
    return sum / float(taps);
#else
    return texture(uShadowMapArray, vec4(uv, float(layer), ref - bias));
#endif
}

float shadowFactor(int i, vec3 worldPos, vec3 N, vec3 L)
{
    vec4 clip = lightSpaceMatrices[i] * vec4(worldPos, 1.0);
    if (clip.w <= 0.0)
        return 1.0; // behind light camera => lit

    vec3 ndc = clip.xyz / clip.w; // [-1,1]
    if (ndc.x < -1.0 || ndc.x > 1.0 || ndc.y < -1.0 || ndc.y > 1.0 || ndc.z < 0.0 || ndc.z > 1.0)
        return 1.0;

    vec2 uv = ndc.xy * 0.5 + 0.5;
    float ref = ndc.z * 0.5 + 0.5;

    // Slope-scaled bias
    float slope = 1.0 - max(dot(N, L), 0.0);
    float bias = max(0.001 * slope, 0.0005);

    return shadowPCF(i, uv, ref, bias);
}

// ---------------- Main ----------------
void main()
{
    vec3 N = normalize(v_Normal);
    vec2 uv = v_UV * textureRepeat;

    // Sample with alpha and do cutout test
    vec4 tex = texture(diffuseMap, uv);
    if (tex.a < ALPHA_CUTOFF)
        discard;

    vec3 albedo = tex.rgb;
    vec3 V = normalize(viewPos - FragPos);

    // Global ambient (not shadowed)
    vec3 lighting = AMBIENT_COLOR;

    for (int i = 0; i < u_numLights; ++i)
    {
        vec3 Lvec = positionsWS[i].xyz - FragPos;
        float dist2 = dot(Lvec, Lvec);
        float dist = sqrt(dist2);
        vec3 L = Lvec / max(dist, 1e-6);

        // cone (NOTE: directionsWS is ***OUTWARD***)
        vec3 LdirOut = directionsWS[i].xyz;
        float spot = spotMask(i, L, LdirOut);
        if (spot <= 0.0001)
            continue;

        float atten = attenuation(i, dist);
        float NdotL = max(dot(N, L), 0.0);

        // Shadow factor (applies to BOTH diffuse & spec)
        float s = shadowFactor(i, FragPos, N, L);

        // Simple Blinn spec
        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float spec = pow(NdotH, max(shininess, 1.0)) * specularIntensity;

        vec3 lightRGB = colorsIntensity[i].rgb * colorsIntensity[i].a;

        // Diffuse and spec both shadowed
        float litTerm = (NdotL + spec) * s;

        // Accumulate (light color scales the term; albedo applied later)
        lighting += lightRGB * litTerm * atten * spot;
    }

    // Apply albedo to the lighting result
    vec3 finalRGB = albedo * lighting;
    FragColor = vec4(finalRGB, tex.a);
}
