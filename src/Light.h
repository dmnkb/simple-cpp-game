
#pragma once

#include <glm/glm.hpp>

enum ELightType
{
    POINT,
    SPOT
    // TODO: DIRECTIONAL
};

struct Light
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};  // 3 floats, 16 bytes due to std140 padding
    float padding1 = 0.0f;                    // Padding to align the next vec3
    glm::vec3 color = {0.0f, 0.0f, 0.0f};     // 3 floats, 16 bytes due to std140 padding
    float padding2 = 0.0f;                    // Padding to align the next vec3
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};  // 3 floats, 16 bytes due to std140 padding
    float padding3 = 0.0f;                    // Padding to align the next vec3
    ELightType lightType = ELightType::POINT; //
    float innerCone = 0.0f;                   // 4 bytes
    float outerCone = 0.0f;                   // 4 bytes
    float padding4 = 0.0f;                    // Padding to align the struct to 16 bytes
};