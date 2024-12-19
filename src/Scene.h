#pragma once

#include "Light.h"
#include "LightSceneNode.h"
#include "MeshSceneNode.h"
#include "pch.h"

const unsigned int MAX_LIGHTS = 3;

struct SceneData
{
    std::vector<std::shared_ptr<LightSceneNode>> lightSceneNodes;
    std::vector<std::shared_ptr<MeshSceneNode>> meshSceneNodes;
};

class Scene
{
  public:
    /**
     * For more complex scenes, implement a culling stage that processes the scene graph and produces
     * Renderables only for visible objects. This can be integrated with spatial partitioning techniques like BVH or
     * octrees.
     */

    static void update();
    static void addMeshSceneNode(const std::shared_ptr<MeshSceneNode>& node);
    static void addLightSceneNode(const std::shared_ptr<LightSceneNode>& node);

  private:
    static void submitLights();
    static void submitMeshSceneNodes();
};