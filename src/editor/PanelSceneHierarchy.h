#pragma once

#include "imgui.h"
#include <imgui_internal.h>

#include "core/Profiler.h"
#include "editor/Shared.h"
#include "renderer/ClearColor.h"
#include "renderer/RendererAPI.h"
#include "scene/Entity.h"
#include "scene/Scene.h"
#include "util/MemoryUsage.h"
#include "util/uuid.h"

namespace Engine
{

struct PanelSceneHierarchy
{
    static void render(const Ref<Scene>& scene)
    {
        // ImGui::ShowDemoWindow();
        // ImGui::ShowStyleEditor();
        constexpr float footerHeight = 220.0f;
        const float availableWidth = ImGui::GetContentRegionAvail().x;

        static bool open = true;
        static int itemClicked = -1;
        static std::unordered_map<int, UUID> lightDict = {};

        ImGui::Begin("Hierarchy", &open);
        ImGui::BeginChild("##hier_tree", ImVec2(0, -footerHeight));
        {
            if (!scene)
            {
                ImGui::Text("No scene loaded.");
                ImGui::EndChild();
                ImGui::End();
                return;
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Entities"))
            {
                // scene->eachEntity(
                //     [&](Entity entity)
                //     {
                //         const auto& tag = entity.getComponent<TagComponent>().tag;
                //         ImGuiTreeNodeFlags nodeFlags =
                //             ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
                //         if (itemClicked == static_cast<int>(entity)) nodeFlags |= ImGuiTreeNodeFlags_Selected;

                //         bool isNodeOpen = ImGui::TreeNodeEx((void*)(uintptr_t)(static_cast<uint32_t>(entity)),
                //                                             nodeFlags, "%s", tag.c_str());
                //         if (ImGui::IsItemClicked())
                //         {
                //             itemClicked = static_cast<int>(entity);
                //         }

                //         if (isNodeOpen)
                //         {
                //             ImGui::TreePop();
                //         }
                //     });
                ImGui::TreePop();
            }
        }
        ImGui::EndChild();

        ImGui::Separator();

        ImGui::End();
    }
};
} // namespace Engine