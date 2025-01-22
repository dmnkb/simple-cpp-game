#include "Material.h"
#include <fmt/core.h>

Material::Material(Ref<Shader>& shader) : m_shader(shader) {}

void Material::bind()
{
    m_shader->bind();
    // 0 = Default color layer across all fragment shaders
    m_colorTexture->bind(0);
    m_shader->setUniform1i("colorMap", 0);
    m_shader->setUniformFloat("colorMapScale", m_colorTextureScale);
}

void Material::unbind()
{
    m_shader->unbind();
    m_colorTexture->unbind();
}

void Material::setColorTexture(Ref<Texture>& texture)
{
    m_colorTexture = texture;
}

void Material::setColorTextureScale(const int& scale)
{
    m_colorTextureScale = scale;
}

void Material::setUniformMatrix4fv(const char* name, const glm::mat4 value)
{
    m_shader->setUniformMatrix4fv(name, value);
}

void Material::setUniform3fv(const char* name, const glm::vec3 value)
{
    m_shader->setUniform3fv(name, value);
}

void Material::setUniform1i(const char* name, GLint value)
{
    m_shader->setUniform1i(name, value);
}

Ref<Shader> Material::getShader() const
{
    return m_shader;
}

Ref<Texture> Material::getColorTexture() const
{
    return m_colorTexture;
}

const bool Material::hasUniform(const char* name)
{
    return m_shader->hasUniform(name);
}