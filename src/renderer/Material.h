#pragma once

#include "core/Core.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"

namespace Engine
{

// Uniform buffer ready
struct MaterialProps
{
    float textureRepeat = 1;
    float shininess = 16.0f;
    float specularIntensity = 0.5f;
};

class Material
{
  public:
    std::string name;
    bool isDoubleSided = false;

    Material(Ref<Shader>& shader, MaterialProps props = MaterialProps());

    void bind();
    void unbind();

    void update();

    void setDiffuseMap(Ref<Texture>& texture);
    const Ref<Texture>& getDiffuseMap();
    void setTextureRepeat(const int& repeat);

    void setUniformMatrix4fv(const char* name, const glm::mat4 value);
    void setUniform3fv(const char* name, const glm::vec3 value);
    void setUniform4fv(const char* name, const glm::vec4 value);
    void setUniform1i(const char* name, GLint value);
    void setUniform1f(const char* name, float value);
    void setIntArray(const char* name, GLint* values, GLsizei count);

    Ref<Shader> getShader() const;
    [[nodiscard]] float getTextureRepeat() const;

    const bool hasUniform(const char* name);

  private:
    Ref<Shader> m_shader = nullptr;
    Ref<Texture> m_diffuseMap;

    MaterialProps m_props;
    GLuint m_uboMaterial;
};

} // namespace Engine