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
uniform vec4 uAmbientLightColor;

// Max light defines
#define MAX_SPOT_LIGHTS 16
#define MAX_POINT_LIGHTS 16
#define DIRECTIONAL_LIGHT_CASCADES 4

// Shadow map arrays (NON-comparison samplers)
uniform sampler2DArray uSpotLightShadowMapArray;
uniform mat4 uSpotLightSpaceMatrices[MAX_SPOT_LIGHTS];
uniform int uSpotLightCount;

uniform samplerCubeArray uPointLightShadowCubeArray;
uniform int uPointLightCount;

uniform sampler2DArray uDirectionalLightShadowMapArray;
uniform mat4 uDirectionalLightSpaceMatrix[DIRECTIONAL_LIGHT_CASCADES];

// MARK: Light blocks (unchanged)
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

layout(std140) uniform DirectionalLight
{
    vec4 directionWS;     // xyz = direction toward scene, w unused
    vec4 colorsIntensity; // rgb color, a = intensity
}
uDirLight;

// MARK: Material block (unchanged)
layout(std140) uniform MaterialPropsBlock
{
    float textureRepeat;
    float shininess;
    float specularIntensity;
};

// MARK: Tunables
const float ALPHA_CUTOFF = 0.5;

// Bias tuning (keep as before)
const float MIN_BIAS = 0.00012;
const float SLOPE_BIAS = 0.0025;
const float RPDB_SCALE = 0.75;

// -------- PCSS controls & texel sizes --------
const float uDirLightRadiusWS = 0.05f;
const float uSpotLightRadiusWS = 1.05f;
const float uPointLightRadiusWS = 1.05f;

const vec2 uDirShadowTexel = vec2(1.0 / 1024.0, 1.0 / 1024.0);
const vec2 uSpotShadowTexel = vec2(1.0 / 1024.0, 1.0 / 1024.0);
const float uPointShadowTexel = 1.0 / 1024.0;

// Sample counts & filter clamps
const int PCSS_SAMPLES_BLOCKER = 6;
const int PCSS_SAMPLES_PCF = 12;
const float PCSS_MAX_FILTER = 15.0;
const float PCSS_MIN_FILTER = 1.0;

// Optional: tiny world-space normal offset ONLY for the spot shadow test
const float SPOT_NORMAL_OFFSET_WS = 0.015; // ~mm scale; tune per scene

// MARK: Helpers (unchanged)
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

float rpdbFromNdc(vec3 ndc)
{
    float ref = ndc.z * 0.5 + 0.5;
    vec2 d = vec2(dFdx(ref), dFdy(ref));
    return RPDB_SCALE * length(d);
}

// ------------------------
// Poisson sets
// ------------------------
const vec2 POISSON_12[12] = vec2[](vec2(-0.326, -0.406), vec2(-0.840, -0.074), vec2(-0.696, 0.457), vec2(-0.203, 0.621),
                                   vec2(0.962, -0.195), vec2(0.473, -0.480), vec2(0.519, 0.767), vec2(0.185, -0.893),
                                   vec2(0.114, 0.223), vec2(0.885, 0.463), vec2(-0.111, -0.137), vec2(-0.321, 0.932));
const vec2 POISSON_24[24] =
    vec2[](vec2(-0.983, 0.184), vec2(-0.919, -0.394), vec2(-0.701, 0.712), vec2(-0.643, -0.265), vec2(-0.454, 0.144),
           vec2(-0.401, -0.882), vec2(-0.276, 0.936), vec2(-0.131, -0.245), vec2(0.040, -0.539), vec2(0.045, 0.321),
           vec2(0.146, 0.888), vec2(0.233, -0.112), vec2(0.328, -0.730), vec2(0.426, 0.172), vec2(0.614, 0.780),
           vec2(0.655, -0.238), vec2(0.701, 0.010), vec2(0.735, -0.675), vec2(0.803, 0.315), vec2(0.857, -0514),
           vec2(0.905, -0.099), vec2(0.936, 0.658), vec2(-0.036, 0.681), vec2(-0.221, -0.551));

