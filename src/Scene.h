#pragma once

#include "Light.h"
#include "MeshSceneNode.h"
#include "pch.h"

const unsigned int MAX_LIGHTS = 3;

struct SceneData
{
    std::vector<Light> m_lights;
    std::vector<std::unique_ptr<MeshSceneNode>> m_meshSceneNodes;
};

class Scene
{
  public:
    /**
     * For more complex scenes, implement a culling stage that processes the scene graph and produces
     * Renderables only for visible objects. This can be integrated with spatial partitioning techniques like BVH or
     * octrees.
     */

    static void init();
    static void update();

  private:
    static void submitLights();
    static void submitMeshSceneNodes();
};