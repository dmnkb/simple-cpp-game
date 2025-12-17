#pragma once

#include "glm/glm.hpp"
#include "imgui.h"
#include <algorithm>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <imgui_internal.h>

#include "editor/EditorState.h"
#include "scene/Entity.h"
#include "scene/Scene.h"

namespace Engine
{

static bool DragVec3Row(const char* id, glm::vec3& v, bool allowNegative = true, bool allowZero = true,
                        float speed = 0.1f)
{
    bool changed = false;

    ImGuiStyle& style = ImGui::GetStyle();
    const float avail = ImGui::GetContentRegionAvail().x;

    ImGui::PushID(id);

    const float widthLabel = ImGui::CalcTextSize("X").x + ImGui::CalcTextSize("Y").x + ImGui::CalcTextSize("Z").x;
    const float widthFull = avail - widthLabel - 3.0f * style.ItemInnerSpacing.x - 2.0f * style.ItemSpacing.x;

    ImGui::PushMultiItemsWidths(3, widthFull);

    auto Axis = [&](const char* label, float& val)
    {
        ImGui::AlignTextToFramePadding();

        const ImVec4 col = (label[0] == 'X')   ? ImVec4(0.8f, 0.1f, 0.15f, 1.0f)
                           : (label[0] == 'Y') ? ImVec4(0.2f, 0.7f, 0.2f, 1.0f)
                                               : ImVec4(0.1f, 0.25f, 0.8f, 1.0f);
        ImGui::TextColored(col, "%s", label);

        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);

        std::string dragLabel = "##" + std::string(id) + std::string(label);

        float min = allowNegative ? -FLT_MAX : allowZero ? 0.0f : 0.01f;
        float max = FLT_MAX;

        bool axisChanged = ImGui::DragFloat(dragLabel.c_str(), &val, speed, min, max, "%.2f");

        ImGui::SameLine(0.0f, style.ItemSpacing.x);
        ImGui::PopItemWidth();
        return axisChanged;
    };

    changed |= Axis("X", v.x);
    changed |= Axis("Y", v.y);
    changed |= Axis("Z", v.z);

    ImGui::NewLine();

    ImGui::PopID();
    return changed;
}

static void renderTagComponen(Entity entity, const Ref<Scene>& scene)
{
    auto& tag = entity.getComponent<TagComponent>().tag;

    char buffer[256];
    std::strncpy(buffer, tag.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0;

    if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
    {
        tag = std::string(buffer);
    }
}

static void renderTransformComponent(Entity entity, const Ref<Scene>& scene)
{
    auto& transform = entity.getComponent<TransformComponent>();

    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody))
    {
        const float availableWidth = ImGui::GetContentRegionAvail().x;
        ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.3f);
        ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Translation");
        ImGui::TableSetColumnIndex(1);
        DragVec3Row("Translation", transform.translation);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Rotation");
        ImGui::TableSetColumnIndex(1);
        DragVec3Row("Rotation", transform.rotation);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Scale");
        ImGui::TableSetColumnIndex(1);
        // Disable negative scale
        DragVec3Row("Scale", transform.scale, false, false);
        ImGui::EndTable();
    }
}

static void drawNewComponentPopup(Entity entity, const Ref<Scene>& scene)
{
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        // Tree View for components
        if (ImGui::TreeNodeEx("Lights", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (!entity.hasComponent<PointLightComponent>())
            {
                if (ImGui::MenuItem("Point Light Component"))
                {
                    entity.addComponent<PointLightComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }
            if (!entity.hasComponent<SpotLightComponent>())
            {
                if (ImGui::MenuItem("Spot Light Component"))
                {
                    entity.addComponent<SpotLightComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }
            if (!entity.hasComponent<DirectionalLightComponent>())
            {
                if (ImGui::MenuItem("Directional Light Component"))
                {
                    entity.addComponent<DirectionalLightComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (!entity.hasComponent<MeshComponent>())
            {
                if (ImGui::MenuItem("Mesh Component"))
                {
                    entity.addComponent<MeshComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }
            // if (!entity.hasComponent<MaterialComponent>())
            // {
            //     if (ImGui::MenuItem("Material Component"))
            //     {
            //         entity.addComponent<MaterialComponent>();
            //         ImGui::CloseCurrentPopup();
            //     }
            // }
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Physics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TreePop();
        }

        ImGui::EndPopup();
    }
}

struct PanelComponents
{
    static void render(const Ref<Scene>& scene)
    {
        static bool open = true;
        ImGui::Begin("Components", &open);

        if (!g_editorState.selectedEntity)
        {
            ImGui::TextUnformatted("No entity selected.");
            ImGui::End();
            return;
        }

        drawNewComponentPopup(g_editorState.selectedEntity, scene);

        const auto& entity = g_editorState.selectedEntity;

        if (entity.hasComponent<TagComponent>())
        {
            ImGui::Separator();
            ImGui::Text("Tag Component");
            renderTagComponen(entity, scene);
        }

        if (entity.hasComponent<TransformComponent>())
        {
            ImGui::Separator();
            ImGui::Text("Transform Component");
            renderTransformComponent(entity, scene);
        }

        ImGui::End();
    }
};

} // namespace Engine