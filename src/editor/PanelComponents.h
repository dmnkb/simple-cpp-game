#pragma once

#include "glm/glm.hpp"
#include "imgui.h"
#include <algorithm>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <imgui_internal.h>

#include "assets/AssetManager.h"
#include "assets/AssetRegistry.h"
#include "editor/EditorState.h"
#include "scene/Entity.h"
#include "scene/Scene.h"

namespace Engine
{

// MARK: Helpers
template <typename T>
void drawComponentUI(const char* name, Entity entity, std::function<void(T&)> uiFunction)
{
    if (!entity.hasComponent<T>()) return;

    ImGui::PushID((int)typeid(T).hash_code());

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
                               ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;

    bool opened = ImGui::TreeNodeEx("##component_header", flags, "%s", name);

    float buttonWidth = ImGui::CalcTextSize("Edit").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    float rightEdge = ImGui::GetWindowContentRegionMax().x;

    ImGui::SameLine();
    ImGui::SetCursorPosX(rightEdge - buttonWidth);

    if (ImGui::Button("Edit")) ImGui::OpenPopup("ComponentSettings");

    bool requestRemove = false;
    bool requestWarn = false;

    if (ImGui::BeginPopup("ComponentSettings"))
    {
        if (ImGui::MenuItem("Remove Component"))
        {
            if constexpr (std::is_same_v<T, TransformComponent> || std::is_same_v<T, TagComponent>) requestWarn = true;
            else requestRemove = true;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (requestWarn) ImGui::OpenPopup("Remove Component Warning");

    if (ImGui::BeginPopupModal("Remove Component Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Cannot remove essential component!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    // Important: remove AFTER popups are handled, and not while drawing UI for it
    if (requestRemove && entity.hasComponent<T>()) entity.removeComponent<T>();

    if (opened)
    {
        // If it was removed this frame, avoid touching it
        if (entity.hasComponent<T>())
        {
            auto& component = entity.getComponent<T>();
            uiFunction(component);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}

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

static void renderComponentSelectionPopup(Entity entity, const Ref<Scene>& scene)
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
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Physics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TreePop();
        }

        ImGui::EndPopup();
    }
}

// MARK: Tag Component
static void renderTagComponent(Entity entity, const Ref<Scene>& scene)
{
    auto& tag = entity.getComponent<TagComponent>().tag;

    char buffer[256];
    std::strncpy(buffer, tag.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0;

    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody))
    {
        const float availableWidth = ImGui::GetContentRegionAvail().x;
        ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.3f);
        ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Tag");
        ImGui::TableSetColumnIndex(1);
        const float avail = ImGui::GetContentRegionAvail().x;
        ImGui::PushItemWidth(avail);
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) tag = std::string(buffer);
        ImGui::PopItemWidth();
        ImGui::EndTable();
    }
}

// MARK: Transform Component
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
        DragVec3Row("Scale", transform.scale, false, false);
        ImGui::EndTable();
    }
}

