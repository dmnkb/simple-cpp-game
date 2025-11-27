#pragma once

#include "imgui.h"

#include "core/Profiler.h"
#include "pch.h"
#include "util/MemoryUsage.h"

namespace Engine
{

struct PanelStats
{
    static void render(const float fps)
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

        ImGui::End();
    }
};

} // namespace Engine