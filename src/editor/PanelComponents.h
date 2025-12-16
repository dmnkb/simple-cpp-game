#pragma once

#include "glm/glm.hpp"
#include "imgui.h"
#include <imgui_internal.h>

#include "editor/EditorState.h"
#include "scene/Entity.h"
#include "scene/Scene.h"

namespace Engine
{

static bool DragVec3Row(const char* id, glm::vec3& v, float speed = 0.1f)
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
        bool axisChanged = ImGui::DragFloat(dragLabel.c_str(), &val, speed, 0.0f, 0.0f, "%.2f");

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

        const auto& entity = g_editorState.selectedEntity;

        if (entity.hasComponent<TagComponent>())
        {
            ImGui::Separator();
            ImGui::Text("Tag Component");
            renderTagComponen(entity, scene);
        }

        ImGui::End();
    }
};

} // namespace Engine