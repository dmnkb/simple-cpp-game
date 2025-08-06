#pragma once

#include "Shader.h"
#include "core/Core.h"

namespace Engine
{

class ShaderManager
{
  public:
    Ref<Shader> getShaderByHandle(const std::string& handle) const
    {
        auto it = m_shaderMap.find(handle);
        if (it != m_shaderMap.end())
        {
            std::println("[Shader Manager] Shader cache hit for: {}", handle);
            return it->second;
        }

        return nullptr;
    }

    void addShader(const std::string& handle, Ref<Shader> shader)
    {
        m_shaderMap[handle] = std::move(shader);
        std::println("[Shader Manager] Added shader: {}", handle);
    }

  private:
    std::unordered_map<std::string, Ref<Shader>> m_shaderMap;
};

} // namespace Engine