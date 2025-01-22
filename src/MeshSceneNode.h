#pragma once

#include "Material.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "pch.h"

enum ERenderFlags
{
    OPAQUE,
    TRANSLUCENT,
    CASTS_SHADOW,
};

// An atomic, stateless POD (plain old data) that contains mesh, material and transform.
struct Renderable
{
    Ref<Mesh> mesh;
    Ref<Material> material;
    glm::mat4 transform;
};

class MeshSceneNode : public SceneNode
{
  public:
    // Maybe the constructor just takes a file path
    MeshSceneNode(Ref<Mesh> mesh, Ref<Material> material = nullptr);

    /**
     * TODO:
     * When traversing the scene graph, ensure that each node produces Renderables efficiently.
     * For dynamic scenes, flag nodes for updates to reduce redundant computations.
     */

    const bool isOpaque() const
    {
        return m_renderFlags.find(OPAQUE) != m_renderFlags.end();
    }

    const bool isTransparent() const
    {
        return m_renderFlags.find(TRANSLUCENT) != m_renderFlags.end();
    }

    const bool isCastsShadow() const
    {
        return m_renderFlags.find(CASTS_SHADOW) != m_renderFlags.end();
    }

    const Renderable prepareRenderable();

  private:
    Ref<Mesh> m_mesh;
    Ref<Material> m_material;
    std::unordered_set<ERenderFlags> m_renderFlags = {OPAQUE};
};
