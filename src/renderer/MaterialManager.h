#pragma once

#include "Material.h"
#include "pch.h"

class MaterialManager
{
  public:
    Ref<Material> getMaterialByHandle(const std::string& handle) const
    {
        auto it = m_materialMap.find(handle);
        if (it != m_materialMap.end())
        {
            std::println("Material cache hit for: {}", handle);
            return it->second;
        }

        std::cerr << "[MaterialManager] Warning: Material '" << handle << "' not found.\n";
        return nullptr; // or fallbackMaterial
    }

    void addMaterial(const std::string& handle, Ref<Material> material)
    {
        m_materialMap[handle] = std::move(material);
        std::println("Added material: {}", handle);
    }

  private:
    std::unordered_map<std::string, Ref<Material>> m_materialMap;
};
