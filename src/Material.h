#pragma once

#include "Shader.h"
#include "Texture.h"
#include "pch.h"

struct MaterialProps
{
    float textureRepeat = 1;
    float shininess = 32.0f;
    float specularIntensity = 0.5f;
};

class Material
{
  public:
    Material(Ref<Shader>& shader, MaterialProps props = MaterialProps());

    void bind();
    void unbind();
    void setDiffuseMap(Ref<Texture>& texture);
    void setTextureRepeat(const int& repeat);

    void setUniformMatrix4fv(const char* name, const glm::mat4 value);
    void setUniform3fv(const char* name, const glm::vec3 value);
    void setUniform1i(const char* name, GLint value);

    Ref<Shader> getShader() const;

    const bool hasUniform(const char* name);

  private:
    Ref<Shader> m_shader = nullptr;
    Ref<Texture> m_diffuseMap;

    MaterialProps m_props;
    GLuint m_uboMaterial;
};