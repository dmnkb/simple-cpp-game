#include "PanelSceneHierarchy.h"
#include "core/Profiler.h"
#include "imgui.h"
#include "pch.h"
#include "renderer/ClearColor.h"
#include "renderer/RendererAPI.h"
#include "util/MemoryUsage.h"

namespace Engine
{

void PanelSceneHierarchy::render(const Scene& scene)
{
    static bool open = true;
    ImGui::Begin("Hierarchy", &open);

    if (ImGui::TreeNode("Basic trees"))
    {
        for (int i = 0; i < 5; i++)
        {
            // Use SetNextItemOpen() so set the default state of a node to be open. We could
            // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
            if (i == 0)
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);

            // Here we use PushID() to generate a unique base ID, and then the "" used as TreeNode id won't conflict.
            // An alternative to using 'PushID() + TreeNode("", ...)' to generate a unique ID is to use
            // 'TreeNode((void*)(intptr_t)i, ...)', aka generate a dummy pointer-sized value to be hashed. The demo
            // below uses that technique. Both are fine.
            ImGui::PushID(i);
            if (ImGui::TreeNode("", "Child %d", i))
            {
                ImGui::Text("blah blah");
                ImGui::SameLine();
                if (ImGui::SmallButton("button"))
                {
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

} // namespace Engine