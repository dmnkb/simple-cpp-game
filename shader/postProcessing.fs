#version 330 core

in vec2 v_UV;
out vec4 FragColor;

uniform sampler2D renderTargetTexture;

const float threshold = 1.0;
const float blurRadius = 4.0;
const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texelSize = 1.0 / textureSize(renderTargetTexture, 0);
    vec3 original = texture(renderTargetTexture, v_UV).rgb;

    // Bright pass threshold
    vec3 brightColor = max(original - threshold, vec3(0.0));

    // Apply 2D Gaussian blur in a single pass
    vec3 blur = brightColor * weight[0];
    for (int x = 1; x < 5; ++x)
    {
        for (int y = 1; y < 5; ++y)
        {
            vec2 offset = vec2(texelSize.x * x * 1.5, texelSize.y * y * 1.5);
            blur += texture(renderTargetTexture, v_UV + offset).rgb * weight[x] * weight[y];
            blur += texture(renderTargetTexture, v_UV - offset).rgb * weight[x] * weight[y];
        }
    }

    vec3 finalColor = original + blur;
    FragColor = vec4(finalColor, 1.0);
}
