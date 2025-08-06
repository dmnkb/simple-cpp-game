#pragma once

#include <array>
#include <glad/glad.h>
#include <string>

namespace Engine
{

enum class EClearColor
{
    CharcoalGray,
    WindwakerSky,
    SlateBlueGray,
    DesaturatedSkyBlue,
    MutedLightBlue,
    DawnDuskMauve,
    MaxElements
};

struct SClearColor
{
    GLfloat red = 0.0f;
    GLfloat green = 0.0f;
    GLfloat blue = 0.0f;
    GLfloat alpha = 0.0f;
};

constexpr size_t ClearColorCount = static_cast<size_t>(EClearColor::MaxElements);

static const std::array<std::string, ClearColorCount> clearColorTitles = {
    "Charcoal Gray",        "Zelda Windwaker Sky", "Slate Blue-Gray",
    "Desaturated Sky Blue", "Muted Light Blue",    "Dawn/Dusk Mauve",
};

static const std::array<SClearColor, ClearColorCount> clearColorValues = {
    SClearColor{0.12f, 0.12f, 0.12f, 1.0f}, SClearColor{0.2902f, 0.4196f, 0.9647f, 1.0f},
    SClearColor{0.17f, 0.24f, 0.31f, 1.0f}, SClearColor{0.44f, 0.50f, 0.56f, 1.0f},
    SClearColor{0.66f, 0.79f, 1.00f, 1.0f}, SClearColor{0.43f, 0.50f, 0.61f, 1.0f},
};

} // namespace Engine