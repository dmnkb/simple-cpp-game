#include "Material.h"
#include "core/Window.h"
#include "pch.h"
#include <fmt/core.h>

namespace Engine
{

Material::Material(Ref<Shader>& shader, MaterialProps props) : m_shader(shader), m_props(props)
{
    // Create and bind the Uniform Buffer Object
    glGenBuffers(1, &m_uboMaterial);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboMaterial);

    // Allocate memory for the UBO (empty for now)
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialProps), nullptr, GL_DYNAMIC_DRAW);

    // Bind the UBO to the binding point
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboMaterial); // Binding point 0
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Material::bind()
{
    m_shader->bind();

    // Bind the diffuse map sampler
    // 0 = Default color layer across all fragment shaders
    m_diffuseMap->bind(0);
    m_shader->setUniform1i("diffuseMap", 0);

    // Bind the material's properties
    GLuint uboBindingPoint = 1; // 0 = LightsBlock, 1 = MaterialPropsBlock
    GLuint blockIndex = glGetUniformBlockIndex(m_shader->getProgramID(), "MaterialPropsBlock");
    if (blockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(m_shader->getProgramID(), blockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, m_uboMaterial);

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboMaterial);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialProps), &m_props);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void Material::unbind()
{
    m_shader->unbind();
    m_diffuseMap->unbind();
}

void Material::update()
{
    // FIXME: dependency inversion violation. Resolve dependency to Window
    float time = Window::getElapsedTime();
    m_shader->setUniform1f("u_Time", time);
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

} // namespace Engine