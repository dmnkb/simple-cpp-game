
#pragma once

#include "Texture.h"

namespace Engine
{

struct ShadowOutputs
{
    Ref<Texture> spotShadowArray;      // GL_TEXTURE_2D_ARRAY (depth)
    Ref<Texture> spotShadowArrayDebug; // optional
    int layers = 0;
    int resolution = 0;
    // TODO: per-layer light VP matrices if needed
};

struct LightingInputs
{
    ShadowOutputs shadows;
    // add other inputs: gbuffer, skybox, IBL, etc.
};

} // namespace Engine
