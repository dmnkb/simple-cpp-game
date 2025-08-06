#pragma once

#include "Material.h"
#include "core/Core.h"

namespace Engine
{

class MaterialManager
{
  public:
    Ref<Material> getMaterialByHandle(const std::string& handle) const
    {
        auto it = m_materialMap.find(handle);
        if (it != m_materialMap.end())
        {
            std::println("[Material Manager] Material cache hit for: {}", handle);
            return it->second;
        }

        return nullptr;
    }

    void addMaterial(const std::string& handle, Ref<Material> material)
    {
        m_materialMap[handle] = std::move(material);
        std::println("[Material Manager] Added material: {}", handle);
    }

  private:
    std::unordered_map<std::string, Ref<Material>> m_materialMap;
};

} // namespace Engine