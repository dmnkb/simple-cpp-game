
#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include "assets/Asset.h"
#include "core/UUID.h"
#include "editor/EditorState.h"

namespace Engine
{

struct PanelAssets
{
    // Configuration (could be moved to EditorState if needed)
    static inline float IconSize = 96.0f;
    static inline int IconSpacing = 10;
    static inline int IconHitSpacing = 4;

    // Selection state - uses ImGui's selection storage with UUID as key
    static inline ImGuiSelectionBasicStorage Selection;
    static inline AssetType CurrentAssetType = AssetType::Material;

    // Calculated layout sizes (computed each frame, no persistent state)
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

    static void render(const Ref<AssetRegistry>& assetRegistry)
    {
        static bool open = true;
        ImGui::Begin("Assets Browser", &open, ImGuiWindowFlags_MenuBar);

        // Get assets from registry (stateless - always reflects current registry state)
        auto assets = assetRegistry->getAllAssetsByType(CurrentAssetType);
        const int itemCount = static_cast<int>(assets.size());

        // Build a vector for indexed access (needed for ImGui multi-select)
        std::vector<std::pair<UUID, AssetMetadata>> assetList(assets.begin(), assets.end());

        // Menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Asset Type"))
            {
                assetRegistry->forEachAssetType(
                    [&](AssetType type, std::string_view name)
                    {
                        bool isSelected = (type == CurrentAssetType);
                        if (ImGui::MenuItem(name.data(), nullptr, isSelected))
                        {
                            CurrentAssetType = type;
                            Selection.Clear(); // Clear selection when switching types
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

        // Calculate layout
        LayoutState layout;
        const float availWidth = ImGui::GetContentRegionAvail().x;
        UpdateLayoutSizes(layout, availWidth, itemCount);

        ImGui::SetNextWindowContentSize(
            ImVec2(0.0f, layout.OuterPadding + layout.LineCount * (layout.ItemSize.y + layout.ItemSpacing)));

        if (ImGui::BeginChild("Assets", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()), ImGuiChildFlags_Borders,
                              ImGuiWindowFlags_NoMove))
        {
            if (itemCount > 0)
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // Calculate start position
                ImVec2 startPos = ImGui::GetCursorScreenPos();
                startPos = ImVec2(startPos.x + layout.OuterPadding, startPos.y + layout.OuterPadding);
                ImGui::SetCursorScreenPos(startPos);

                // Multi-select setup
                ImGuiMultiSelectFlags msFlags =
                    ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_ClearOnClickVoid |
                    ImGuiMultiSelectFlags_SelectOnClickRelease | ImGuiMultiSelectFlags_NavWrapX;

                ImGuiMultiSelectIO* msIo = ImGui::BeginMultiSelect(msFlags, Selection.Size, itemCount);

                // Custom adapter: use index directly as storage ID
                Selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage*, int idx) -> ImGuiID
                { return static_cast<ImGuiID>(idx); };

                const int prevSelectionSize = Selection.Size;
                Selection.ApplyRequests(msIo);

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                                    ImVec2(layout.SelectableSpacing, layout.SelectableSpacing));

                // Rendering parameters
                const ImU32 iconBgColor = ImGui::GetColorU32(IM_COL32(35, 35, 35, 220));
                const bool displayLabel = (layout.ItemSize.x >= ImGui::CalcTextSize("999").x);

                // Clipper for virtualized rendering
                ImGuiListClipper clipper;
                clipper.Begin(layout.LineCount, layout.ItemStep.y);
                if (msIo->RangeSrcItem != -1)
                    clipper.IncludeItemByIndex(static_cast<int>(msIo->RangeSrcItem) / layout.ColumnCount);

                while (clipper.Step())
                {
                    for (int lineIdx = clipper.DisplayStart; lineIdx < clipper.DisplayEnd; lineIdx++)
                    {
                        const int itemMinIdx = lineIdx * layout.ColumnCount;
                        const int itemMaxIdx = ImMin((lineIdx + 1) * layout.ColumnCount, itemCount);

                        for (int itemIdx = itemMinIdx; itemIdx < itemMaxIdx; ++itemIdx)
                        {
                            const auto& [uuid, metadata] = assetList[itemIdx];
                            ImGui::PushID(static_cast<int>(itemIdx));

                            // Position item
                            ImVec2 pos = ImVec2(startPos.x + (itemIdx % layout.ColumnCount) * layout.ItemStep.x,
                                                startPos.y + lineIdx * layout.ItemStep.y);
                            ImGui::SetCursorScreenPos(pos);

                            ImGui::SetNextItemSelectionUserData(itemIdx);
                            bool itemIsSelected = Selection.Contains(static_cast<ImGuiID>(itemIdx));
                            bool itemIsVisible = ImGui::IsRectVisible(layout.ItemSize);
                            ImGui::Selectable("", itemIsSelected, ImGuiSelectableFlags_None, layout.ItemSize);

                            // Update selection state immediately for visual feedback
                            if (ImGui::IsItemToggledSelection()) itemIsSelected = !itemIsSelected;

                            // Drag and drop
                            if (ImGui::BeginDragDropSource())
                            {
                                // Payload contains the UUID of the dragged asset
                                ImGui::SetDragDropPayload("ASSET_UUID", &uuid, sizeof(UUID));
                                ImGui::Text("%s", metadata.name.c_str());
                                ImGui::EndDragDropSource();
                            }

                            // Render icon
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

                // Context menu (read-only, no deletion)
                if (ImGui::BeginPopupContextWindow())
                {
                    ImGui::Text("Selection: %d items", Selection.Size);
                    ImGui::EndPopup();
                }

                msIo = ImGui::EndMultiSelect();
                Selection.ApplyRequests(msIo);

                // Fire callback if selection changed
                if (Selection.Size != prevSelectionSize)
                {
                    // Get first selected item's UUID
                    void* it = nullptr;
                    ImGuiID id = 0;
                    if (Selection.GetNextSelectedItem(&it, &id))
                    {
                        int idx = static_cast<int>(id);
                        if (idx >= 0 && idx < itemCount)
                        {
                            g_editorState.selectedMaterial = assetList[idx].first;
                        }
                    }
                }
            }
            else
            {
                ImGui::TextDisabled("No assets of this type");
            }
        }
        ImGui::EndChild();

        // Status bar
        ImGui::Text("%d assets | %d selected", itemCount, Selection.Size);

        ImGui::End();
    }
};

} // namespace Engine