// Optional tiny per-pixel rotation to reduce patterns
float hash12(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}
mat2 rot2(float a)
{
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

// ------------------------
// Single-tap compare helpers (no bilinear)
// ------------------------
float compare2D(sampler2DArray shad, vec2 uv, float layer, float refDepth)
{
    float z = texture(shad, vec3(uv, layer)).r;
    return (refDepth <= z) ? 1.0 : 0.0;
}

void basisFromDir(vec3 n, out vec3 t, out vec3 b)
{
    vec3 a = (abs(n.z) < 0.999) ? vec3(0, 0, 1) : vec3(0, 1, 0);
    t = normalize(cross(a, n));
    b = cross(n, t);
}

float compareCube(samplerCubeArray shad, vec3 dir, float layer, float refDepth)
{
    float z = texture(shad, vec4(dir, layer)).r;
    return (refDepth <= z) ? 1.0 : 0.0;
}

// ------------------------
// PCSS: blocker search & PCF (2D) – single-sample taps
// ------------------------
bool findBlocker2D(sampler2DArray shad, vec2 uv, float layer, float refDepth, vec2 texel, float searchRadiusTexels,
                   out float avgBlocker, mat2 R)
{
    float sum = 0.0;
    int cnt = 0;
    vec2 radius = texel * searchRadiusTexels;

    for (int k = 0; k < PCSS_SAMPLES_BLOCKER; ++k)
    {
        vec2 duv = (R * POISSON_12[k]) * radius;
        float z = texture(shad, vec3(uv + duv, layer)).r;
        if (z < refDepth)
        {
            sum += z;
            cnt++;
        }
    }
    if (cnt == 0)
        return false;
    avgBlocker = sum / float(cnt);
    return true;
}

float pcf2D(sampler2DArray shad, vec2 uv, float layer, float refDepth, vec2 texel, float radiusTexels, mat2 R)
{
    vec2 radius = texel * radiusTexels;
    float sum = 0.0, wsum = 0.0;

    for (int k = 0; k < PCSS_SAMPLES_PCF; ++k)
    {
        vec2 o = R * POISSON_24[k];
        float w = 1.0 - clamp(length(o), 0.0, 1.0); // tent weight
        vec2 duv = o * radius;

        float s = compare2D(shad, uv + duv, layer, refDepth);
        sum += s * w;
        wsum += w;
    }
    return sum / max(wsum, 1e-6);
}

// ------------------------
// PCSS: blocker search & PCF (cube) – single-sample taps
// ------------------------
bool findBlockerCube(samplerCubeArray shad, vec3 dir, float layer, float refDepth, float texelAngle,
                     float searchRadiusTexels, out float avgBlocker, mat2 R)
{
    vec3 t, b;
    basisFromDir(dir, t, b);
    float angle = texelAngle * searchRadiusTexels;

    float sum = 0.0;
    int cnt = 0;
    for (int k = 0; k < PCSS_SAMPLES_BLOCKER; ++k)
    {
        vec2 o = R * POISSON_12[k];
        vec3 d = normalize(dir + (o.x * angle) * t + (o.y * angle) * b);
        float z = texture(shad, vec4(d, layer)).r;
        if (z < refDepth)
        {
            sum += z;
            cnt++;
        }
    }
    if (cnt == 0)
        return false;
    avgBlocker = sum / float(cnt);
    return true;
}

float pcfCube(samplerCubeArray shad, vec3 dir, float layer, float refDepth, float texelAngle, float radiusTexels,
              mat2 R)
{
    vec3 t, b;
    basisFromDir(dir, t, b);
    float angle = texelAngle * radiusTexels;

    float sum = 0.0, wsum = 0.0;
    for (int k = 0; k < PCSS_SAMPLES_PCF; ++k)
    {
        vec2 o = R * POISSON_24[k];
        float w = 1.0 - clamp(length(o), 0.0, 1.0);
        vec3 d = normalize(dir + (o.x * angle) * t + (o.y * angle) * b);

        float s = compareCube(shad, d, layer, refDepth);
        sum += s * w;
        wsum += w;
    }
    return sum / max(wsum, 1e-6);
}

// ---------------------------
// Directional light PCSS (unchanged)
// ---------------------------
float shadowFactorDirectional(vec3 worldPos, vec3 N, vec3 L)
{
    for (int c = 0; c < DIRECTIONAL_LIGHT_CASCADES; ++c)
    {
        vec4 clip = uDirectionalLightSpaceMatrix[c] * vec4(worldPos, 1.0);
        if (clip.w <= 0.0)
            continue;

        vec3 ndc = clip.xyz / clip.w;
        if (ndc.x < -1.0 || ndc.x > 1.0 || ndc.y < -1.0 || ndc.y > 1.0 || ndc.z < -1.0 || ndc.z > 1.0)
            continue;

        vec2 uv = ndc.xy * 0.5 + 0.5;
        float ref = ndc.z * 0.5 + 0.5;

        float bias = slopeBias(N, L) + rpdbFromNdc(ndc);
        float r = ref - bias;

        float ang = hash12(gl_FragCoord.xy) * 6.2831853;
        mat2 R = rot2(ang);

        float searchRadius = clamp(uDirLightRadiusWS * 150.0, 1.0, 25.0);
        float avgBlocker;
        bool hasBlocker = findBlocker2D(uDirectionalLightShadowMapArray, uv, float(c), r, uDirShadowTexel, searchRadius,
                                        avgBlocker, R);
        if (!hasBlocker)
            return 1.0;

        float penumbra = clamp(((r - avgBlocker) / max(avgBlocker, 1e-4)) * 150.0 * uDirLightRadiusWS, PCSS_MIN_FILTER,
                               PCSS_MAX_FILTER);

        return pcf2D(uDirectionalLightShadowMapArray, uv, float(c), r, uDirShadowTexel, penumbra, R);
    }
    return 1.0;
}

// =====================================================
// SPOT LIGHT — linearized PCSS + RPDB to fix peter-panning
// =====================================================

// Light's near plane used for the spot shadow camera (matches your C++)
const float SPOT_NEAR = 0.05;

// Linearize depth in [0..1] using near/far
float linearizeDepth01(float d, float n, float f)
{
    float z = d * 2.0 - 1.0;                      // back to NDC
    return (2.0 * n * f) / (f + n - z * (f - n)); // positive view-space depth
}

// Receiver-plane depth bias for perspective (spot) shadow maps.
// Inputs are the light-clip-space position (pre-divide).
float rpdbSpot(vec4 clip, vec2 texelSize)
{
    // Light NDC
    vec3 s = clip.xyz / clip.w; // [-1,1]
    // Derivatives of ndc
    vec3 sdx = dFdx(s);
    vec3 sdy = dFdy(s);

    // uv = ndc.xy * 0.5 + 0.5 -> duv/dx = dndc/dx * 0.5, same for y
    vec2 uvdx = sdx.xy * 0.5;
    vec2 uvdy = sdy.xy * 0.5;

    float a = uvdx.x, b = uvdx.y;
    float c = uvdy.x, d = uvdy.y;
    float det = a * d - b * c;

    if (abs(det) < 1e-8)
    {
        // Fallback: conservative screen-space depth gradient scaled by texel
        float dzlen = length(vec2(sdx.z, sdy.z));
        return RPDB_SCALE * dzlen * max(texelSize.x, texelSize.y);
    }

    // Depth gradient wrt uv
    vec2 g;
    g.x = (d * sdx.z - b * sdy.z) / det;  // dz/du
    g.y = (-c * sdx.z + a * sdy.z) / det; // dz/dv

    float gradLen = length(g);
    float texelLen = length(texelSize);
    return RPDB_SCALE * gradLen * texelLen;
}

// Blocker search in linear depth
bool findBlocker2D_linear(sampler2DArray shad, vec2 uv, float layer, float refLin, vec2 texel, float radiusTexels,
                          out float avgBlockerLin, mat2 R, float n, float f)
{
    float sum = 0.0;
    int cnt = 0;
    vec2 radius = texel * radiusTexels;
    for (int k = 0; k < PCSS_SAMPLES_BLOCKER; ++k)
    {
        vec2 duv = (R * POISSON_12[k]) * radius;
        float z = texture(shad, vec3(uv + duv, layer)).r;
        float zLin = linearizeDepth01(z, n, f);
        if (zLin < refLin)
        {
            sum += zLin;
            cnt++;
        }
    }
    if (cnt == 0)
        return false;
    avgBlockerLin = sum / float(cnt);
    return true;
}

// PCF in linear depth
float pcf2D_linear(sampler2DArray shad, vec2 uv, float layer, float refLin, vec2 texel, float radiusTexels, mat2 R,
                   float n, float f)
{
    vec2 radius = texel * radiusTexels;
    float sum = 0.0, wsum = 0.0;
    for (int k = 0; k < PCSS_SAMPLES_PCF; ++k)
    {
        vec2 o = R * POISSON_24[k];
        float w = 1.0 - clamp(length(o), 0.0, 1.0);
        vec2 duv = o * radius;

        float z = texture(shad, vec3(uv + duv, layer)).r;
        float zLin = linearizeDepth01(z, n, f);
        float s = (refLin <= zLin) ? 1.0 : 0.0;

        sum += s * w;
        wsum += w;
    }
    return sum / max(wsum, 1e-6);
}

float shadowFactorSpot(int i, vec3 worldPos, vec3 N, vec3 L)
{
    // Optional world-space normal offset to reduce self-occlusion without silhouette expansion
    vec3 wsOffsetPos = worldPos + N * SPOT_NORMAL_OFFSET_WS;

    // Project (offset) world position into light space
    vec4 clip = uSpotLightSpaceMatrices[i] * vec4(wsOffsetPos, 1.0);
    if (clip.w <= 0.0)
        return 1.0;

    vec3 ndc = clip.xyz / clip.w;
    if (ndc.x < -1.0 || ndc.x > 1.0 || ndc.y < -1.0 || ndc.y > 1.0 || ndc.z < 0.0 || ndc.z > 1.0)
        return 1.0;

    vec2 uv = ndc.xy * 0.5 + 0.5;
    float ref = ndc.z * 0.5 + 0.5;

    // Use your light's far from UBO (conesRange[i].z)
    float farP = max(conesRange[i].z, SPOT_NEAR + 1e-4);

    // --- New: receiver-plane depth bias (computed in non-linear depth space) ---
    float rpdb = rpdbSpot(clip, uSpotShadowTexel);

    // Gentler base & slope bias; RPDB does the heavy lifting
    float bias = MIN_BIAS * 0.1 + slopeBias(N, L) * 0.005;

    // Apply bias in the same space as the shadow map value
    float r = clamp(ref - bias, 0.0, 1.0);

    // Linearize reference depth for linear-PCSS math
    float refLin = linearizeDepth01(r, SPOT_NEAR, farP);

    float ang = hash12(gl_FragCoord.xy) * 6.2831853;
    mat2 R = rot2(ang);

    // Make search/filter grow with distance (simple distance scale)
    float distScale = clamp(refLin / farP, 0.0, 1.0); // 0 near .. 1 far
    float searchRadius = clamp(uSpotLightRadiusWS * (120.0) * (0.75 + 0.75 * distScale), 1.0, 25.0);

    float avgBlockerLin;
    bool hasBlocker = findBlocker2D_linear(uSpotLightShadowMapArray, uv, float(i), refLin, uSpotShadowTexel,
                                           searchRadius, avgBlockerLin, R, SPOT_NEAR, farP);
    if (!hasBlocker)
        return 1.0;

    float penumbraRaw = ((refLin - avgBlockerLin) / max(avgBlockerLin, 1e-4)) * uSpotLightRadiusWS * (120.0) *
                        (0.75 + 0.75 * distScale);
    float penumbra = clamp(penumbraRaw, PCSS_MIN_FILTER, PCSS_MAX_FILTER);

    return pcf2D_linear(uSpotLightShadowMapArray, uv, float(i), refLin, uSpotShadowTexel, penumbra, R, SPOT_NEAR, farP);
}

// ---------------------------
// Point light PCSS (unchanged)
// ---------------------------
float shadowFactorPoint(int i, vec3 Ldir, float dist, vec3 N)
{
    vec3 dir = normalize(-Ldir);
    float farRange = max(p_rangeFar[i].x, 1e-6);
    float ref = dist / farRange;

    float bias = MIN_BIAS + SLOPE_BIAS * (1.0 - max(dot(N, -dir), 0.0));
    float r = ref - bias;

    // Angular texel size approximation for 90° face FOV
    float texelAngle = uPointShadowTexel * 2.0 * 3.14159265 / 4.0;

    float ang = hash12(gl_FragCoord.xy) * 6.2831853;
    mat2 R = rot2(ang);

    float searchRadius = clamp(uPointLightRadiusWS * 150.0, 1.0, 25.0);

    float avgBlocker;
    bool hasBlocker =
        findBlockerCube(uPointLightShadowCubeArray, dir, float(i), r, texelAngle, searchRadius, avgBlocker, R);
    if (!hasBlocker)
        return 1.0;

    float penumbra = clamp(((r - avgBlocker) / max(avgBlocker, 1e-4)) * 150.0 * uPointLightRadiusWS, PCSS_MIN_FILTER,
                           PCSS_MAX_FILTER);

    return pcfCube(uPointLightShadowCubeArray, dir, float(i), r, texelAngle, penumbra, R);
}

// ---------------------------
// Directional/Spot/Point loops (unchanged shading)
// ---------------------------
void main()
{
    vec3 N = normalize(vNormal);
    vec2 uv = vUV * textureRepeat;

    vec4 tex = texture(uDiffuseMap, uv);
    if (tex.a < ALPHA_CUTOFF)
        discard;

    vec3 albedo = tex.rgb;
    vec3 V = normalize(uViewPos - vFragPos);

    vec3 lighting = uAmbientLightColor.rgb * uAmbientLightColor.a;

    // Directional
    {
        vec3 L = normalize(-uDirLight.directionWS.xyz);
        float NdotL = max(dot(N, L), 0.0);
        float s = shadowFactorDirectional(vFragPos, N, L);

        vec3 H = normalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float spec = pow(NdotH, max(shininess, 1.0)) * specularIntensity;

        vec3 lightRGB = uDirLight.colorsIntensity.rgb * uDirLight.colorsIntensity.a;
        lighting += lightRGB * (NdotL + spec) * s;
    }

    // Spot
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

    // Point
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
