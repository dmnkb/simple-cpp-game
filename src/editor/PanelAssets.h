#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include "assets/AssetManager.h"
#include "assets/AssetRegistry.h"
#include "core/UUID.h"
#include "editor/EditorState.h"
#include "util/FileDialog.h" // <-- adjust to your actual path

namespace Engine
{

struct PanelAssets
{
    // Configuration (could be moved to EditorState if needed)
    static inline float IconSize = 96.0f;
    static inline int IconSpacing = 10;
    static inline int IconHitSpacing = 4;

    // Selection state - uses ImGui's selection storage with index as key
    static inline ImGuiSelectionBasicStorage Selection;
    static inline AssetType CurrentAssetType = AssetType::Material;

    struct LayoutState
    {
        ImVec2 ItemSize;
        ImVec2 ItemStep;
        float ItemSpacing = 0.0f;
        float SelectableSpacing = 0.0f;
        float OuterPadding = 0.0f;
        int ColumnCount = 0;
        int LineCount = 0;
    };

    static void UpdateLayoutSizes(LayoutState& layout, float availWidth, int itemCount)
    {
        layout.ItemSpacing = static_cast<float>(IconSpacing);
        layout.ItemSize = ImVec2(floorf(IconSize), floorf(IconSize));
        layout.ColumnCount = ImMax(static_cast<int>(availWidth / (layout.ItemSize.x + layout.ItemSpacing)), 1);
        layout.LineCount = (itemCount + layout.ColumnCount - 1) / layout.ColumnCount;

        if (layout.ColumnCount > 1)
            layout.ItemSpacing = floorf(availWidth - layout.ItemSize.x * layout.ColumnCount) / layout.ColumnCount;

        layout.ItemStep = ImVec2(layout.ItemSize.x + layout.ItemSpacing, layout.ItemSize.y + layout.ItemSpacing);
        layout.SelectableSpacing = ImMax(floorf(layout.ItemSpacing) - IconHitSpacing, 0.0f);
        layout.OuterPadding = floorf(layout.ItemSpacing * 0.5f);
    }

    static std::vector<FileDialog::Filter> FiltersForType(AssetType type)
    {
        // Adjust as you like.
        switch (type)
        {
            case AssetType::Mesh: return {{"Meshes", "gltf,glb,obj,fbx"}};
            case AssetType::Texture: return {{"Textures", "png,jpg,jpeg,tga,bmp,ktx,ktx2"}};
            case AssetType::Shader: return {{"Shaders", "vert,frag,glsl"}};
            case AssetType::Material: return {{"Materials", "mat,yaml,yml"}};
            case AssetType::Scene: return {{"Scenes", "scene,yaml,yml"}};
            default: return {};
        }
    }

    static void render()
    {
        static bool open = true;
        ImGui::Begin("Assets Browser", &open, ImGuiWindowFlags_MenuBar);

        // Get assets from registry (stateless - always reflects current registry state)
        auto assets = AssetRegistry::getAllAssetsByType(CurrentAssetType);
        std::vector<std::pair<UUID, AssetMetadata>> assetList(assets.begin(), assets.end());

        const int realItemCount = static_cast<int>(assetList.size());
        const int totalItemCount = realItemCount + 1; // + tile at index 0

        // Menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Asset Type"))
            {
                AssetRegistry::forEachAssetType(
                    [&](AssetType type, std::string_view name)
                    {
                        bool isSelected = (type == CurrentAssetType);
                        if (ImGui::MenuItem(name.data(), nullptr, isSelected))
                        {
                            CurrentAssetType = type;
                            Selection.Clear();
                        }
                    });
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Options"))
            {
                ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
                ImGui::SeparatorText("Layout");
                ImGui::SliderFloat("Icon Size", &IconSize, 16.0f, 128.0f, "%.0f");
                ImGui::SliderInt("Icon Spacing", &IconSpacing, 0, 32);
                ImGui::SliderInt("Icon Hit Spacing", &IconHitSpacing, 0, 32);
                ImGui::PopItemWidth();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Layout includes the + tile
        LayoutState layout;
        const float availWidth = ImGui::GetContentRegionAvail().x;
        UpdateLayoutSizes(layout, availWidth, totalItemCount);

        ImGui::SetNextWindowContentSize(
            ImVec2(0.0f, layout.OuterPadding + layout.LineCount * (layout.ItemSize.y + layout.ItemSpacing)));

        if (ImGui::BeginChild("Assets", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()), ImGuiChildFlags_Borders,
                              ImGuiWindowFlags_NoMove))
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 startPos = ImGui::GetCursorScreenPos();
            startPos = ImVec2(startPos.x + layout.OuterPadding, startPos.y + layout.OuterPadding);
            ImGui::SetCursorScreenPos(startPos);

            ImGuiMultiSelectFlags msFlags = ImGuiMultiSelectFlags_ClearOnEscape |
                                            ImGuiMultiSelectFlags_ClearOnClickVoid |
                                            ImGuiMultiSelectFlags_SelectOnClickRelease | ImGuiMultiSelectFlags_NavWrapX;

            ImGuiMultiSelectIO* msIo = ImGui::BeginMultiSelect(msFlags, Selection.Size, totalItemCount);

            Selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage*, int idx) -> ImGuiID
            { return static_cast<ImGuiID>(idx); };

            const int prevSelectionSize = Selection.Size;
            Selection.ApplyRequests(msIo);

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(layout.SelectableSpacing, layout.SelectableSpacing));

