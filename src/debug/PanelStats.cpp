#include "PanelStats.h"
#include "imgui.h"
#include "renderer/ClearColor.h"
#include "renderer/RendererAPI.h"

void PanelStats::render(const float fps, const std::map<std::string, int>& drawCallsPerPass, const Scene& scene)
{
    static bool open = true;
    ImGui::Begin("Stats", &open);

    std::string headerTitle = "FPS: " + std::to_string(fps);

    // FPS
    if (ImGui::CollapsingHeader(headerTitle.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        // Static circular buffer to store FPS history
        static constexpr int historySize = 10000;
        static float fpsHistory[historySize] = {};
        static int historyOffset = 0;

        fpsHistory[historyOffset] = fps;
        historyOffset = (historyOffset + 1) % historySize;

        ImGui::PushStyleColor(ImGuiCol_PlotLines, IM_COL32(200, 200, 200, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(35, 35, 35, 255));

        ImGui::PlotLines("##FPSPlot", fpsHistory, historySize, historyOffset, nullptr, 0.0f, 1000,
                         ImVec2(ImGui::GetContentRegionAvail().x, 60.0f)); // width = auto, height = 60px

        ImGui::PopStyleColor(2);
    }

    // Draw Call Stats
    if (!drawCallsPerPass.empty())
    {
        if (ImGui::CollapsingHeader("Draw Calls"))
        {
            if (ImGui::BeginTable("DrawCallTable", 2, ImGuiTableFlags_RowBg))
            {
                for (const auto& [passName, callCount] : drawCallsPerPass)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", passName.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", callCount);
                }
            }
            ImGui::EndTable();
            ImGui::Spacing();
        }
    }

    // Clear Color
    if (ImGui::CollapsingHeader("Sky Color"))
    {
        static int selected = static_cast<int>(EClearColor::CharcoalGray);

        ImGui::Spacing();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##ClearColorCombo", clearColorTitles[selected].c_str()))
        {
            for (int i = 0; i < ClearColorCount; ++i)
            {
                bool isSelected = (i == selected);

                if (ImGui::Selectable(clearColorTitles[i].c_str(), isSelected))
                    selected = i;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::Spacing();

        const SClearColor& color = clearColorValues[selected];
        RendererAPI::setClearColor(color);
    }

    // Shadow Map Previews
    if (!scene.getLightSceneNodes().empty())
    {
        if (ImGui::CollapsingHeader("Shadow Maps"))
        {
            for (const auto& lightNode : scene.getLightSceneNodes())
            {
                const auto& depth = lightNode->getShadowDepthTexture();
                const auto& color = lightNode->getShadowDebugColorTexture();

                ImGui::Image((void*)(intptr_t)depth->id, ImVec2(128, 96), ImVec2(1, 1), ImVec2(0, 0));
                ImGui::SameLine();
                ImGui::Image((void*)(intptr_t)color->id, ImVec2(128, 96), ImVec2(1, 1), ImVec2(0, 0));
            }
        }
    }

    ImGui::End();
}
