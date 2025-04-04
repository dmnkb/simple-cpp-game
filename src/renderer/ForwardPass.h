#pragma once

#include "LightSceneNode.h"
#include "Material.h"
#include "Scene.h"
#include "pch.h"

class ForwardPass
{
  public:
    ForwardPass();
    ~ForwardPass();
    void execute(Scene& scene);

  private:
    void updateUniforms(Scene& scene, const Ref<Material>& material);

  private:
    LightSceneNode::LightUBO m_lightBuffer[256];
    GLuint m_uboLights;
};