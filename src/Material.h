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
    void addTexture(const std::string& uniformName, Ref<Texture> texture);

    void setUniformMatrix4fv(const char* name, const glm::mat4 value);
    void setUniform3fv(const char* name, const glm::vec3 value);
    void setUniform1i(const char* name, GLint value);

    Ref<Shader> getShader() const;
    const bool hasUniform(const char* name);

  private:
    Ref<Shader> m_shader = nullptr;
    std::unordered_map<std::string, Ref<Texture>> m_textures;
    std::vector<Ref<Texture>> m_shadowMaps;
};