#version 410 core

// ===== Inputs / Outputs =====
in vec2 v_UV;
in vec3 v_Normal;
in vec3 FragPos;
out vec4 FragColor;

// ===== Material / Textures =====
uniform vec3 viewPos;
uniform sampler2D diffuseMap;

#define MAX_SPOT_LIGHTS 16
#define MAX_POINT_LIGHTS 16

// --- Spot shadow map array (2D array) ---
uniform sampler2DArrayShadow uSpotLightShadowMapArray;
uniform mat4 uSpotLightSpaceMatrices[MAX_SPOT_LIGHTS];
uniform int uSpotLightCount;

// --- Point shadow map array (CUBEMAP array) ---
uniform samplerCubeArrayShadow uPointLightShadowCubeArray;
uniform int uPointLightCount;

// ===== UBOs =====
layout(std140) uniform SpotLights
{
    // xyz position (w unused)
    vec4 positionsWS[MAX_SPOT_LIGHTS];
    // xyz direction ***OUTWARD*** from the light (w unused)
    vec4 directionsWS[MAX_SPOT_LIGHTS];
    // rgb color, a=intensity
    vec4 colorsIntensity[MAX_SPOT_LIGHTS];
    // x=innerCos, y=outerCos, z=range, w=0
    vec4 conesRange[MAX_SPOT_LIGHTS];
    // attenuation: x=kc, y=kl, z=kq, w=0
    vec4 attenuations[MAX_SPOT_LIGHTS];
};

layout(std140) uniform PointLights
{
    // xyz position (w unused)
    vec4 p_positionsWS[MAX_POINT_LIGHTS];
    // rgb color, a=intensity
    vec4 p_colorsIntensity[MAX_POINT_LIGHTS];
    // attenuation: x=kc, y=kl, z=kq, w=0
    vec4 p_attenuations[MAX_POINT_LIGHTS];
    // range/far: x=range (far plane used when writing depth = dist/range), y=z=w unused
    vec4 p_rangeFar[MAX_POINT_LIGHTS];
};

layout(std140) uniform MaterialPropsBlock
{
    float textureRepeat;     // UV tiling
    float shininess;         // specular power (optional)
    float specularIntensity; // specular intensity (optional)
};

// ===== Tunables =====
const vec3 AMBIENT_COLOR = vec3(0.5, 0.55, 0.6);
const float ALPHA_CUTOFF = 0.5;

// ===== Shadow PCF controls (spot only; point uses HW linear filter) =====
#ifndef SHADOW_PCF_ENABLED
#define SHADOW_PCF_ENABLED 3
#endif

#ifndef SHADOW_PCF_RADIUS
#define SHADOW_PCF_RADIUS 3
#endif

// ===== Helpers (common) =====
float attenuation(int i, float dist)
{
    float kc = attenuations[i].x;
    float kl = attenuations[i].y;
    float kq = attenuations[i].z;
    return 1.0 / (kc + kl * dist + kq * dist * dist);
}

float pattn(int i, float dist)
{
    float kc = p_attenuations[i].x;
    float kl = p_attenuations[i].y;
    float kq = p_attenuations[i].z;
    return 1.0 / (kc + kl * dist + kq * dist * dist);
}

// ===== Spot light =====
float spotMask(int i, vec3 L, vec3 LdirOut)
{
    float innerCos = conesRange[i].x;
    float outerCos = conesRange[i].y;
    float theta = dot(L, -normalize(LdirOut));
    float eps = max(innerCos - outerCos, 1e-5);
    return clamp((theta - outerCos) / eps, 0.0, 1.0);
}

float shadowPCF(int layer, vec2 uv, float ref, float bias)
{
#if SHADOW_PCF_ENABLED
    vec2 texel = 1.0 / vec2(textureSize(uSpotLightShadowMapArray, 0));
    float sum = 0.0;
    int r = SHADOW_PCF_RADIUS;
    int taps = 0;
    for (int dy = -r; dy <= r; ++dy)
    {
        for (int dx = -r; dx <= r; ++dx)
        {
            vec2 offs = vec2(dx, dy) * texel;
            sum += texture(uSpotLightShadowMapArray, vec4(uv + offs, float(layer), ref - bias));
            ++taps;
        }
    }
    return sum / float(taps);
#else
    return texture(uSpotLightShadowMapArray, vec4(uv, float(layer), ref - bias));
#endif
}

