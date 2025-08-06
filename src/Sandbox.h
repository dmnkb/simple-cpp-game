#pragma once

#include "Model.h"
#include "Scene.h"
#include "core/Core.h"

namespace Engine
{

/**
 * A demo layer to test implementations with
 */
class Sandbox
{
  public:
    void init(Scene& scene);
    void update(double deltyaTime);

  private:
    Ref<LightSceneNode> m_movingLight;
};

} // namespace Engine