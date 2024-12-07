
#pragma once

#include <glm/glm.hpp>

struct Light
{
    glm::vec3 position; // 3 floats, 16 bytes due to std140 padding
    float padding1;     // Padding to align the next vec3
    glm::vec3 color;    // 3 floats, 16 bytes due to std140 padding
    float padding2;     // Padding to align the next vec3 (optional)
};