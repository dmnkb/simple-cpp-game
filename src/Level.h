#pragma once

#include "pch.h"
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct PairHash
{
    template <typename T1, typename T2> std::size_t operator()(const std::pair<T1, T2>& pair) const
    {
        std::hash<T1> hasher1;
        std::hash<T2> hasher2;
        return hasher1(pair.first) ^ (hasher2(pair.second) << 1);
    }
};

class Level
{
  public:
    Level();
    std::optional<std::vector<glm::vec2>> getCoordsByTextureFile(const std::string filename);
    std::vector<glm::vec2> getCubesInCell(const glm::vec2 origin);
    int getCellSize()
    {
        return m_cellSize;
    }
    void update();

  private:
    GLuint m_texture0, m_texture1 = 0;
    std::vector<glm::vec2> m_coords;
    int m_cellSize = 16;
    std::unordered_map<std::pair<int, int>, std::vector<glm::vec2>, PairHash> m_CoordsMap;
};
