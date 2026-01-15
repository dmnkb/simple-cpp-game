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
        const entt::entity id = entity.getHandle();
        auto& tag = entity.getComponent<TagComponent>().tag;

        ImGui::PushID(static_cast<int>(static_cast<uint32_t>(id)));

        const float rowHeight = ImGui::GetFrameHeight();
        const float buttonWidth = ImGui::CalcTextSize("Edit").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        const float fullWidth = ImGui::GetContentRegionAvail().x;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        
        const bool isSelected = (s_selected == id);

        // Calculate positions
        ImVec2 rowStart = ImGui::GetCursorScreenPos();
        const float labelWidth = fullWidth - buttonWidth - spacing;

        // Draw selectable for the label area only (left side)
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetStyle().ItemSpacing.y));
        
        if (ImGui::Selectable("##entity_row", isSelected, ImGuiSelectableFlags_None, ImVec2(labelWidth, rowHeight)))
        {
            s_selected = id;
            g_editorState.selectedEntity = entity;
        }

        // Draw the entity name text, vertically centered
        {
            const float textHeight = ImGui::GetTextLineHeight();
            const float yOffset = (rowHeight - textHeight) * 0.5f;
            ImGui::SetCursorScreenPos(ImVec2(rowStart.x + ImGui::GetStyle().FramePadding.x, rowStart.y + yOffset));
            ImGui::TextUnformatted(tag.c_str());
        }

        ImGui::PopStyleVar();

        // Position button on the same line, right-aligned
        ImGui::SameLine(fullWidth - buttonWidth);
        ImGui::SetCursorPosY(rowStart.y - ImGui::GetCursorScreenPos().y + ImGui::GetCursorPosY());

        bool requestRemove = false;
        if (ImGui::Button("Edit", ImVec2(buttonWidth, rowHeight)))
        {
            ImGui::OpenPopup("EntityContextMenu");
        }

        if (ImGui::BeginPopup("EntityContextMenu"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                requestRemove = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (requestRemove) entity.destroy();

        ImGui::PopID();
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
