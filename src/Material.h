#pragma once
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"

class Material
{
  public:
    Material(Ref<Shader>& shader);

    void bind();
    void unbind();
    void setColorTexture(Ref<Texture>& texture);
    void setColorTextureScale(const int& scale);

    void setUniformMatrix4fv(const char* name, const glm::mat4 value);
    void setUniform3fv(const char* name, const glm::vec3 value);
    void setUniform1i(const char* name, GLint value);

    Ref<Shader> getShader() const;
    Ref<Texture> getColorTexture() const;

    const bool hasUniform(const char* name);

  private:
    Ref<Shader> m_shader = nullptr;
    Ref<Texture> m_colorTexture;

    int m_colorTextureScale = 1;
};