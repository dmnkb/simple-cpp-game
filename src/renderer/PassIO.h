
#pragma once

#include "Texture.h"
#include "core/Core.h"

namespace Engine
{

struct ShadowOutputs
{
    Ref<Texture> spotShadowArray;
    Ref<Texture> pointShadowCubeArray;
    Ref<Texture> directionalShadowArray;
    // TODO: per-layer light VP matrices, when lights become PODs without cameras
};

using LightingInputs = ShadowOutputs;

struct LightingOutputs
{
    Ref<Texture> renderTargetTexture;
};

using PostProcessingInputs = LightingOutputs;

} // namespace Engine
