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
    MeshSceneNode(Ref<Mesh> mesh, Ref<Shader> shader = nullptr, const Ref<Texture>& texture = {});

    /**
     * TODO:
     * When traversing the scene graph, ensure that each node produces Renderables efficiently.
     * For dynamic scenes, flag nodes for updates to reduce redundant computations.
     */

    const Renderable prepareRenderable();

    // FIXME: remove
    const Ref<Texture> getTexture()
    {
        return m_texture;
    }

  private:
    Ref<Mesh> m_mesh;
    Ref<Shader> m_shader;
    Ref<Texture> m_texture;
};