float shadowFactorSpot(int i, vec3 worldPos /*, vec3 N, vec3 L*/)
{
    vec4 clip = uSpotLightSpaceMatrices[i] * vec4(worldPos, 1.0);
    if (clip.w <= 0.0)
        return 1.0;

    vec3 ndc = clip.xyz / clip.w; // [-1,1]
    if (ndc.x < -1.0 || ndc.x > 1.0 || ndc.y < -1.0 || ndc.y > 1.0 || ndc.z < 0.0 || ndc.z > 1.0)
        return 1.0;

    vec2 uv = ndc.xy * 0.5 + 0.5;
    float ref = ndc.z * 0.5 + 0.5;

    // Bias can be tuned in your depth pass via polygon offset; leave 0 here for HW compare.
    const float bias = 0.0;
    return shadowPCF(i, uv, ref, bias);
}

// ===== Point light (cubemap array shadow) =====
// Assumes the depth has been written as: depth = distance(light, worldPos) / rangeFar[i]
// and the sampler has compare mode enabled (GL_COMPARE_REF_TO_TEXTURE).
float shadowFactorPoint(int i, vec3 Ldir, float dist)
{
    // Normalize direction for cubemap lookup
    vec3 dir = normalize(-Ldir);

    float farRange = max(p_rangeFar[i].x, 1e-6);
    float ref = dist / farRange;

    // Optional small bias to reduce acne; tweak if needed (or prefer offset in depth pass)
    const float bias = 0.0;
    float compare = ref - bias;

    // Hardware comparison w/ cubemap array: texture(samplerCubeArrayShadow, vec4(dir, layer), ref)
    // If the cubemap’s min/mag filters are LINEAR with compare enabled, you get 2x2 PCF-ish filtering.
    return texture(uPointLightShadowCubeArray, vec4(dir, float(i)), compare);
}

void main()
{
    vec3 N = normalize(v_Normal);
    vec2 uv = v_UV * textureRepeat;

    vec4 tex = texture(diffuseMap, uv);
    if (tex.a < ALPHA_CUTOFF)
        discard;

    vec3 albedo = tex.rgb;
    vec3 V = normalize(viewPos - FragPos);

    vec3 lighting = AMBIENT_COLOR;

    // ---- Spot lights ----
    for (int i = 0; i < uSpotLightCount; ++i)
    {
        vec3 Lvec = positionsWS[i].xyz - FragPos;
        float dist = length(Lvec);
        vec3 L = (dist > 1e-6) ? Lvec / dist : vec3(0.0);

        float spot = spotMask(i, L, directionsWS[i].xyz);
        if (spot <= 0.0001)
            continue;

        float atten = attenuation(i, dist);
        float NdotL = max(dot(N, L), 0.0);
        float s = shadowFactorSpot(i, FragPos /*, N, L*/);

        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float spec = pow(NdotH, max(shininess, 1.0)) * specularIntensity;

        vec3 lightRGB = colorsIntensity[i].rgb * colorsIntensity[i].a;
        float litTerm = (NdotL + spec) * s;

        lighting += lightRGB * litTerm * atten * spot;
    }

    // ---- Point lights ----
    for (int i = 0; i < uPointLightCount; ++i)
    {
        vec3 Lvec = p_positionsWS[i].xyz - FragPos;
        float dist = length(Lvec);
        if (dist <= 1e-6)
            continue;

        vec3 L = Lvec / dist;

        float atten = pattn(i, dist);
        float NdotL = max(dot(N, L), 0.0);

        // Cubemap array hardware-compare shadow
        // float s = shadowFactorPoint(i, L, dist);
        float s = shadowFactorPoint(i, L, dist);

        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float spec = pow(NdotH, max(shininess, 1.0)) * specularIntensity;

        vec3 lightRGB = p_colorsIntensity[i].rgb * p_colorsIntensity[i].a;
        float litTerm = (NdotL + spec) * s;

        lighting += lightRGB * litTerm * atten;
    }

    vec3 finalRGB = albedo * lighting;
    FragColor = vec4(finalRGB, tex.a);
}
