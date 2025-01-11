#pragma once

#include "Mesh.h"
#include "SceneNode.h"
#include "Shader.h"
#include "TextureManager.h"
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
    Ref<Shader> shader;
    glm::mat4 transform;
};

class MeshSceneNode : public SceneNode
{
  public:
    // Maybe the constructor just takes a file path
    MeshSceneNode(Ref<Mesh> mesh, Ref<Shader> shader = nullptr, const Ref<Texture>& texture = {});

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

    const Renderable& prepareRenderable();

    // FIXME: remove
    const Ref<Texture> getTexture()
    {
        return m_texture;
    }

  private:
    Ref<Mesh> m_mesh;
    Ref<Shader> m_shader;
    Ref<Texture> m_texture;
    std::unordered_set<ERenderFlags> m_renderFlags = {OPAQUE};
};
