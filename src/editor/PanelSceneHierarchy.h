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
    // Keep selection state here for now
    inline static entt::entity s_selected = entt::null;

    static void DrawEntityNode(Entity entity)
    {
        auto& tag = entity.getComponent<TagComponent>().tag;

        const entt::entity id = entity.getHandle();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

        // If you don't have children yet, make it a leaf:
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (s_selected == id) flags |= ImGuiTreeNodeFlags_Selected;

        // Stable unique ID for ImGui
        ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)id, flags, "%s", tag.c_str());

        if (ImGui::IsItemClicked()) s_selected = id;

        // Context menu (optional)
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))
            {
                // Usually you'd queue this instead of immediate delete while iterating
                // entity.getScene()->destroyEntity(id);
            }
            ImGui::EndPopup();
        }

        // Later, when you have a RelationshipComponent:
        // if (opened) { for (child : children) DrawEntityNode(childEntity); ImGui::TreePop(); }
    }

    static void render(const Ref<Scene>& scene)
    {
        static bool open = true;
        ImGui::Begin("Hierarchy", &open);

        ImGui::BeginChild("##hier_tree", ImVec2(0, 0));
        {
            if (!scene)
            {
                ImGui::TextUnformatted("No scene loaded.");
                ImGui::EndChild();
                ImGui::End();
                return;
            }

            // Click empty space to clear selection
            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) s_selected = entt::null;

            scene->forEachHierarchyEntity([&](entt::entity entityID)
                                          { DrawEntityNode(Entity{entityID, scene.get()}); });
        }
        ImGui::EndChild();

        ImGui::End();
    }
};

} // namespace Engine
