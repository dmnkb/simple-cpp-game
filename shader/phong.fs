#version 410 core

// Inputs
in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;

// Outputs
out vec4 FragColor;

// Material / Textures
uniform vec3 uViewPos;
uniform sampler2D uDiffuseMap;

// Max light defines
#define MAX_SPOT_LIGHTS 16
#define MAX_POINT_LIGHTS 16
#define DIRECTIONAL_LIGHT_CASCADES 4

// Shadow map arrays
// Spot
uniform sampler2DArrayShadow uSpotLightShadowMapArray;
uniform mat4 uSpotLightSpaceMatrices[MAX_SPOT_LIGHTS];
uniform int uSpotLightCount;

// Point
uniform samplerCubeArrayShadow uPointLightShadowCubeArray;
uniform int uPointLightCount;

// Directional
uniform sampler2DArrayShadow uDirectionalLightShadowMapArray;
uniform mat4 uDirectionalLightSpaceMatrix[DIRECTIONAL_LIGHT_CASCADES];

// MARK: Light blocks
layout(std140) uniform SpotLights
{
    vec4 positionsWS[MAX_SPOT_LIGHTS];     // xyz position (w unused)
    vec4 directionsWS[MAX_SPOT_LIGHTS];    // xyz direction from the light (w unused)
    vec4 colorsIntensity[MAX_SPOT_LIGHTS]; // rgb color, a=intensity
    vec4 conesRange[MAX_SPOT_LIGHTS];      // x=innerCos, y=outerCos, z=range, w=0
    vec4 attenuations[MAX_SPOT_LIGHTS];    // attenuation: x=kc, y=kl, z=kq, w=0
};

layout(std140) uniform PointLights
{
    vec4 p_positionsWS[MAX_POINT_LIGHTS];     // xyz position (w unused)
    vec4 p_colorsIntensity[MAX_POINT_LIGHTS]; // rgb color, a=intensity
    vec4 p_attenuations[MAX_POINT_LIGHTS];    // attenuation: x=kc, y=kl, z=kq, w=0
    vec4 p_rangeFar[MAX_POINT_LIGHTS];        // range/far: x=range
};

// Directional light UBO
layout(std140) uniform DirectionalLight
{
    vec4 directionWS;     // xyz = direction toward scene, w unused
    vec4 colorsIntensity; // rgb color, a = intensity
}
uDirLight;

// MARK: Material block
layout(std140) uniform MaterialPropsBlock
{
    float textureRepeat;
    float shininess;
    float specularIntensity;
};

// MARK: Tunables
const vec3 AMBIENT_COLOR = vec3(0.5, 0.55, 0.6);
const float ALPHA_CUTOFF = 0.5;

// Bias tuning
const float MIN_BIAS = 0.00012;  // base bias
const float SLOPE_BIAS = 0.0025; // extra bias at grazing angles
const float RPDB_SCALE = 0.75;   // derivative-based receiver-plane term (0.5–1.0 typical)

// MARK: Helpers
float attenuation(int i, float dist)
{
    float kc = attenuations[i].x;
    float kl = attenuations[i].y;
    float kq = attenuations[i].z;
    return 1.0 / (kc + kl * dist + kq * dist * dist);
}

// Pattern attenuation for point lights
float pattn(int i, float dist)
{
    float kc = p_attenuations[i].x;
    float kl = p_attenuations[i].y;
    float kq = p_attenuations[i].z;
    return 1.0 / (kc + kl * dist + kq * dist * dist);
}

float spotMask(int i, vec3 L, vec3 LdirOut)
{
    float innerCos = conesRange[i].x;
    float outerCos = conesRange[i].y;
    float theta = dot(L, -normalize(LdirOut));
    float eps = max(innerCos - outerCos, 1e-5);
    return clamp((theta - outerCos) / eps, 0.0, 1.0);
}

float slopeBias(vec3 N, vec3 L)
{
    float nl = max(dot(N, L), 0.0);
    return MIN_BIAS + SLOPE_BIAS * (1.0 - nl);
}

// Receiver-plane depth bias (RPDB)
float rpdbFromNdc(vec3 ndc)
{
    float ref = ndc.z * 0.5 + 0.5;
    vec2 d = vec2(dFdx(ref), dFdy(ref));
    return RPDB_SCALE * length(d);
}

