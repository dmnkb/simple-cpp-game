#pragma once

#include <memory>

#include "core/Core.h"
#include "core/UUID.h"

namespace Engine
{

enum class AssetType
{
    None = 0,
    Mesh,
    Texture,
    Shader,
    Material,
    Scene,
    Size
};

static const std::array<std::string, 7> AssetTypeNames = {
    "None", "Mesh", "Texture", "Shader", "Material", "Scene", "Size",
};

struct AssetMetadata
{
    AssetType type = AssetType::None;
    std::string name = "Unnamed Asset";
    std::filesystem::path path = "Unknown Path"; // Stored as relative path
    UUID uuid = UUID::random();

    // Helper to get absolute path (forward declaration, defined in AssetManager.h)
    std::filesystem::path getAbsolutePath() const;
};

class Asset : public std::enable_shared_from_this<Asset>
{
  public:
    virtual ~Asset() = default;
    AssetMetadata metadata;

    template <typename T>
    Ref<T> as()
    {
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }
};

} // namespace Engine