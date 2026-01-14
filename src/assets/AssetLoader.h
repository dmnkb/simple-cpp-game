#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>

#include "assets/MeshLoader.h"
#include "core/Core.h"
#include "scene/Scene.h"

namespace Engine
{

// Forward declarations
struct AssetMetadata;
class Texture;
class Shader;
struct Material;

class AssetLoader
{

  public:
    // Generic entry point: dispatch to a specialization
    template <typename T>
    static Ref<T> load(const AssetMetadata& meta)
    {
        // Force a compile error for unsupported asset types
        static_assert(sizeof(T) == 0, "AssetLoader::load not specialized for this type");
        return nullptr;
    }
};

// Template specialization declarations
template <>
Ref<Texture> AssetLoader::load<Texture>(const AssetMetadata& meta);

template <>
Ref<Shader> AssetLoader::load<Shader>(const AssetMetadata& meta);

template <>
Ref<Material> AssetLoader::load<Material>(const AssetMetadata& meta);

template <>
Ref<Mesh> AssetLoader::load<Mesh>(const AssetMetadata& meta);

template <>
Ref<Scene> AssetLoader::load<Scene>(const AssetMetadata& meta);

} // namespace Engine
