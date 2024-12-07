#pragma once

#include "Light.h"
#include "pch.h"

const unsigned int MAX_LIGHTS = 3;

class Scene
{
  public:
    Scene();
    void update();

  private:
    std::vector<Light> m_lights;
};