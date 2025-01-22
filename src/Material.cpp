#include "Material.h"
#include <fmt/core.h>

Material::Material(Ref<Shader>& shader) : m_shader(shader) {}

void Material::bind()
{
    m_shader->bind();

    for (const auto& [uniformName, texture] : m_textures)
    {
        texture->bind(0);
        m_shader->setUniform1i(uniformName.c_str(), 0);
    }
}

void Material::unbind()
{
    m_shader->unbind();

    for (const auto& [uniformName, texture] : m_textures)
    {
        texture->unbind();
    }
}

void Material::addTexture(const std::string& uniformName, Ref<Texture> texture)
{
    m_textures[uniformName] = texture;
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

const bool Material::hasUniform(const char* name)
{
    return m_shader->hasUniform(name);
}