#pragma once

#include "Light.h"
#include "MeshSceneNode.h"
#include "pch.h"

const unsigned int MAX_LIGHTS = 3;

class Scene
{
  public:
    Scene();

    /**
     * For more complex scenes, implement a culling stage that processes the scene graph and produces
     * Renderables only for visible objects. This can be integrated with spatial partitioning techniques like BVH or
     * octrees.
     */

    void update();

  private:
    std::vector<Light> m_lights;
    std::vector<std::unique_ptr<MeshSceneNode>> m_meshSceneNodes;

    void submitLights();
    void submitMeshSceneNodes();
};