// MARK: Mesh Component
static void renderMeshComponent(Entity entity, const Ref<Scene>& scene)
{
    ImGui::Button("Assign Mesh...");
    if (ImGui::IsItemClicked())
    {
        ImGui::OpenPopup("AssignMeshPopup");
    }
    if (ImGui::BeginPopup("AssignMeshPopup", ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Select Mesh to Assign:");
        ImGui::Separator();

        AssetRegistry::forEachByType(AssetType::Mesh,
                                     [&](UUID id, const AssetMetadata& meta)
                                     {
                                         if (ImGui::Selectable(meta.name.c_str()))
                                         {
                                             std::println("Assigning mesh {}", meta.name.c_str());

                                             Ref<Mesh> mesh = AssetManager::getOrImport<Mesh>(id);

                                             auto& meshComp = entity.getComponent<MeshComponent>();
                                             meshComp.mesh = mesh;

                                             meshComp.overrideMaterials.resize(mesh->defaultMaterialSlots.size());

                                             ImGui::CloseCurrentPopup();
                                         }
                                     });

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    auto& meshComp = entity.getComponent<MeshComponent>();
    if (!meshComp.mesh)
    {
        ImGui::TextUnformatted("No mesh assigned.");
        return;
    }

    ImGui::Text("Path: %s", meshComp.mesh->metadata.path.c_str());
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    if (ImGui::BeginTable("MaterialSlotTable", 4,
                          ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_BordersInnerH))
    {
        const float indexNumberWidth = ImGui::CalcTextSize("42").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        const float assignButtonWidth = ImGui::CalcTextSize("Assign...").x + ImGui::GetStyle().FramePadding.x * 2.0f;

        ImGui::TableSetupColumn("Slot", ImGuiTableColumnFlags_WidthFixed, indexNumberWidth);
        ImGui::TableSetupColumn("Default Material", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Override Material", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, assignButtonWidth);

        ImGui::TableHeadersRow();

        for (uint32_t i = 0; i < meshComp.mesh->defaultMaterialSlots.size(); ++i)
        {
            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(i)); // Unique ID scope per slot row

            // Column 0: Slot index
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%u", i);

            // Column 1: Default material name
            ImGui::TableSetColumnIndex(1);
            ImGui::AlignTextToFramePadding();
            if (i < meshComp.mesh->defaultMaterialSlots.size() && meshComp.mesh->defaultMaterialSlots[i])
                ImGui::TextUnformatted(meshComp.mesh->defaultMaterialSlots[i]->metadata.name.c_str());
            else ImGui::TextUnformatted("None");

            // Column 2: Override material name
            // TODO: Add remove button
            ImGui::TableSetColumnIndex(2);
            ImGui::AlignTextToFramePadding();
            const bool hasOverride = (i < meshComp.overrideMaterials.size() && meshComp.overrideMaterials[i]);
            ImGui::TextUnformatted(hasOverride ? meshComp.overrideMaterials[i]->metadata.name.c_str() : "None");

            // Column 3: Actions
            ImGui::TableSetColumnIndex(3);

            // Use a stable popup name, scoped by PushID(i)
            if (ImGui::Button("Assign..."))
            {
                ImGui::OpenPopup("AssignMaterialPopup");
            }

            if (ImGui::BeginPopup("AssignMaterialPopup", ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextUnformatted("Select Material to Assign:");
                ImGui::Separator();

                AssetRegistry::forEachByType(AssetType::Material,
                                             [&](UUID id, const AssetMetadata& meta)
                                             {
                                                 // Ensure unique IDs even if meta.name duplicates
                                                 ImGui::PushID(id.to_string().c_str());

                                                 if (ImGui::Selectable(meta.name.c_str()))
                                                 {
                                                     // Ensure vector is large enough
                                                     if (meshComp.overrideMaterials.size() <= i)
                                                         meshComp.overrideMaterials.resize(i + 1);

                                                     meshComp.overrideMaterials[i] =
                                                         AssetManager::getOrImport<Material>(id);
                                                     ImGui::CloseCurrentPopup();
                                                 }

                                                 ImGui::PopID();
                                             });

                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    if (ImGui::TreeNode("##SubmeshInfo", "Debug"))
    {
        if (ImGui::TreeNodeEx("##SubmeshList", ImGuiTreeNodeFlags_None, "Total Submeshes: %d",
                              (int)meshComp.mesh->submeshes.size()))
        {
            for (uint32_t i = 0; i < meshComp.mesh->submeshes.size(); ++i)
            {
                ImGui::BulletText("Submesh %d: %d indices", i, meshComp.mesh->submeshes[i].indexCount);
            }
            ImGui::TreePop();
        }
        ImGui::Text("Total Vertices: %d", (int)meshComp.mesh->vertices.size());
        ImGui::Text("Total Indices: %d", (int)meshComp.mesh->indices.size());
        ImGui::TreePop();
    }
}

struct PanelComponents
{
    static void render(const Ref<Scene>& scene)
    {
        static bool open = true;
        ImGui::Begin("Components", &open);

        renderComponentSelectionPopup(g_editorState.selectedEntity, scene);

        if (!g_editorState.selectedEntity)
        {
            ImGui::TextUnformatted("No entity selected.");
            ImGui::End();
            return;
        }

        Entity& entity = g_editorState.selectedEntity;

        // Tag Component
        drawComponentUI<TagComponent>("Tag Component", entity,
                                      [&](TagComponent& component) { renderTagComponent(entity, scene); });

        // Transform Component
        drawComponentUI<TransformComponent>("Transform Component", entity, [&](TransformComponent& component)
                                            { renderTransformComponent(entity, scene); });

        // Mesh Component
        std::string meshComponentHeader = "Mesh Component";
        if (entity.hasComponent<MeshComponent>())
        {
            auto& meshComp = entity.getComponent<MeshComponent>();
            meshComponentHeader =
                std::format("Mesh Component ({})", meshComp.mesh ? meshComp.mesh->metadata.name.c_str() : "No Mesh");
        }
        drawComponentUI<MeshComponent>(meshComponentHeader.c_str(), entity,
                                       [&](MeshComponent& component) { renderMeshComponent(entity, scene); });

        ImGui::End();
    }
};

} // namespace Engine