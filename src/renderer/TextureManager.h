#pragma once

#include "Texture.h"

namespace Engine
{

class TextureManager
{
  public:
    Ref<Texture> getTextureByPath(const std::string& path)
    {
        auto it = m_textureMap.find(path);
        if (it != m_textureMap.end())
        {
            std::println("[Texture Manager] Texture cache hit for: {}", path);
            return it->second;
        }

        Ref<Texture> texture = CreateRef<Texture>(path);

        if (texture->isLoaded)
        {
            m_textureMap[path] = texture;
            std::println("[Texture Manager] Added texture: {}", path);

            return texture;
        }

        std::println("[WARNING] [getTextureByPath] Texture {} could not be loaded.", path);

        // Return anyways
        return texture;
    }

  private:
    std::unordered_map<std::string, Ref<Texture>> m_textureMap;
};

} // namespace Engine