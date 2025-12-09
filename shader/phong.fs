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

// Shadow map arrays (Regular samplers for blocker search - need depth values)
uniform sampler2DArray uSpotLightShadowMapArray;
uniform mat4 uSpotLightSpaceMatrices[MAX_SPOT_LIGHTS];
uniform int uSpotLightCount;

uniform samplerCubeArray uPointLightShadowCubeArray;
uniform int uPointLightCount;

uniform sampler2DArray uDirectionalLightShadowMapArray;
uniform mat4 uDirectionalLightSpaceMatrix[DIRECTIONAL_LIGHT_CASCADES];

// Shadow map arrays (Comparison samplers for PCF - hardware filtering)
uniform sampler2DArrayShadow uSpotLightShadowMapArrayCmp;
uniform samplerCubeArrayShadow uPointLightShadowCubeArrayCmp;
uniform sampler2DArrayShadow uDirectionalLightShadowMapArrayCmp;

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

// Bias tuning
// Bias tuning
const float MIN_BIAS = 0.0005; // Kept at 0.0005 to prevent acne
const float SLOPE_BIAS = 0.02; // Reduced from 0.01 to fix peter panning
const float RPDB_SCALE = 0.75;

// -------- PCSS controls & texel sizes --------
// Light world-space radius (controls shadow softness)
// Smaller = sharper shadows + better performance | Larger = softer shadows + slower
// Ideal ranges: Directional [0.01-0.1], Spot/Point [0.2-2.0]
const float uDirLightRadiusWS = 0.08f;   // Default: 0.03 (balanced)
const float uSpotLightRadiusWS = 1.05f;  // Default: 1.05
const float uPointLightRadiusWS = 1.05f; // Default: 1.05

// Shadow map texel sizes (must match actual shadow map resolution)
// Based on 1024x1024 maps. For 512: use 1.0/512.0, for 2048: use 1.0/2048.0
const vec2 uDirShadowTexel = vec2(1.0 / 1024.0, 1.0 / 1024.0);
const vec2 uSpotShadowTexel = vec2(1.0 / 1024.0, 1.0 / 1024.0);
const float uPointShadowTexel = 1.0 / 1024.0;

// -------- PCSS Sample Counts (PRIMARY PERFORMANCE KNOB) --------
// With hardware comparison samplers, each PCF sample = 2x2 bilinear filtered comparison
// This means effective coverage is 4x per sample, so we can use fewer samples!
//
// Quality Presets (with hardware PCF):
//   Ultra:       12 blocker, 16 PCF (excellent quality)
//   High:         8 blocker, 12 PCF (very good quality, RECOMMENDED)
//   Medium:       6 blocker,  8 PCF (good balance)
//   Low:          4 blocker,  6 PCF (acceptable for performance)
//   Performance:  4 blocker,  4 PCF (maximum speed)
const int PCSS_SAMPLES_BLOCKER = 8; // Blocker search samples (ideal range: 4-12)
const int PCSS_SAMPLES_PCF = 32;    // PCF filter samples (ideal range: 4-16)
//                                   // Note: Each PCF sample = 2x2 with hardware filtering!

// Filter kernel size clamps (in texels)
// Lower PCSS_MAX_FILTER = less soft shadows but better performance
// Ideal ranges: PCSS_MAX_FILTER [6.0-20.0], PCSS_MIN_FILTER [0.5-2.0]
const float PCSS_MAX_FILTER = 15.0; // Maximum filter radius (Increased to 20.0 for smoother falloff)
const float PCSS_MIN_FILTER = 1.0;  // Minimum filter radius (default: 1.0)

// Optional: tiny world-space normal offsets
const float SPOT_NORMAL_OFFSET_WS = 0.0; // Removed entirely to fix peter panning
const float DIR_NORMAL_OFFSET_WS = 0.01; // directional (Reverted to 0.01)

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

// ------------------------
// Poisson Disk Sampling - Industry Standard for PCSS
// ------------------------
// 16-sample pattern for blocker search (optimized blue-noise distribution)
const vec2 POISSON_16[16] =
    vec2[](vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725), vec2(-0.094184101, -0.92938870),
           vec2(0.34495938, 0.29387760), vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
           vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379), vec2(0.44323325, -0.97511554),
           vec2(0.53742981, -0.47373420), vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
           vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590), vec2(0.19984126, 0.78641367),
           vec2(0.14383161, -0.14100790));

