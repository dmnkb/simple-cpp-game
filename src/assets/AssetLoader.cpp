#include "assets/AssetLoader.h"

#include <filesystem>
#include <iostream>

#include "assets/Asset.h"
#include "renderer/Material.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"

namespace Engine
{

// MARK: Texture
template <>
Ref<Texture> AssetLoader::load<Texture>(const AssetMetadata& meta)
{
    if (meta.path.empty())
    {
        std::cerr << "AssetLoader<Texture>: empty path (uuid=" << meta.uuid.to_string() << ")\n";
        return nullptr;
    }

    if (!std::filesystem::exists(meta.path))
    {
        std::cerr << "AssetLoader<Texture>: file not found '" << meta.path.string()
                  << "' (uuid=" << meta.uuid.to_string() << ")\n";
        return nullptr;
    }

    // Your current texture ctor seems to take a filename
    Ref<Texture> tex = CreateRef<Texture>(meta.path.string());
    tex->metadata = meta;
    return tex;
}

// MARK: Shader
template <>
Ref<Shader> AssetLoader::load<Shader>(const AssetMetadata& meta)
{
    if (meta.path.empty())
    {
        std::cerr << "AssetLoader<Shader>: empty path (uuid=" << meta.uuid.to_string() << ")\n";
        return nullptr;
    }

    if (!std::filesystem::exists(meta.path))
    {
        std::cerr << "AssetLoader<Shader>: file not found '" << meta.path.string()
                  << "' (uuid=" << meta.uuid.to_string() << ")\n";
        return nullptr;
    }

    // FIXME: What to do about separate vertex/fragment paths?
    // Currently passing the same path for both - needs proper shader asset format
    std::string pathStr = meta.path.string();
    Ref<Shader> shader = CreateRef<Shader>(pathStr.c_str(), pathStr.c_str());
    shader->metadata = meta;
    return shader;
}

// MARK: Material
template <>
Ref<Material> AssetLoader::load<Material>(const AssetMetadata& meta)
{
    // If you have a material file format, deserialize it here.
    // For now, we create a material with your standard shader as a fallback.
    // This keeps the pipeline working while you build proper serialization.

    // Optional: if you require the file to exist:
    // if (!std::filesystem::exists(meta.path)) { ... return nullptr; }

    Ref<Material> mat = CreateRef<Material>(Shader::getStandardShader());
    mat->metadata = meta;

    // TODO: when you implement MaterialSerializer:
    // MaterialSerializer::Deserialize(*mat, meta.path);

    return mat;
}

} // namespace Engine
