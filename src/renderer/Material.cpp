#include <fmt/core.h>

#include "core/Window.h"
#include "pch.h"
#include "renderer/Material.h"

namespace Engine
{

Material::Material(Ref<Shader>& shader, MaterialProps props) : m_shader(shader), m_props(props)
{
    glGenBuffers(1, &m_uboMaterial);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboMaterial);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialProps), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uboMaterial); // binding = 1 for MaterialPropsBlock
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Material::bind()
{
    m_shader->bind();

    if (m_diffuseMap)
    {
        m_diffuseMap->bind(0);
        m_shader->setUniform1i("uDiffuseMap", 0);
    }

    // Bind & upload material UBO to binding point 1
    // FIXME: Fuck this, the binding points need to be aligned somehow. Too much randomness
    GLuint uboBindingPoint = 3;
    GLuint prog = m_shader->getProgramID();
    GLuint blockIndex = glGetUniformBlockIndex(prog, "MaterialPropsBlock");
    if (blockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(prog, blockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, m_uboMaterial);

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboMaterial);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialProps), &m_props);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void Material::unbind()
{
    m_shader->unbind();
    if (m_diffuseMap)
        m_diffuseMap->unbind(0);
}

void Material::update()
{
    float time = Window::getElapsedTime();
    m_shader->setUniform1f("uTime", time);
}

void Material::setDiffuseMap(Ref<Texture>& texture)
{
    m_diffuseMap = texture;
}

const Ref<Texture>& Material::getDiffuseMap()
{
    return m_diffuseMap;
}

void Material::setTextureRepeat(const int& repeat)
{
    m_props.textureRepeat = repeat;
}

void Material::setUniformMatrix4fv(const char* name, const glm::mat4 value)
{
    m_shader->setUniformMatrix4fv(name, value);
}

void Material::setUniform3fv(const char* name, const glm::vec3 value)
{
    m_shader->setUniform3fv(name, value);
}

void Material::setUniform4fv(const char* name, const glm::vec4 value)
{
    m_shader->setUniform4fv(name, value);
}

void Material::setUniform1i(const char* name, GLint value)
{
    m_shader->setUniform1i(name, value);
}

void Material::setUniform1f(const char* name, float value)
{
    m_shader->setUniform1f(name, value);
}

void Material::setIntArray(const char* name, GLint* values, GLsizei count)
{
    m_shader->setIntArray(name, values, count);
}

Ref<Shader> Material::getShader() const
{
    return m_shader;
}

float Material::getTextureRepeat() const
{
    return m_props.textureRepeat;
}

const bool Material::hasUniform(const char* name)
{
    // True only if the uniform exists (location >= 0)
    return m_shader->getCachedLocation(name) >= 0;
}

} // namespace Engine
