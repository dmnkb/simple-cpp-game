#pragma once

#include "Scene.h"
#include "Shader.h"
#include "pch.h"

class ShadowPass
{
  public:
    ShadowPass();
    void execute(Scene& scene, int& drawCalls);

  private:
    Shader m_depthShader;
};