#include <fmt/core.h>

#include "core/Window.h"
#include "pch.h"
#include "renderer/GLDebug.h"
#include "renderer/Material.h"

namespace Engine
{

Material::Material(const Ref<Shader>& shader) : shader(shader) {}

void Material::bind()
{
    shader->bind();

    // Bind textures to their respective slots
    // if (albedo)
    // {
    //     albedo->bind(0);
    //     shader->setUniform1i("uAlbedoMap", 0);
    //     shader->setUniform1i("uHasAlbedoMap", 1);
    // }
    // else
    // {
    //     shader->setUniform1i("uHasAlbedoMap", 0);
    // }

    // if (normal)
    // {
    //     normal->bind(1);
    //     shader->setUniform1i("uNormalMap", 1);
    // }

    // if (roughness)
    // {
    //     roughness->bind(2);
    //     shader->setUniform1i("uRoughnessMap", 2);
    // }

    // if (metallic)
    // {
    //     metallic->bind(3);
    //     shader->setUniform1i("uMetallicMap", 3);
    // }

    // if (ao)
    // {
    //     ao->bind(4);
    //     shader->setUniform1i("uAOMap", 4);
    // }

    // if (emissive)
    // {
    //     emissive->bind(5);
    //     shader->setUniform1i("uEmissiveMap", 5);
    // }

    // // Upload scalar fallbacks
    // shader->setUniform3fv("uBaseColor", baseColor);
    // shader->setUniformFloat("uRoughnessValue", roughnessValue);
    // shader->setUniformFloat("uMetallicValue", metallicValue);
    // shader->setUniformFloat("uAOValue", aoValue);
    // shader->setUniform3fv("uEmissiveColor", emissiveColor);
    // shader->setUniformFloat("uEmissiveIntensity", emissiveIntensity);

    // // UV transform
    // shader->setUniformFloat("uTextureRepeat", textureRepeat);
    // shader->setUniform3fv("uUVOffset", glm::vec3(uvOffset, 0.0f));
}

void Material::unbind()
{
    shader->unbind();

    if (albedo) albedo->unbind(0);
    if (normal) normal->unbind(1);
    if (roughness) roughness->unbind(2);
    if (metallic) metallic->unbind(3);
    if (ao) ao->unbind(4);
    if (emissive) emissive->unbind(5);
}

void Material::update()
{
    float time = Window::getElapsedTime();
    shader->setUniform1f("uTime", time);
}

void Material::assignTexture(const Ref<Texture>& texture, TextureType type)
{
    switch (type)
    {
        case TextureType::Albedo: albedo = texture; break;
        case TextureType::Normal: normal = texture; break;
        case TextureType::Roughness: roughness = texture; break;
        case TextureType::Metallic: metallic = texture; break;
        case TextureType::AO: ao = texture; break;
        case TextureType::Emissive: emissive = texture; break;
    }
}

const Ref<Texture>& Material::getTexture(TextureType type)
{
    switch (type)
    {
        case TextureType::Albedo: return albedo;
        case TextureType::Normal: return normal;
        case TextureType::Roughness: return roughness;
        case TextureType::Metallic: return metallic;
        case TextureType::AO: return ao;
        case TextureType::Emissive: return emissive;
        default: return albedo;
    }
}

void Material::setUniformMatrix4fv(const char* name, const glm::mat4 value)
{
    shader->setUniformMatrix4fv(name, value);
}

void Material::setUniform3fv(const char* name, const glm::vec3 value)
{
    shader->setUniform3fv(name, value);
}

void Material::setUniform4fv(const char* name, const glm::vec4 value)
{
    shader->setUniform4fv(name, value);
}

void Material::setUniform1i(const char* name, GLint value)
{
    shader->setUniform1i(name, value);
}

void Material::setUniform1f(const char* name, float value)
{
    shader->setUniform1f(name, value);
}

void Material::setIntArray(const char* name, GLint* values, GLsizei count)
{
    shader->setIntArray(name, values, count);
}

Ref<Shader> Material::getShader() const
{
    return shader;
}

float Material::getTextureRepeat() const
{
    return textureRepeat;
}

bool Material::hasUniform(const char* name)
{
    return shader->getCachedLocation(name) >= 0;
}

} // namespace Engine
