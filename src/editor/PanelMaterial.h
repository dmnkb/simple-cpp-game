#pragma once

#include "imgui.h"

namespace Engine
{
struct PanelMaterial
{
    static void render()
    {
        // TODO: Implementation for rendering the Material panel in the editor
        static bool open = true;
        ImGui::Begin("PanelMaterial", &open);

        ImGui::End();
    }
};

} // namespace Engine