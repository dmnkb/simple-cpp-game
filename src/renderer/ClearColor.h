#pragma once

#include <array>
#include <glad/glad.h>
#include <string>

namespace Engine
{

struct SClearColor
{
    std::array<GLfloat, 4> rgba{0.f, 0.f, 0.f, 1.f};

    // clang-format off
    GLfloat& r() noexcept { return rgba[0]; }
    GLfloat& g() noexcept { return rgba[1]; }
    GLfloat& b() noexcept { return rgba[2]; }
    GLfloat& a() noexcept { return rgba[3]; }
    const GLfloat& r() const noexcept { return rgba[0]; }
    const GLfloat& g() const noexcept { return rgba[1]; }
    const GLfloat& b() const noexcept { return rgba[2]; }
    const GLfloat& a() const noexcept { return rgba[3]; }
    // clang-format on
};

static const std::array<std::string, 6> clearColorTitles = {
    "Charcoal Gray",        "Zelda Windwaker Sky", "Slate Blue-Gray",
    "Desaturated Sky Blue", "Muted Light Blue",    "Dawn/Dusk Mauve",
};

static const std::array<SClearColor, 6> clearColorValues = {
    SClearColor{0.12f, 0.12f, 0.12f, 1.0f}, SClearColor{0.2902f, 0.4196f, 0.9647f, 1.0f},
    SClearColor{0.17f, 0.24f, 0.31f, 1.0f}, SClearColor{0.44f, 0.50f, 0.56f, 1.0f},
    SClearColor{0.66f, 0.79f, 1.00f, 1.0f}, SClearColor{0.43f, 0.50f, 0.61f, 1.0f},
};

} // namespace Engine