// 32-sample pattern for PCF filtering (higher quality)
const vec2 POISSON_32[32] = vec2[](
    vec2(-0.975402, -0.0711386), vec2(-0.920347, -0.312706), vec2(-0.867103, 0.0636654), vec2(-0.799540, 0.349211),
    vec2(-0.765764, -0.543839), vec2(-0.711889, 0.616689), vec2(-0.626698, -0.780115), vec2(-0.581956, -0.196889),
    vec2(-0.534807, 0.844628), vec2(-0.451789, 0.484251), vec2(-0.421003, -0.408535), vec2(-0.378917, 0.131933),
    vec2(-0.250851, 0.761536), vec2(-0.231663, -0.750035), vec2(-0.212819, 0.358628), vec2(-0.162022, -0.136819),
    vec2(-0.083597, 0.566651), vec2(-0.043244, -0.442623), vec2(0.038385, 0.177851), vec2(0.109677, -0.749922),
    vec2(0.147027, 0.948695), vec2(0.206071, -0.194662), vec2(0.279452, 0.395757), vec2(0.331209, -0.942573),
    vec2(0.425181, 0.682868), vec2(0.483240, -0.554677), vec2(0.537317, 0.131939), vec2(0.623838, 0.873883),
    vec2(0.663456, -0.196449), vec2(0.733926, 0.437770), vec2(0.816447, -0.535477), vec2(0.902144, 0.135014));

