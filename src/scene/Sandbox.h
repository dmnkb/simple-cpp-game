#pragma once

#include "core/Core.h"
#include "renderer/Model.h"
#include "scene/Scene.h"

// TODO: Part of gameplay logic, will go into game/

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
};

} // namespace Engine