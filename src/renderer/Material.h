#pragma once

#include "assets/Asset.h"
#include "core/Core.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"

namespace Engine
{

enum class TextureType
{
    Albedo = 0,
    Normal = 1,
    Roughness = 2,
    Metallic = 3,
    AO = 4,
    Emissive = 5
};

enum class AlphaMode
{
    Opaque,
    Mask,
    Blend
};

struct Material : public Asset
{
    // Identity
    AssetMetadata metadata{.type = AssetType::Material, .name = "Unnamed Material"};

    // Render state
    bool isDoubleSided = false;
    AlphaMode alphaMode = AlphaMode::Opaque;
    float alphaCutoff = 0.5f;

    // UV transform
    float textureRepeat = 1.0f;
    glm::vec2 uvOffset = glm::vec2(0.0f);

    // Shader
    Ref<Shader> shader = nullptr;

    // PBR Textures
    Ref<Texture> albedo;
    Ref<Texture> normal;
    Ref<Texture> roughness;
    Ref<Texture> metallic;
    Ref<Texture> ao;
    Ref<Texture> emissive;

    // PBR Scalar fallbacks (used when texture is null)
    glm::vec3 baseColor = glm::vec3(1.0f);
    float roughnessValue = 0.5f;
    float metallicValue = 0.0f;
    float aoValue = 1.0f;
    glm::vec3 emissiveColor = glm::vec3(0.0f);
    float emissiveIntensity = 1.0f;

    // Constructor
    Material(const Ref<Shader>& shader);

    // Binding
    void bind();
    void unbind();
    void update();

    // Texture management
    void assignTexture(const Ref<Texture>& texture, TextureType type = TextureType::Albedo);
    const Ref<Texture>& getTexture(TextureType type = TextureType::Albedo);

    // Uniform setters (pass-through to shader)
    // TODO: Decide if we simply want to expose the Shader directly instead
    void setUniformMatrix4fv(const char* name, const glm::mat4 value);
    void setUniform3fv(const char* name, const glm::vec3 value);
    void setUniform4fv(const char* name, const glm::vec4 value);
    void setUniform1i(const char* name, GLint value);
    void setUniform1f(const char* name, float value);
    void setIntArray(const char* name, GLint* values, GLsizei count);
    bool hasUniform(const char* name);

    // Getters
    Ref<Shader> getShader() const;
    float getTextureRepeat() const;
};

} // namespace Engine