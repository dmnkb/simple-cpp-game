#pragma once

#include "imgui.h"

#include "assets/Asset.h"
#include "editor/EditorState.h"

namespace Engine
{
struct PanelMaterial
{
    static void render(const Ref<AssetRegistry>& assetRegistry)
    {
        // TODO: Implementation for rendering the Material panel in the editor
        static bool open = true;
        ImGui::Begin("PanelMaterial", &open);

        if (g_editorState.selectedMaterial != UUID::zero())
        {
            const AssetMetadata* meta = assetRegistry->find(g_editorState.selectedMaterial);
            if (meta)
            {
                ImGui::Text("Material: %s", meta->name.c_str());
                ImGui::Text("UUID: %s", meta->uuid.to_string().c_str());
                ImGui::Text("Path: %s", meta->path.string().c_str());
                // Additional material properties can be displayed and edited here
            }
            else
            {
                ImGui::TextDisabled("Selected material not found in registry.");
            }
        }
        else
        {
            ImGui::TextDisabled("No material selected.");
        }

        ImGui::End();
    }
};

} // namespace Engine