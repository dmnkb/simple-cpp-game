#pragma once

#include "Material.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "pch.h"

enum ERenderFlags
{
    ERF_ALL, // Placeholder as it corresponds to simply returning "true" during the filtering stage
    ERF_OPAQUE_ONLY,
    ERF_TRANSLUCENT_ONLY,
    ERF_CASTS_SHADOW,
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
        return m_renderFlags.find(ERF_OPAQUE_ONLY) != m_renderFlags.end();
    }

    const bool isTransparent() const
    {
        return m_renderFlags.find(ERF_TRANSLUCENT_ONLY) != m_renderFlags.end();
    }

    const bool isCastsShadow() const
    {
        return m_renderFlags.find(ERF_CASTS_SHADOW) != m_renderFlags.end();
    }

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

    Ref<Renderable>& getRenderable()
    {
        return m_renderable;
    }

  private:
    void updateTransform();

  private:
    Ref<Mesh> m_mesh;
    Ref<Material> m_material;
    std::unordered_set<ERenderFlags> m_renderFlags = {ERF_ALL};
    Ref<Renderable> m_renderable;
};
