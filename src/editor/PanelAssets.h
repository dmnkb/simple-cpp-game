
#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include "assets/AssetRegistry.h"
#include "core/UUID.h"
#include "editor/EditorState.h"
#include "platform/FileDialog.h"

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

    // Pending import data - set when user confirms import dialog
    struct PendingImport
    {
        bool ready;
        AssetType type;
        std::string filePath;
        std::string assetName;
    };
    static inline PendingImport pendingImport = {false, AssetType::None, "", ""};

    // Import dialog state
    static inline bool showImportDialog = false;
    static inline char importPathBuffer[512] = "";
    static inline char importNameBuffer[128] = "";

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

    static void renderImportDialog()
    {
        if (!showImportDialog) return;

        ImGui::OpenPopup("Import Asset");
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Import Asset", &showImportDialog, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Import new %s asset", AssetTypeNames[static_cast<int>(CurrentAssetType)].c_str());
            ImGui::Separator();

            ImGui::Text("File Path:");
            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##path", importPathBuffer, sizeof(importPathBuffer));
            ImGui::SameLine();
            if (ImGui::Button("Browse..."))
            {
                auto result = FileDialog::openFile(
                    "Select Asset File",
                    {}, // No filters for now - accepts all files
                    ""  // No default path
                );
                if (result.has_value())
                {
                    strncpy(importPathBuffer, result->c_str(), sizeof(importPathBuffer) - 1);
                    importPathBuffer[sizeof(importPathBuffer) - 1] = '\0';
                    
                    // Auto-fill asset name from filename if empty
                    if (importNameBuffer[0] == '\0')
                    {
                        std::filesystem::path p(result.value());
                        std::string stem = p.stem().string();
                        strncpy(importNameBuffer, stem.c_str(), sizeof(importNameBuffer) - 1);
                        importNameBuffer[sizeof(importNameBuffer) - 1] = '\0';
                    }
                }
            }

            ImGui::Text("Asset Name:");
            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##name", importNameBuffer, sizeof(importNameBuffer));

            ImGui::Separator();

            if (ImGui::Button("Import", ImVec2(120, 0)))
            {
                // Store pending import data for external processing
                pendingImport.ready = true;
                pendingImport.type = CurrentAssetType;
                pendingImport.filePath = importPathBuffer;
                pendingImport.assetName = importNameBuffer[0] ? importNameBuffer : "Unnamed Asset";

                // Reset buffers
                importPathBuffer[0] = '\0';
                importNameBuffer[0] = '\0';
                showImportDialog = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                importPathBuffer[0] = '\0';
                importNameBuffer[0] = '\0';
                showImportDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    static void renderAddButton(ImDrawList* drawList, const ImVec2& pos, const LayoutState& layout)
    {
        const ImU32 bgColor = ImGui::GetColorU32(IM_COL32(50, 50, 50, 220));
        const ImU32 plusColor = ImGui::GetColorU32(IM_COL32(120, 120, 120, 255));
        const ImU32 hoverBgColor = ImGui::GetColorU32(IM_COL32(70, 70, 70, 220));
        const ImU32 hoverPlusColor = ImGui::GetColorU32(IM_COL32(200, 200, 200, 255));

        ImVec2 boxMin(pos.x - 1, pos.y - 1);
        ImVec2 boxMax(boxMin.x + layout.ItemSize.x + 2, boxMin.y + layout.ItemSize.y + 2);

        // Make the button clickable
        ImGui::SetCursorScreenPos(pos);
        bool hovered = false;
        bool clicked = ImGui::InvisibleButton("##add_asset", layout.ItemSize);
        hovered = ImGui::IsItemHovered();

        // Draw background
        drawList->AddRectFilled(boxMin, boxMax, hovered ? hoverBgColor : bgColor);

        // Draw plus icon
        const float plusSize = layout.ItemSize.x * 0.4f;
        const float thickness = 3.0f;
        ImVec2 center(boxMin.x + layout.ItemSize.x * 0.5f, boxMin.y + layout.ItemSize.y * 0.5f);

        ImU32 currentPlusColor = hovered ? hoverPlusColor : plusColor;

        // Horizontal line
        drawList->AddLine(ImVec2(center.x - plusSize * 0.5f, center.y), ImVec2(center.x + plusSize * 0.5f, center.y),
                          currentPlusColor, thickness);
        // Vertical line
        drawList->AddLine(ImVec2(center.x, center.y - plusSize * 0.5f), ImVec2(center.x, center.y + plusSize * 0.5f),
                          currentPlusColor, thickness);

        // Handle click
        if (clicked)
        {
            showImportDialog = true;
        }
    }

    static void render(const Ref<AssetRegistry>& assetRegistry)
    {
        static bool open = true;
        ImGui::Begin("Assets Browser", &open, ImGuiWindowFlags_MenuBar);

        // Get assets from registry (stateless - always reflects current registry state)
        auto assets = assetRegistry->getAllAssetsByType(CurrentAssetType);
        const int assetCount = static_cast<int>(assets.size());
        const int totalItemCount = assetCount + 1; // +1 for the add button

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
        UpdateLayoutSizes(layout, availWidth, totalItemCount);

        ImGui::SetNextWindowContentSize(
            ImVec2(0.0f, layout.OuterPadding + layout.LineCount * (layout.ItemSize.y + layout.ItemSpacing)));

        if (ImGui::BeginChild("Assets", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()), ImGuiChildFlags_Borders,
                              ImGuiWindowFlags_NoMove))
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // Calculate start position
            ImVec2 startPos = ImGui::GetCursorScreenPos();
            startPos = ImVec2(startPos.x + layout.OuterPadding, startPos.y + layout.OuterPadding);
            ImGui::SetCursorScreenPos(startPos);

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                                ImVec2(layout.SelectableSpacing, layout.SelectableSpacing));

            // Rendering parameters
            const ImU32 iconBgColor = ImGui::GetColorU32(IM_COL32(35, 35, 35, 220));
            const bool displayLabel = (layout.ItemSize.x >= ImGui::CalcTextSize("999").x);

            // Render all items (add button + assets)
            for (int itemIdx = 0; itemIdx < totalItemCount; ++itemIdx)
            {
                const int lineIdx = itemIdx / layout.ColumnCount;
                ImVec2 pos = ImVec2(startPos.x + (itemIdx % layout.ColumnCount) * layout.ItemStep.x,
                                    startPos.y + lineIdx * layout.ItemStep.y);

                if (itemIdx == 0)
                {
                    // First item: Add button
                    renderAddButton(drawList, pos, layout);
                }
                else
                {
                    // Asset items (index shifted by 1)
                    const int assetIdx = itemIdx - 1;
                    const auto& [uuid, metadata] = assetList[assetIdx];
                    ImGui::PushID(static_cast<int>(itemIdx));

                    ImGui::SetCursorScreenPos(pos);
                    ImGui::SetNextItemSelectionUserData(assetIdx);
                    bool itemIsSelected = Selection.Contains(static_cast<ImGuiID>(assetIdx));
                    bool itemIsVisible = ImGui::IsRectVisible(layout.ItemSize);
                    ImGui::Selectable("", itemIsSelected, ImGuiSelectableFlags_None, layout.ItemSize);

                    // Update selection state immediately for visual feedback
                    if (ImGui::IsItemToggledSelection()) itemIsSelected = !itemIsSelected;

                    // Drag and drop
                    if (ImGui::BeginDragDropSource())
                    {
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

                    // Handle click for selection
                    if (ImGui::IsItemClicked())
                    {
                        g_editorState.selectedMaterial = uuid;
                    }

                    ImGui::PopID();
                }
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        // Status bar
        ImGui::Text("%d assets | %d selected", assetCount, Selection.Size);

        ImGui::End();

        // Render import dialog (outside main window)
        renderImportDialog();
    }
};

} // namespace Engine