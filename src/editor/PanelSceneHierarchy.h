#pragma once

#include "imgui.h"
#include <imgui_internal.h>

#include "core/Profiler.h"
#include "editor/Shared.h"
#include "renderer/ClearColor.h"
#include "renderer/RendererAPI.h"
#include "scene/Scene.h"
#include "util/MemoryUsage.h"
#include "util/uuid.h"

namespace Engine
{

struct PanelSceneHierarchy
{
    static void render(const Ref<Scene>& scene)
    {
        // ImGui::ShowDemoWindow();
        // ImGui::ShowStyleEditor();
        constexpr float footerHeight = 220.0f;
        const float availableWidth = ImGui::GetContentRegionAvail().x;

        static bool open = true;
        static int itemClicked = -1;
        static std::unordered_map<int, UUID> lightDict = {};

        const auto& spotLights = scene->getSpotLights();
        const auto& pointLights = scene->getPointLights();

        int spotLightCount = static_cast<int>(scene->getSpotLights().size());
        int pointLightCount = static_cast<int>(scene->getPointLights().size());

        ImGui::Begin("hierarchy", &open);
        ImGui::BeginChild("##hier_tree", ImVec2(0, -footerHeight));
        {
            if (!scene)
            {
                ImGui::Text("No scene loaded.");
                ImGui::EndChild();
                ImGui::End();
                return;
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Lights"))
            {

                for (size_t i = 0; i < spotLightCount; ++i)
                {
                    ImGui::PushID(i);

                    const std::string label = ("Spot Light " + std::to_string(i));
                    ImGui::Selectable(label.c_str(), false, ImGuiTreeNodeFlags_SpanAvailWidth);

                    lightDict[i] = spotLights[i]->getIdentifier();

                    if (ImGui::IsItemClicked())
                    {
                        itemClicked = static_cast<int>(i);
                    }

                    ImGui::PopID();
                }
                for (size_t i = 0; i < pointLightCount; ++i)
                {
                    ImGui::PushID(static_cast<int>(i + spotLightCount));

                    const std::string label = ("Point Light " + std::to_string(i));
                    ImGui::Selectable(label.c_str(), false, ImGuiTreeNodeFlags_SpanAvailWidth);

                    lightDict[static_cast<int>(i + spotLightCount)] = pointLights[i]->getIdentifier();

                    if (ImGui::IsItemClicked())
                    {
                        itemClicked = static_cast<int>(i + spotLightCount);
                    }
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }
        }
        ImGui::EndChild();

        ImGui::Separator();

        ImGui::BeginChild("##hier_details", ImVec2(0, footerHeight));
        {
            if (itemClicked < spotLightCount)
            {
                // Spot
                if (auto it = lightDict.find(itemClicked); it != lightDict.end())
                {
                    auto identifier = it->second;
                    ImGui::Text("Spot Light ID: %s", identifier.to_string().c_str());

                    auto light = scene->getSpotLightByID(identifier);
                    if (light)
                    {
                        if (ImGui::BeginTable("table_spot_light", 2,
                                              ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody))
                        {
                            ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed,
                                                    ImGui::GetContentRegionAvail().x * 0.3f);
                            ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);

                            // Position
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Position");

                            ImGui::TableSetColumnIndex(1);
                            static glm::vec3 pos = light->getSpotLightProperties().position;
                            if (DragVec3Row("position", pos, 0.1f))
                                light->setPosition(pos);

                            // Direction
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Direction");

                            ImGui::TableSetColumnIndex(1);
                            static glm::vec3 dir = light->getSpotLightProperties().direction;
                            if (DragVec3Row("direction", dir, 0.1f))
                                light->setDirection(dir);

                            // Color + Intensity (same row)
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Color / Intensity");

                            ImGui::TableSetColumnIndex(1);
                            {
                                // Read current values (avoid statics so it always reflects engine state)
                                const auto& ci = light->getSpotLightProperties().colorIntensity; // rgba in [0..1]
                                float color[4] = {ci.r, ci.g, ci.b, ci.a};
                                float intensity = ci.w;

                                ImGui::PushID("color_intensity");

                                ImGuiStyle& style = ImGui::GetStyle();
                                const float avail = ImGui::GetContentRegionAvail().x;
                                const float h = ImGui::GetFrameHeight(); // nice control height
                                const float btnW = avail / 3.0f;         // swatch width (tweak)
                                const float spacing = style.ItemSpacing.x;

                                // Swatch button
                                if (ImGui::ColorButton("##swatch", ImVec4(color[0], color[1], color[2], color[3]),
                                                       ImGuiColorEditFlags_NoTooltip, ImVec2(btnW, h)))
                                {
                                    ImGui::OpenPopup("##picker");
                                }

                                // Intensity to the right of the swatch
                                ImGui::SameLine(0.0f, spacing);
                                ImGui::SetNextItemWidth(avail - btnW - spacing);
                                bool intensityChanged =
                                    ImGui::DragFloat("##intensity", &intensity, 0.1f, 0.0f, FLT_MAX, "I: %.2f");

                                // Color popup
                                if (ImGui::BeginPopup("##picker"))
                                {
                                    bool colorChanged = ImGui::ColorPicker4(
                                        "##picker4", color,
                                        ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB |
                                            ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar |
                                            ImGuiColorEditFlags_NoSidePreview);

                                    if (colorChanged || ImGui::IsItemDeactivatedAfterEdit())
                                        light->setColor(color[0], color[1], color[2]); // add alpha if your API uses it

                                    ImGui::EndPopup();
                                }

                                if (intensityChanged)
                                    light->setIntensity(intensity);

                                ImGui::PopID();
                            }

                            ImGui::EndTable();
                        }
                    }
                }
            }
            else if (itemClicked < spotLightCount + pointLightCount)
            {
                // Point
                if (auto it = lightDict.find(itemClicked); it != lightDict.end())
                {
                    auto identifier = it->second;
                    ImGui::Text("Point Light ID: %s", identifier.to_string().c_str());

                    auto light = scene->getPointLightByID(identifier);
                    if (light)
                    {
                        if (ImGui::BeginTable("table_point_light", 2,
                                              ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody))
                        {
                            ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed,
                                                    ImGui::GetContentRegionAvail().x * 0.3f);
                            ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);

                            // Position
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Position");

                            ImGui::TableSetColumnIndex(1);
                            static glm::vec3 pos = light->getPointLightProperties().position;
                            if (DragVec3Row("position", pos, 0.1f))
                                light->setPosition(pos);

                            // Color + Intensity (same row)
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted("Color / Intensity");

                            ImGui::TableSetColumnIndex(1);
                            {
                                // Read current values (avoid statics so it always reflects engine state)
                                const auto& ci = light->getPointLightProperties().colorIntensity; // rgba in [0..1]
                                float color[4] = {ci.r, ci.g, ci.b, ci.a};
                                float intensity = ci.w;

                                ImGui::PushID("color_intensity");

                                ImGuiStyle& style = ImGui::GetStyle();
                                const float avail = ImGui::GetContentRegionAvail().x;
                                const float h = ImGui::GetFrameHeight(); // nice control height
                                const float btnW = avail / 3.0f;         // swatch width (tweak)
                                const float spacing = style.ItemSpacing.x;

                                // Swatch button
                                if (ImGui::ColorButton("##swatch", ImVec4(color[0], color[1], color[2], color[3]),
                                                       ImGuiColorEditFlags_NoTooltip, ImVec2(btnW, h)))
                                {
                                    ImGui::OpenPopup("##picker");
                                }

                                // Intensity to the right of the swatch
                                ImGui::SameLine(0.0f, spacing);
                                ImGui::SetNextItemWidth(avail - btnW - spacing);
                                bool intensityChanged =
                                    ImGui::DragFloat("##intensity", &intensity, 0.1f, 0.0f, FLT_MAX, "I: %.2f");

                                // Color popup
                                if (ImGui::BeginPopup("##picker"))
                                {
                                    bool colorChanged = ImGui::ColorPicker4(
                                        "##picker4", color,
                                        ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB |
                                            ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar |
                                            ImGuiColorEditFlags_NoSidePreview);

                                    if (colorChanged || ImGui::IsItemDeactivatedAfterEdit())
                                        light->setColor(color[0], color[1], color[2]); // add alpha if your API uses it

                                    ImGui::EndPopup();
                                }

                                if (intensityChanged)
                                    light->setIntensity(intensity);

                                ImGui::PopID();
                            }

                            ImGui::EndTable();
                        }
                    }
                }
            }
        }
        ImGui::EndChild();
        ImGui::End();
    }
};
} // namespace Engine