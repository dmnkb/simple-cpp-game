#pragma once

#include "Model.h"
#include "Scene.h"
#include "pch.h"

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
