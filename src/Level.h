#pragma once

#include "pch.h"
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Level
{
  public:
    Level();
    std::vector<glm::vec2> getCoordsByTextureFile(const std::string filename);
    std::vector<glm::vec2> getCubesInsideRadius(const glm::vec3 origin, const int radius);
    void update();

  private:
    GLuint m_texture0, m_texture1 = 0;
    std::vector<glm::vec2> m_coords;
};
