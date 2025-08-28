#version 330 core

in vec2 v_UV;
in vec3 v_Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D diffuseMap;

// ---- Shadow inputs (SOFTWARE PCF uses non-comparison samplers) ----
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

// ---- bias knobs (compile-time constants) ----
const float kMinBias = 0.00012;  // tiny constant depth bias
const float kSlopeBias = 0.0016; // scales with (1 - N·L)
const float kNormalBias = 1.25;  // in texels, converted to world-units per light

// ----------------------------------------------------
// N64 3-point texture filter
// ----------------------------------------------------
vec4 N64_3Point_Filter(sampler2D tex, vec2 uv, vec2 texSize)
{
    vec2 texelSize = 1.0 / texSize;
    vec2 half_tex = texelSize * 0.5;
    vec2 UVCentered = uv - half_tex;

    vec2 pixelPos = UVCentered * texSize;
    vec2 texelPos = floor(pixelPos);
    vec2 f = fract(pixelPos);

    if (pixelPos.x < 0.0)
        f.x = 1.0 - f.x;
    if (pixelPos.y < 0.0)
        f.y = 1.0 - f.y;

    vec2 uv00 = (texelPos + vec2(0.0, 0.0)) / texSize;
    vec2 uv10 = (texelPos + vec2(1.0, 0.0)) / texSize;
    vec2 uv01 = (texelPos + vec2(0.0, 1.0)) / texSize;
    vec2 uv11 = (texelPos + vec2(1.0, 1.0)) / texSize;

    vec4 t00 = texture(tex, uv00);
    vec4 t10 = texture(tex, uv10);
    vec4 t01 = texture(tex, uv01);
    vec4 t11 = texture(tex, uv11);

    vec4 c0 = t00 + f.x * (t10 - t00) + f.y * (t01 - t00);
    vec4 c1 = t11 + (1.0 - f.x) * (t01 - t11) + (1.0 - f.y) * (t10 - t11);
    return mix(c0, c1, step(1.0, f.x + f.y));
}

// ----------------------------------------------------
// Helpers to avoid dynamic sampler array indexing on macOS
// ----------------------------------------------------
float SampleDepth(int idx, vec2 uv)
{
    if (idx < 0 || idx >= MAX_SHADOW_MAPS)
        return 1.0;
    float d = 1.0;
    switch (idx)
    {
    case 0:
        d = texture(shadowMaps[0], uv).r;
        break;
    case 1:
        d = texture(shadowMaps[1], uv).r;
        break;
    case 2:
        d = texture(shadowMaps[2], uv).r;
        break;
    case 3:
        d = texture(shadowMaps[3], uv).r;
        break;
    case 4:
        d = texture(shadowMaps[4], uv).r;
        break;
    case 5:
        d = texture(shadowMaps[5], uv).r;
        break;
    case 6:
        d = texture(shadowMaps[6], uv).r;
        break;
    default:
        d = 1.0;
        break;
    }
    return d;
}

// ----------------------------------------------------
// Software 2x2 PCF (bilinear of four comparisons)
// ----------------------------------------------------
float shadowFactor(int lightIndex, vec3 N, vec3 L, vec4 fragPosLightSpace)
{
    if (lightIndex >= MAX_SHADOW_MAPS)
        return 1.0;

    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj = proj * 0.5 + 0.5;

    if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0)
        return 1.0;

    // float ndotl = max(dot(N, L), 0.0);
    // float bias = max(kMinBias, kSlopeBias * (1.0 - ndotl));
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
    float d00 = SampleDepth(lightIndex, base);
    float d10 = SampleDepth(lightIndex, base + vec2(texel.x, 0.0));
    float d01 = SampleDepth(lightIndex, base + vec2(0.0, texel.y));
    float d11 = SampleDepth(lightIndex, base + vec2(texel.x, texel.y));

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

// ----------------------------------------------------
// Spot light shading (Blinn-Phong) with NORMAL-OFFSET
// ----------------------------------------------------
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

void main()
{
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec2 scaledUV = v_UV * textureRepeat;
    vec2 texSize = vec2(textureSize(diffuseMap, 0));
    vec4 albedo = N64_3Point_Filter(diffuseMap, scaledUV, texSize);

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