// MARK: Directional light shadow factor
float shadowFactorDirectional(vec3 worldPos, vec3 N, vec3 L)
{
    for (int c = 0; c < DIRECTIONAL_LIGHT_CASCADES; ++c)
    {
        vec4 clip = uDirectionalLightSpaceMatrix[c] * vec4(worldPos, 1.0);
        if (clip.w <= 0.0)
            continue;

        vec3 ndc = clip.xyz / clip.w; // [-1, 1]
        if (ndc.x < -1.0 || ndc.x > 1.0 || ndc.y < -1.0 || ndc.y > 1.0 || ndc.z < -1.0 || ndc.z > 1.0)
            continue;

        vec2 uv = ndc.xy * 0.5 + 0.5;
        float ref = ndc.z * 0.5 + 0.5;

        float bias = slopeBias(N, L) + rpdbFromNdc(ndc);
        return texture(uDirectionalLightShadowMapArray, vec4(uv, float(c), ref - bias));
    }
    // Outside all cascades -> lit
    return 1.0;
}

// MARK: Spot light shadow factor
float shadowFactorSpot(int i, vec3 worldPos, vec3 N, vec3 L)
{
    vec4 clip = uSpotLightSpaceMatrices[i] * vec4(worldPos, 1.0);
    if (clip.w <= 0.0)
        return 1.0;

    vec3 ndc = clip.xyz / clip.w; // [-1,1]
    if (ndc.x < -1.0 || ndc.x > 1.0 || ndc.y < -1.0 || ndc.y > 1.0 || ndc.z < 0.0 || ndc.z > 1.0)
        return 1.0;

    vec2 uv = ndc.xy * 0.5 + 0.5;
    float ref = ndc.z * 0.5 + 0.5;

    float bias = 0.0;
    return texture(uSpotLightShadowMapArray, vec4(uv, float(i), ref - bias));
}

// MARK: Point light shadow factor
float shadowFactorPoint(int i, vec3 Ldir, float dist, vec3 N)
{
    vec3 dir = normalize(-Ldir);
    float farRange = max(p_rangeFar[i].x, 1e-6);
    float ref = dist / farRange;

    float bias = MIN_BIAS + SLOPE_BIAS * (1.0 - max(dot(N, -dir), 0.0));
    return texture(uPointLightShadowCubeArray, vec4(dir, float(i)), ref - bias);
}

void main()
{
    vec3 N = normalize(vNormal);
    vec2 uv = vUV * textureRepeat;

    vec4 tex = texture(uDiffuseMap, uv);
    if (tex.a < ALPHA_CUTOFF)
        discard;

    vec3 albedo = tex.rgb;
    vec3 V = normalize(uViewPos - vFragPos);

    vec3 lighting = AMBIENT_COLOR;

    // MARK: Directional light
    {
        vec3 L = normalize(-uDirLight.directionWS.xyz);
        float NdotL = max(dot(N, L), 0.0);

        float s = shadowFactorDirectional(vFragPos, N, L);

        // Blinn-Phong specular
        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float spec = pow(NdotH, max(shininess, 1.0)) * specularIntensity;

        vec3 lightRGB = uDirLight.colorsIntensity.rgb * uDirLight.colorsIntensity.a;
        lighting += lightRGB * (NdotL + spec) * s; // no attenuation
    }

    // MARK: Spot lights
    for (int i = 0; i < uSpotLightCount; ++i)
    {
        vec3 Lvec = positionsWS[i].xyz - vFragPos;
        float dist = length(Lvec);
        vec3 L = (dist > 1e-6) ? Lvec / dist : vec3(0.0);

        float spot = spotMask(i, L, directionsWS[i].xyz);
        if (spot <= 0.0001)
            continue;

        float atten = attenuation(i, dist);
        float NdotL = max(dot(N, L), 0.0);
        float s = shadowFactorSpot(i, vFragPos, N, L);

        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float spec = pow(NdotH, max(shininess, 1.0)) * specularIntensity;

        vec3 lightRGB = colorsIntensity[i].rgb * colorsIntensity[i].a;
        lighting += lightRGB * (NdotL + spec) * s * atten * spot;
    }

    // MARK: Point lights
    for (int i = 0; i < uPointLightCount; ++i)
    {
        vec3 Lvec = p_positionsWS[i].xyz - vFragPos;
        float dist = length(Lvec);
        if (dist <= 1e-6)
            continue;

        vec3 L = Lvec / dist;

        float atten = pattn(i, dist);
        float NdotL = max(dot(N, L), 0.0);
        float s = shadowFactorPoint(i, L, dist, N);

        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float spec = pow(NdotH, max(shininess, 1.0)) * specularIntensity;

        vec3 lightRGB = p_colorsIntensity[i].rgb * p_colorsIntensity[i].a;
        lighting += lightRGB * (NdotL + spec) * s * atten;
    }

    vec3 finalRGB = albedo * lighting;
    FragColor = vec4(finalRGB, tex.a);
}
