#pragma once

#include "Mesh.h"
#include "Renderer.h"
#include "SceneNode.h"
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"

class MeshSceneNode : public SceneNode
{
  public:
    // Maybe the constructor just takes a file path
    MeshSceneNode(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader = nullptr,
                  const Texture& texture = Texture{});

    /**
     * TODO:
     * When traversing the scene graph, ensure that each node produces Renderables efficiently.
     * For dynamic scenes, flag nodes for updates to reduce redundant computations.
     */

    const Renderable prepareRenderable();

    // FIXME: remove
    const Texture getTexture()
    {
        return m_texture;
    }

  private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Shader> m_shader;
    Texture m_texture;
};
