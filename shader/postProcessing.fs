#version 330 core

in vec2 v_UV;
out vec4 FragColor;

uniform sampler2D renderTargetTexture;

// --- Bloom (original) ---
const float threshold = 1.0;
const float blurRadius = 2.0;
const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

// --- N64-style defaults (no external uniforms) ---
const float PIXEL_SCALE = 2.0;     // 1=no pixelation; use integers (2,3,4…) (Default: 2.0)
const float DITHER_STRENGTH = 0.8; // 0..1 (Default: 0.6)
const float VI_BLUR_AMOUNT = 0.25; // 0..1 (Default: 0.35)
const float GAMMA_IN = 1.5;        // (Default: 2.2)
const float GAMMA_OUT = 1.5;       // (Default: 2.2)
const bool DO_QUANTIZE = true;     // (Default: true)

// ----------------- Helpers -----------------
float bayer4(vec2 ip)
{
    int x = int(mod(ip.x, 4.0));
    int y = int(mod(ip.y, 4.0));
    int m[16] = int[16](0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5);
    float v = float(m[y * 4 + x]) / 15.0;
    return v - 0.5; // -0.5..0.5
}

vec3 toLinear(vec3 c, float g)
{
    return pow(max(c, 0.0), vec3(g));
}
vec3 toGamma(vec3 c, float g)
{
    return pow(max(c, 0.0), vec3(1.0 / g));
}

vec3 quantizeRGB555(vec3 c, float dither, float strength)
{
    const float levels = 31.0; // 5 bits
    vec3 q = floor(c * levels + dither * strength + 0.5) / levels;
    return clamp(q, 0.0, 1.0);
}

// ----------------- Main -----------------
void main()
{
    ivec2 ires = textureSize(renderTargetTexture, 0);
    vec2 res = vec2(ires);
    vec2 texelSz = 1.0 / res;

    // --- Bloom: bright pass + 2D Gaussian-ish blur ---
    vec3 original = texture(renderTargetTexture, v_UV).rgb;
    vec3 brightColor = max(original - threshold, vec3(0.0));

    vec3 blur = brightColor * weight[0];
    for (int x = 1; x < 5; ++x)
    {
        for (int y = 1; y < 5; ++y)
        {
            vec2 offset = vec2(texelSz.x * x * blurRadius, texelSz.y * y * blurRadius);
            blur += texture(renderTargetTexture, v_UV + offset).rgb * weight[x] * weight[y];
            blur += texture(renderTargetTexture, v_UV - offset).rgb * weight[x] * weight[y];
        }
    }

    vec3 composited = original + blur;

    // --- N64-style pixelation + quantized dithering + VI-like blur ---
    float scale = max(PIXEL_SCALE, 1.0);
    vec2 targetRes = res / scale;

    // snap UVs to integer pixel grid at targetRes
    vec2 snappedUV = (floor(v_UV * targetRes) + 0.5) / targetRes;

    // sample composited color at snapped UV
    vec3 src = texture(renderTargetTexture, snappedUV).rgb;

    // linearize
    vec3 lin = toLinear(src, GAMMA_IN);

    // ordered dither using target-res integer coords
    vec2 ip = floor(snappedUV * targetRes);
    float d = bayer4(ip);

    // RGB555 quantization with ordered dither
    vec3 q = DO_QUANTIZE ? quantizeRGB555(lin, d, DITHER_STRENGTH) : lin;

    // VI-like horizontal blur (5-tap [1 3 4 3 1]/12), aligned to pixel grid
    vec2 onePx = vec2(texelSz.x * scale, 0.0);
    vec3 s0 = texture(renderTargetTexture, snappedUV - 2.0 * onePx).rgb;
    vec3 s1 = texture(renderTargetTexture, snappedUV - 1.0 * onePx).rgb;
    vec3 s2 = texture(renderTargetTexture, snappedUV).rgb;
    vec3 s3 = texture(renderTargetTexture, snappedUV + 1.0 * onePx).rgb;
    vec3 s4 = texture(renderTargetTexture, snappedUV + 2.0 * onePx).rgb;
    vec3 blurLin = toLinear((1.0 / 12.0) * (s0 + 3.0 * s1 + 4.0 * s2 + 3.0 * s3 + s4), GAMMA_IN);

    // mix quantized with VI blur in linear space
    vec3 vi = mix(q, blurLin, clamp(VI_BLUR_AMOUNT, 0.0, 1.0));

    // back to display space
    vec3 outColor = toGamma(vi, GAMMA_OUT);

    FragColor = vec4(outColor, 1.0);
}
