#pragma once

#include "imgui.h"

namespace Engine
{

// https://colorbrewer2.org/#type=qualitative&scheme=Set3&n=12
static const std::array<ImColor, 12> colors = {
    IM_COL32(141, 211, 199, 255), IM_COL32(255, 255, 179, 255), IM_COL32(190, 186, 218, 255),
    IM_COL32(251, 128, 114, 255), IM_COL32(128, 177, 211, 255), IM_COL32(253, 180, 98, 255),
    IM_COL32(179, 222, 105, 255), IM_COL32(252, 205, 229, 255), IM_COL32(217, 217, 217, 255),
    IM_COL32(188, 128, 189, 255), IM_COL32(204, 235, 197, 255), IM_COL32(255, 237, 111, 255),
};

class PanelFrametime
{
  public:
    static void render();
};

} // namespace Engine