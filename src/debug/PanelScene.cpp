#include "PanelScene.h"
#include "imgui.h"
#include "pch.h"
#include "renderer/ClearColor.h"
#include "renderer/RendererAPI.h"

namespace Engine
{

void PanelScene::render(const float fps, const Scene& scene)
{
    static bool open = true;
    ImGui::Begin("Scene", &open);

    const float availableWidth = ImGui::GetContentRegionAvail().x;

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Scene"))
        {
            ImGui::SeparatorText("Clear Color");
            if (ImGui::BeginTable("tex_props", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody))
            {
                ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.4f);
                ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);

                static int selected = 0;
                static int last_selected = -1;
                static float clearColor[4] = {clearColorValues[0].r(), clearColorValues[0].g(), clearColorValues[0].b(),
                                              clearColorValues[0].a()};

                // Presets row
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Presets");
                ImGui::TableSetColumnIndex(1);

                bool reapplyPreset = false;

                ImGui::SetNextItemWidth(-1);
                if (ImGui::BeginCombo("##ClearColorCombo", clearColorTitles[selected].c_str()))
                {
                    for (int i = 0; i < (int)clearColorTitles.size(); ++i)
                    {
                        bool isSelected = (i == selected);
                        if (ImGui::Selectable(clearColorTitles[i].c_str(), isSelected))
                        {
                            selected = i;
                            reapplyPreset = true;
                        }
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (reapplyPreset || selected != last_selected)
                {
                    const SClearColor& preset = clearColorValues[selected];
                    clearColor[0] = preset.r();
                    clearColor[1] = preset.g();
                    clearColor[2] = preset.b();
                    clearColor[3] = preset.a();

                    RendererAPI::setClearColor(preset);
                    last_selected = selected;
                }

                // row: Clear Color
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Clear Color");
                ImGui::TableSetColumnIndex(1);

                // Button that shows the current color and opens a picker
                if (ImGui::ColorButton("##clearColorBtn",
                                       ImVec4(clearColor[0], clearColor[1], clearColor[2], clearColor[3]),
                                       ImGuiColorEditFlags_NoTooltip, ImVec2(ImGui::GetContentRegionAvail().x, 24.0f)))
                {
                    ImGui::OpenPopup("##clearColorPopup");
                }

                // Popup with the color picker
                if (ImGui::BeginPopup("##clearColorPopup"))
                {
                    bool changedLive = ImGui::ColorPicker4(
                        "##clearColorPicker", clearColor,
                        ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_AlphaBar |
                            ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoSidePreview);

                    // Apply while dragging/typing…
                    if (changedLive)
                    {
                        RendererAPI::setClearColor(
                            SClearColor{clearColor[0], clearColor[1], clearColor[2], clearColor[3]});
                    }

                    // …or only once the edit is “committed”
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        RendererAPI::setClearColor(
                            SClearColor{clearColor[0], clearColor[1], clearColor[2], clearColor[3]});
                    }

                    ImGui::EndPopup();
                }

                ImGui::EndTable();
            }

            // Environment Lighting
            ImGui::SeparatorText("Environment Lighting");

            if (ImGui::BeginTable("tex_props", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody))
            {
                ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.4f);
                ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);

                static float sunColor[4] = {1, 1, 1, 1};

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Sun Color");
                ImGui::TableSetColumnIndex(1);

                if (ImGui::ColorButton("##sunColorBtn", ImVec4(sunColor[0], sunColor[1], sunColor[2], sunColor[3]),
                                       ImGuiColorEditFlags_NoTooltip, ImVec2(ImGui::GetContentRegionAvail().x, 24.0f)))
                {
                    ImGui::OpenPopup("##sunColorPopup");
                }
                if (ImGui::BeginPopup("##sunColorPopup"))
                {
                    ImGui::ColorPicker4("##sunColorPicker", sunColor,
                                        ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB |
                                            ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar |
                                            ImGuiColorEditFlags_NoSidePreview);
                    ImGui::EndPopup();
                }

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Inspector"))
        {
            ImGui::SeparatorText("Inspector Panel");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

} // namespace Engine