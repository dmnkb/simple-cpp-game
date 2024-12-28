#pragma once

#include "Camera.h"
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"
#include <glad/glad.h>

// TODO: Make this a struct where filter() is public, so that it can be set from outside like:
// RenderPass opaquePass;
// opaquePass.filter = [](const SceneObject& object) {
//     return object.isOpaque();
// };
// RenderPass shadowPass;
// shadowPass.filter = [](const SceneObject& object) {
//     return object.castsShadows();
// };

class RenderPass
{
  public:
    RenderPass(const bool renderToScreen = true);
    void setup() const;
    Texture getResult();

    // TODO:
    // Filter defining what objects this pass should render
    // std::function<bool(const SceneObject&)> filter;

  private:
    bool m_renderToScreen = true;
    Texture m_texture;
    GLuint m_fbo;

    // TODO:
    Ref<Shader> shader = nullptr;
};