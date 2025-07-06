#pragma once

#include "Shader.h"
#include "pch.h"

class ShaderManager
{
  public:
    Ref<Shader> getShaderByHandle(const std::string& handle) const
    {
        auto it = m_shaderMap.find(handle);
        if (it != m_shaderMap.end())
        {
            std::println("Shader cache hit for: {}", handle);
            return it->second;
        }

        std::cerr << "[ShaderManager] Warning: Shader '" << handle << "' not found.\n";
        return nullptr; // Or return a fallback shader if defined
    }

    void addShader(const std::string& handle, Ref<Shader> shader)
    {
        m_shaderMap[handle] = std::move(shader);
        std::println("Added shader: {}", handle);
    }

  private:
    std::unordered_map<std::string, Ref<Shader>> m_shaderMap;
};
