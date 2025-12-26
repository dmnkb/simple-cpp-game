#pragma once

#include "imgui.h"
#include <imgui_internal.h>

#include "core/Profiler.h"
#include "editor/EditorState.h"
#include "renderer/ClearColor.h"
#include "renderer/RendererAPI.h"
#include "scene/Entity.h"
#include "scene/Scene.h"
#include "util/MemoryUsage.h"

namespace Engine
{

struct PanelSceneHierarchy
{
    inline static entt::entity s_selected = entt::null;

    static void drawEntityNode(Entity entity)
    {
        auto& tag = entity.getComponent<TagComponent>().tag;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

        const entt::entity id = entity.getHandle();
        if (s_selected == id) flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)id, flags, "%s", tag.c_str());

        if (ImGui::IsItemClicked())
        {
            s_selected = id;
            g_editorState.selectedEntity = entity;
            g_editorState.selectedMesh = entity.hasComponent<MeshComponent>()
                                             ? entity.getComponent<MeshComponent>().mesh->metadata.uuid
                                             : UUID::zero();
        }
    }

    static void drawNewEntityPopup(const Ref<Scene>& scene)
    {
        if (ImGui::Button("Add Entity"))
        {
            ImGui::OpenPopup("AddEntityPopup");
        }

        if (ImGui::BeginPopup("AddEntityPopup"))
        {
            static char nameBuffer[128] = "New Entity";
            ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer));

            if (ImGui::Button("Create"))
            {
                scene->createEntity(std::string(nameBuffer));
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
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
            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            {
                s_selected = entt::null;
                g_editorState.selectedEntity = {};
            }

            // Open popup to enter entity name
            drawNewEntityPopup(scene);

            // Select first entity by default
            static bool firstFrame = true;
            scene->forEachHierarchyEntity(
                [&](entt::entity entityID)
                {
                    if (firstFrame)
                    {
                        s_selected = entityID;
                        g_editorState.selectedEntity = {entityID, scene.get()};
                        firstFrame = false;
                    }
                    drawEntityNode(Entity{entityID, scene.get()});
                });
        }
        ImGui::EndChild();

        ImGui::End();
    }
};

} // namespace Engine
