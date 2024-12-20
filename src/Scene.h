#pragma once

#include "Light.h"
#include "LightSceneNode.h"
#include "MeshSceneNode.h"
#include "pch.h"

const unsigned int MAX_LIGHTS = 3;

using SceneNodeVariant = std::variant<Ref<MeshSceneNode>, Ref<LightSceneNode>>;

struct SceneData
{
    std::vector<Ref<LightSceneNode>> lightSceneNodes;
    std::vector<Ref<MeshSceneNode>> meshSceneNodes;
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
    static void addMeshSceneNode(const Ref<MeshSceneNode>& node);
    static void addLightSceneNode(const Ref<LightSceneNode>& node);

    static std::optional<SceneNodeVariant> getByName(const std::string& name);

  private:
    static void submitLights();
    static void submitMeshSceneNodes();
};