// Per-pixel rotation to eliminate visible patterns
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
// PCSS: blocker search & PCF (2D) – Poisson disk sampling
// ------------------------
bool findBlocker2D(sampler2DArray shad, vec2 uv, float layer, float refDepth, vec2 texel, float searchRadiusTexels,
                   out float avgBlocker, mat2 R)
{
    float sum = 0.0;
    int cnt = 0;
    vec2 radius = texel * searchRadiusTexels;

    // Use POISSON_16 array (supports up to 16 samples)
    for (int k = 0; k < PCSS_SAMPLES_BLOCKER; ++k)
    {
        vec2 duv = (R * POISSON_16[k]) * radius;
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

float pcf2D(sampler2DArrayShadow shadCmp, vec2 uv, float layer, float refDepth, vec2 texel, float radiusTexels, mat2 R)
{
    vec2 radius = texel * radiusTexels;
    float sum = 0.0;

    // Use POISSON_32 array (supports up to 32 samples)
    // Hardware comparison sampler: each sample gets automatic 2x2 bilinear filtering + depth comparison
    for (int k = 0; k < PCSS_SAMPLES_PCF; ++k)
    {
        vec2 duv = (R * POISSON_32[k]) * radius;
        // texture() with sampler2DArrayShadow returns filtered comparison result (0.0-1.0)
        float s = texture(shadCmp, vec4(uv + duv, layer, refDepth));
        sum += s;
    }
    return sum / float(PCSS_SAMPLES_PCF);
}

// ------------------------
// PCSS: blocker search & PCF (cube) – Poisson disk sampling
// ------------------------
bool findBlockerCube(samplerCubeArray shad, vec3 dir, float layer, float refDepth, float texelAngle,
                     float searchRadiusTexels, out float avgBlocker, mat2 R)
{
    vec3 t, b;
    basisFromDir(dir, t, b);
    float angle = texelAngle * searchRadiusTexels;

    float sum = 0.0;
    int cnt = 0;

    // Use POISSON_16 array (supports up to 16 samples)
    for (int k = 0; k < PCSS_SAMPLES_BLOCKER; ++k)
    {
        vec2 o = R * POISSON_16[k];
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

float pcfCube(samplerCubeArrayShadow shadCmp, vec3 dir, float layer, float refDepth, float texelAngle,
              float radiusTexels, mat2 R)
{
    vec3 t, b;
    basisFromDir(dir, t, b);
    float angle = texelAngle * radiusTexels;

    float sum = 0.0;

    // Use POISSON_32 array (supports up to 32 samples)
    // Hardware comparison sampler: each sample gets automatic filtering + depth comparison
    for (int k = 0; k < PCSS_SAMPLES_PCF; ++k)
    {
        vec2 o = R * POISSON_32[k];
        vec3 d = normalize(dir + (o.x * angle) * t + (o.y * angle) * b);

        // texture() with samplerCubeArrayShadow returns filtered comparison result (0.0-1.0)
        float s = texture(shadCmp, vec4(d, layer), refDepth);
        sum += s;
    }
    return sum / float(PCSS_SAMPLES_PCF);
}

// =====================================================
// DIRECTIONAL LIGHT — PCSS with base + slope bias
// =====================================================

// Helper for directional shadow sampling (no RPDB → avoids cascade seams)
float sampleCascadePCSS(int cascadeIndex, vec3 worldPos, vec3 N, vec3 L, float seedOffset)
{
    vec4 clip = uDirectionalLightSpaceMatrix[cascadeIndex] * vec4(worldPos, 1.0);
    vec3 ndc = clip.xyz / clip.w;

    vec2 uv = ndc.xy * 0.5 + 0.5;
    float ref = ndc.z * 0.5 + 0.5;

    // 1. Calculate World Space Texel Size (orthographic)
    float scaleX = length(uDirectionalLightSpaceMatrix[cascadeIndex][0].xyz);
    float texelSizeWS = (2.0 / scaleX) / 1024.0; // Assuming 1024 map size

    // 2. Calculate World Space Z Range
    float scaleZ = length(uDirectionalLightSpaceMatrix[cascadeIndex][2].xyz);
    float zRangeWS = 2.0 / scaleZ;

    // --- Bias: base + slope only (same across cascades) ---
    float baseBias = MIN_BIAS * 0.5;
    float slope = slopeBias(N, L) * 0.7; // slightly scaled
    float biasPCF = baseBias + slope;    // Full bias for PCF to prevent acne
    float biasBlocker = biasPCF * 0.5;   // Increased from 0.2 to 0.5 to avoid self-shadowing in blocker search

    float rPCF = clamp(ref - biasPCF, 0.0, 1.0);
    float rBlocker = clamp(ref - biasBlocker, 0.0, 1.0);

    // Per-pixel rotation for Poisson disk samples
    float ang = hash12(gl_FragCoord.xy + vec2(seedOffset)) * 6.2831853;
    mat2 R = rot2(ang);

    // 3. WS-consistent search radius
    // Multiplier controls search area size - tune value [3.0-15.0] for performance vs quality
    float targetSearchRadiusWS = uDirLightRadiusWS * 2.0 * 10.0; // Increased to 10.0
    float searchRadiusTexels = targetSearchRadiusWS / texelSizeWS;
    searchRadiusTexels = clamp(searchRadiusTexels, 1.0, 32.0); // Max increased to 32.0

    float avgBlocker;
    bool hasBlocker = findBlocker2D(uDirectionalLightShadowMapArray, uv, float(cascadeIndex), rBlocker, uDirShadowTexel,
                                    searchRadiusTexels, avgBlocker, R);

    if (!hasBlocker)
        return 1.0;

    // 4. Penumbra in World Space
    float depthDiff = rBlocker - avgBlocker;
    float distWS = depthDiff * zRangeWS;
    float penumbraWS = distWS * uDirLightRadiusWS;

    // 5. Convert Penumbra to Texels
    float penumbraTexels = penumbraWS / texelSizeWS;
    penumbraTexels = clamp(penumbraTexels, PCSS_MIN_FILTER, PCSS_MAX_FILTER);

    return pcf2D(uDirectionalLightShadowMapArrayCmp, uv, float(cascadeIndex), rPCF, uDirShadowTexel, penumbraTexels, R);
}

float shadowFactorDirectional(vec3 worldPos, vec3 N, vec3 L)
{
    const float BLEND_RANGE = 0.20; // 20% blend zone near cascade edges

    // Small WS normal offset to reduce self-occlusion
    vec3 wsOffsetPos = worldPos + N * DIR_NORMAL_OFFSET_WS;

    // 1. Find the best cascade
    int primaryCascade = -1;
    float ndcDist = 0.0;

    for (int c = 0; c < DIRECTIONAL_LIGHT_CASCADES; ++c)
    {
        vec4 clip = uDirectionalLightSpaceMatrix[c] * vec4(wsOffsetPos, 1.0);
        if (clip.w <= 0.0)
            continue;

        vec3 ndc = clip.xyz / clip.w;
        if (ndc.x < -1.0 || ndc.x > 1.0 || ndc.y < -1.0 || ndc.y > 1.0 || ndc.z < -1.0 || ndc.z > 1.0)
            continue;

        primaryCascade = c;
        ndcDist = max(max(abs(ndc.x), abs(ndc.y)), abs(ndc.z));
        break;
    }

    if (primaryCascade == -1)
        return 1.0; // outside all cascades

    // 2. Sample the primary cascade
    float shadow = sampleCascadePCSS(primaryCascade, wsOffsetPos, N, L, 0.0);

    // 3. Blend with the next cascade near edges
    int nextCascade = primaryCascade + 1;
    if (nextCascade < DIRECTIONAL_LIGHT_CASCADES && ndcDist > (1.0 - BLEND_RANGE))
    {
        vec4 clip1 = uDirectionalLightSpaceMatrix[nextCascade] * vec4(wsOffsetPos, 1.0);
        if (clip1.w > 0.0)
        {
            vec3 ndc1 = clip1.xyz / clip1.w;
            if (ndc1.x >= -1.0 && ndc1.x <= 1.0 && ndc1.y >= -1.0 && ndc1.y <= 1.0 && ndc1.z >= -1.0 && ndc1.z <= 1.0)
            {
                float shadowNext = sampleCascadePCSS(nextCascade, wsOffsetPos, N, L, 1.0);

                float blendFactor = (ndcDist - (1.0 - BLEND_RANGE)) / BLEND_RANGE;
                blendFactor = clamp(blendFactor, 0.0, 1.0);

                shadow = mix(shadow, shadowNext, blendFactor);
            }
        }
    }

    return shadow;
}

// =====================================================
// SPOT LIGHT — linearized PCSS + RPDB
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

// Blocker search in linear depth (Poisson disk sampling)
bool findBlocker2D_linear(sampler2DArray shad, vec2 uv, float layer, float refLin, vec2 texel, float radiusTexels,
                          out float avgBlockerLin, mat2 R, float n, float f)
{
    float sum = 0.0;
    int cnt = 0;
    vec2 radius = texel * radiusTexels;

    // Use POISSON_16 array (supports up to 16 samples)
    for (int k = 0; k < PCSS_SAMPLES_BLOCKER; ++k)
    {
        vec2 duv = (R * POISSON_16[k]) * radius;
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

// PCF in linear depth (using hardware comparison)
// Note: Hardware comparison works in non-linear depth space, so we pass 'refDepth' (non-linear) directly
float pcf2D_linear(sampler2DArrayShadow shadCmp, vec2 uv, float layer, float refDepth, vec2 texel, float radiusTexels,
                   mat2 R)
{
    vec2 radius = texel * radiusTexels;
    float sum = 0.0;

    // Use POISSON_32 array (supports up to 32 samples)
    // Hardware comparison sampler: each sample gets automatic 2x2 bilinear filtering + depth comparison
    for (int k = 0; k < PCSS_SAMPLES_PCF; ++k)
    {
        vec2 duv = (R * POISSON_32[k]) * radius;
        // texture() with sampler2DArrayShadow returns filtered comparison result (0.0-1.0)
        float s = texture(shadCmp, vec4(uv + duv, layer, refDepth));
        sum += s;
    }
    return sum / float(PCSS_SAMPLES_PCF);
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

    // Receiver-plane depth bias
    float rpdb = rpdbSpot(clip, uSpotShadowTexel);
    rpdb = min(rpdb, 0.001); // Clamp RPDB tightly (0.001)

    // Gentler base & slope bias; RPDB does the heavy lifting
    float baseBias = MIN_BIAS * 0.1 + slopeBias(N, L) * 0.005 + rpdb;

    // Dual Bias Strategy
    float biasPCF = baseBias; // Full bias for PCF to prevent acne
    float biasBlocker = 0.0;  // Zero bias for blocker search to force attachment

    // Apply bias in the same space as the shadow map value
    float rPCF = clamp(ref - biasPCF, 0.0, 1.0);
    float rBlocker = clamp(ref - biasBlocker, 0.0, 1.0);

    // Linearize reference depth for linear-PCSS math (use blocker bias for search)
    float refLinBlocker = linearizeDepth01(rBlocker, SPOT_NEAR, farP);
    float refLinPCF = linearizeDepth01(rPCF, SPOT_NEAR, farP);

    // Per-pixel rotation for Poisson disk samples
    float ang = hash12(gl_FragCoord.xy) * 6.2831853;
    mat2 R = rot2(ang);

    // Make search/filter grow with distance
    // Multiplier 120.0 controls shadow softness - tune range [60.0-200.0] for performance vs quality
    float distScale = clamp(refLinPCF / farP, 0.0, 1.0); // 0 near .. 1 far
    float searchRadius = clamp(uSpotLightRadiusWS * 120.0 * (0.75 + 0.75 * distScale), 1.0, 25.0);

    float avgBlockerLin;
    bool hasBlocker = findBlocker2D_linear(uSpotLightShadowMapArray, uv, float(i), refLinBlocker, uSpotShadowTexel,
                                           searchRadius, avgBlockerLin, R, SPOT_NEAR, farP);
    if (!hasBlocker)
        return 1.0;

    float penumbraRaw = ((refLinPCF - avgBlockerLin) / max(avgBlockerLin, 1e-4)) * uSpotLightRadiusWS * 120.0 *
                        (0.75 + 0.75 * distScale);
    float penumbra = clamp(penumbraRaw, PCSS_MIN_FILTER, PCSS_MAX_FILTER);

    return pcf2D_linear(uSpotLightShadowMapArrayCmp, uv, float(i), rPCF, uSpotShadowTexel, penumbra, R);
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

    // Per-pixel rotation for Poisson disk samples
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

    return pcfCube(uPointLightShadowCubeArrayCmp, dir, float(i), r, texelAngle, penumbra, R);
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
