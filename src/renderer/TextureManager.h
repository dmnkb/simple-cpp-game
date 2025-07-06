#pragma once

#include "Texture.h"
#include "pch.h"

class TextureManager
{
  public:
    Ref<Texture> getTextureByPath(const std::string& path)
    {
        auto it = m_textureMap.find(path);
        if (it != m_textureMap.end())
        {
            std::println("Texture cache hit for: {}", path);
            return it->second;
        }

        Ref<Texture> texture = CreateRef<Texture>(path);
        m_textureMap[path] = texture;
        std::println("Added texture: {}", path);
        return texture;
    }

  private:
    std::unordered_map<std::string, Ref<Texture>> m_textureMap;
};
