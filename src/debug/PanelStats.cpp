#include "PanelStats.h"
#include "core/Profiler.h"
#include "helpers/MemoryUsage.h"
#include "imgui.h"
#include "pch.h"
#include "renderer/ClearColor.h"
#include "renderer/RendererAPI.h"

namespace Engine
{

void PanelStats::render(const float fps, const Scene& scene)
{
    static bool open = true;
    ImGui::Begin("Stats", &open);

    // FPS
    std::string fpsHeaderTitle = std::format("FPS: {:.2f}###FPS", fps);
    if (ImGui::CollapsingHeader(fpsHeaderTitle.c_str()))
    {
        static constexpr int historySize = 10000;
        static float fpsHistory[historySize] = {};
        static int historyOffset = 0;

        fpsHistory[historyOffset] = fps;
        historyOffset = (historyOffset + 1) % historySize;

        ImGui::PushStyleColor(ImGuiCol_PlotLines, IM_COL32(200, 200, 200, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(35, 35, 35, 255));

        ImGui::PlotLines("##FPSPlot", fpsHistory, historySize, historyOffset, nullptr, 0.0f, 1000,
                         ImVec2(ImGui::GetContentRegionAvail().x, 60.0f));

        ImGui::PopStyleColor(2);
    }

    // Memory Usage
    const auto memUsageMB = static_cast<double>(GetProcessMemoryUsage()) / (1024.0 * 1024.0);
    std::string memHeaderTitle = std::format("RAM: {:.5f} MB###RAM", memUsageMB);
    if (ImGui::CollapsingHeader(memHeaderTitle.c_str()))
    {
        static constexpr int historySize = 10000;
        static float memHistory[historySize] = {};
        static int historyOffset = 0;

        memHistory[historyOffset] = memUsageMB;
        historyOffset = (historyOffset + 1) % historySize;

        ImGui::PushStyleColor(ImGuiCol_PlotLines, IM_COL32(200, 200, 200, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(35, 35, 35, 255));

        ImGui::PlotLines("##MemPlot", memHistory, historySize, historyOffset, nullptr, 0.0f, 1000,
                         ImVec2(ImGui::GetContentRegionAvail().x, 60.0f));

        ImGui::PopStyleColor(2);
    }

    // Draw Call Stats
    const auto drawCallsPerPass = Profiler::getDrawCallList();

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

    ImGui::End();
}

} // namespace Engine