            const ImU32 iconBgColor = ImGui::GetColorU32(IM_COL32(35, 35, 35, 220));
            const ImU32 plusBgColor = ImGui::GetColorU32(IM_COL32(45, 45, 45, 220));
            const bool displayLabel = (layout.ItemSize.x >= ImGui::CalcTextSize("999").x);

            ImGuiListClipper clipper;
            clipper.Begin(layout.LineCount, layout.ItemStep.y);
            if (msIo->RangeSrcItem != -1)
                clipper.IncludeItemByIndex(static_cast<int>(msIo->RangeSrcItem) / layout.ColumnCount);

            while (clipper.Step())
            {
                for (int lineIdx = clipper.DisplayStart; lineIdx < clipper.DisplayEnd; lineIdx++)
                {
                    const int itemMinIdx = lineIdx * layout.ColumnCount;
                    const int itemMaxIdx = ImMin((lineIdx + 1) * layout.ColumnCount, totalItemCount);

                    for (int itemIdx = itemMinIdx; itemIdx < itemMaxIdx; ++itemIdx)
                    {
                        ImGui::PushID(itemIdx);

                        ImVec2 pos = ImVec2(startPos.x + (itemIdx % layout.ColumnCount) * layout.ItemStep.x,
                                            startPos.y + lineIdx * layout.ItemStep.y);
                        ImGui::SetCursorScreenPos(pos);

                        ImGui::SetNextItemSelectionUserData(itemIdx);
                        bool itemIsSelected = Selection.Contains(static_cast<ImGuiID>(itemIdx));
                        bool itemIsVisible = ImGui::IsRectVisible(layout.ItemSize);

                        ImGui::Selectable("", itemIsSelected, ImGuiSelectableFlags_None, layout.ItemSize);

                        // --- + tile ---
                        if (itemIdx == 0)
                        {
                            if (itemIsVisible)
                            {
                                ImVec2 boxMin(pos.x - 1, pos.y - 1);
                                ImVec2 boxMax(boxMin.x + layout.ItemSize.x + 2, boxMin.y + layout.ItemSize.y + 2);
                                drawList->AddRectFilled(boxMin, boxMax, plusBgColor);

                                const char* plus = "+";
                                ImVec2 plusSize = ImGui::CalcTextSize(plus);
                                ImVec2 center = ImVec2((boxMin.x + boxMax.x) * 0.5f, (boxMin.y + boxMax.y) * 0.5f);
                                drawList->AddText(ImVec2(center.x - plusSize.x * 0.5f, center.y - plusSize.y * 0.5f),
                                                  ImGui::GetColorU32(ImGuiCol_Text), plus);

                                if (displayLabel)
                                {
                                    drawList->AddText(ImVec2(boxMin.x, boxMax.y - ImGui::GetFontSize()),
                                                      ImGui::GetColorU32(ImGuiCol_TextDisabled),
                                                      (CurrentAssetType == AssetType::Material ||
                                                       CurrentAssetType == AssetType::Scene)
                                                          ? "Create..."
                                                          : "Import...");
                                }
                            }

                            // Click: Create material via SaveDialog, otherwise import via OpenDialog
                            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                std::println("Add asset button clicked for type {}",
                                             static_cast<int>(CurrentAssetType));

                                //  TODO: This can be optimized
                                if (CurrentAssetType == AssetType::Material)
                                {
                                    // New authored asset => Save dialog
                                    auto pathOpt =
                                        FileDialog::saveFile("Create Material", FiltersForType(CurrentAssetType),
                                                             /*defaultPath*/ "", /*defaultName*/ "NewMaterial.mat");
                                    if (pathOpt)
                                    {
                                        std::filesystem::path p = *pathOpt;
                                        std::string name = p.stem().string();
                                        AssetManager::createMaterial(p, name);
                                        Selection.Clear();
                                    }
                                }
                                else if (CurrentAssetType == AssetType::Scene)
                                {
                                    // New authored asset => Save dialog
                                    auto pathOpt =
                                        FileDialog::saveFile("Create Scene", FiltersForType(CurrentAssetType),
                                                             /*defaultPath*/ "", /*defaultName*/ "NewScene.scene");
                                    if (pathOpt)
                                    {
                                        std::filesystem::path p = *pathOpt;
                                        std::string name = p.stem().string();
                                        AssetManager::createScene(p, name);
                                        Selection.Clear();
                                    }
                                }
                                else
                                {
                                    // Imported asset => Open dialog
                                    auto pathOpt =
                                        FileDialog::openFile("Import Asset", FiltersForType(CurrentAssetType));
                                    if (pathOpt)
                                    {
                                        std::filesystem::path p = *pathOpt;
                                        std::string name = p.stem().string();
                                        AssetRegistry::findOrRegisterAsset(CurrentAssetType, p, name);
                                        Selection.Clear();
                                    }
                                }
                            }

                            ImGui::PopID();
                            continue;
                        }

                        // --- real tiles start at idx 1 ---
                        const int realIdx = itemIdx - 1;
                        const auto& [uuid, metadata] = assetList[realIdx];

                        if (ImGui::IsItemToggledSelection()) itemIsSelected = !itemIsSelected;

                        if (ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("ASSET_UUID", &uuid, sizeof(UUID));
                            ImGui::Text("%s", metadata.name.c_str());
                            ImGui::EndDragDropSource();
                        }

                        if (itemIsVisible)
                        {
                            ImVec2 boxMin(pos.x - 1, pos.y - 1);
                            ImVec2 boxMax(boxMin.x + layout.ItemSize.x + 2, boxMin.y + layout.ItemSize.y + 2);
                            drawList->AddRectFilled(boxMin, boxMax, iconBgColor);

                            if (displayLabel)
                            {
                                ImU32 labelCol =
                                    ImGui::GetColorU32(itemIsSelected ? ImGuiCol_Text : ImGuiCol_TextDisabled);
                                drawList->AddText(ImVec2(boxMin.x, boxMax.y - ImGui::GetFontSize()), labelCol,
                                                  metadata.name.c_str());
                            }
                        }

                        ImGui::PopID();
                    }
                }
            }
            clipper.End();

            ImGui::PopStyleVar();

            if (ImGui::BeginPopupContextWindow())
            {
                ImGui::Text("Selection: %d items", Selection.Size);
                ImGui::EndPopup();
            }

            msIo = ImGui::EndMultiSelect();
            Selection.ApplyRequests(msIo);

            // Selection change => update editor state
            UUID newSelection = UUID::zero();

            void* it = nullptr;
            ImGuiID id = 0;
            if (Selection.GetNextSelectedItem(&it, &id))
            {
                int idx = (int)id;

                // ignore + tile and validate range BEFORE indexing assetList
                if (idx >= 1 && idx < totalItemCount)
                {
                    int realIdx = idx - 1;
                    newSelection = assetList[realIdx].first;
                }
            }

            // Update if selection changed OR selection count changed
            if (newSelection != g_editorState.selectedMaterial || prevSelectionSize != Selection.Size)
            {
                g_editorState.selectedMaterial = newSelection;
            }

            if (realItemCount == 0) ImGui::TextDisabled("No assets of this type (use + to add one)");
        }
        ImGui::EndChild();

        ImGui::Text("%d assets | %d selected", realItemCount, Selection.Size);
        ImGui::End();
    }
};

} // namespace Engine
