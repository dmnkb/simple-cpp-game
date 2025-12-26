#pragma once

#include "imgui.h"

#include "core/Profiler.h"
#include "util/MemoryUsage.h"

namespace Engine
{

// https://colorbrewer2.org/#type=qualitative&scheme=Set3&n=12
static const std::array<ImColor, 12> colors = {
    IM_COL32(141, 211, 199, 255), IM_COL32(255, 255, 179, 255), IM_COL32(190, 186, 218, 255),
    IM_COL32(251, 128, 114, 255), IM_COL32(128, 177, 211, 255), IM_COL32(253, 180, 98, 255),
    IM_COL32(179, 222, 105, 255), IM_COL32(252, 205, 229, 255), IM_COL32(217, 217, 217, 255),
    IM_COL32(188, 128, 189, 255), IM_COL32(204, 235, 197, 255), IM_COL32(255, 237, 111, 255),
};

struct PanelStats
{
    static void render(const float fps)
    {
        static bool open = true;
        ImGui::Begin("Stats", &open);

        // MARK: FPS
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

        // MARK: Memory Usage
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

        // MARK: Draw Call Stats
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

        // MARK: Frametime Stats
        if (ImGui::CollapsingHeader("Frametime"))
        {

            const float maxFrameBudget = 16.67f; // 60 FPS target
            float availableWidth = ImMin(ImGui::GetContentRegionAvail().x, 800.0f);
            const float barHeight = 20.0f;
            const float sectionGap = 0.0f;
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // Get CPU timings
            const auto cpuTimings = Engine::Profiler::getFrameTimeList();
            double cpuTotal = 0.0;
            for (const auto& [name, t] : cpuTimings)
                cpuTotal += t;

            // Get GPU timings
            const auto gpuTimings = Engine::Profiler::getGPUTimeList();
            double gpuTotal = 0.0;
            for (const auto& [name, t] : gpuTimings)
                gpuTotal += t;

            // Build color map for pass names (consistent colors across CPU/GPU)
            std::map<std::string, size_t> colorIndex;
            size_t idx = 0;
            for (const auto& [name, t] : cpuTimings)
            {
                if (colorIndex.find(name) == colorIndex.end()) colorIndex[name] = idx++;
            }
            for (const auto& [name, t] : gpuTimings)
            {
                if (colorIndex.find(name) == colorIndex.end()) colorIndex[name] = idx++;
            }

            // Helper to draw a timing bar row
            auto drawTimingRow = [&](const char* label, double total, const auto& timings, float yOffset)
            {
                ImVec2 basePos = ImGui::GetCursorScreenPos();
                basePos.y += yOffset;

                // Draw label
                char labelBuf[64];
                snprintf(labelBuf, sizeof(labelBuf), "%s: %.2fms", label, total);
                drawList->AddText(ImVec2(basePos.x, basePos.y), IM_COL32(255, 255, 255, 255), labelBuf);

                float barY = basePos.y + 18.0f;
                float xOffset = 0.0f;

                for (const auto& [name, time] : timings)
                {
                    float widthPercentage = (100.f / maxFrameBudget) * static_cast<float>(time);
                    float width = std::max(2.0f, availableWidth / 100.f * widthPercentage);

                    ImVec2 p0 = ImVec2(basePos.x + xOffset, barY);
                    ImVec2 p1 = ImVec2(basePos.x + xOffset + width, barY + barHeight);

                    size_t cIdx = colorIndex.count(name) ? colorIndex.at(name) : 0;
                    ImU32 col = colors[cIdx % colors.size()];
                    drawList->AddRectFilled(p0, p1, col);

                    // Tooltip on hover
                    if (ImGui::IsMouseHoveringRect(p0, p1)) ImGui::SetTooltip("%s: %.2f ms", name.c_str(), time);

                    xOffset += width;
                }

                // Draw budget line at 16.67ms
                float budgetX = basePos.x + availableWidth;
                drawList->AddLine(ImVec2(budgetX, barY), ImVec2(budgetX, barY + barHeight),
                                  IM_COL32(255, 100, 100, 180), 2.0f);
            };

            // Draw CPU row
            drawTimingRow("CPU", cpuTotal, cpuTimings, 0.0f);

            // Draw GPU row
            drawTimingRow("GPU", gpuTotal, gpuTimings, barHeight + 26.0f + sectionGap);

            // Bottleneck indicator
            float indicatorY = (barHeight + 26.0f) * 2 + sectionGap * 2;
            ImVec2 basePos = ImGui::GetCursorScreenPos();
            const char* bottleneckText = nullptr;
            ImU32 bottleneckColor = IM_COL32(200, 200, 200, 255);

            if (gpuTotal > 0.01 && cpuTotal > 0.01)
            {
                if (gpuTotal > cpuTotal * 1.2)
                {
                    bottleneckText = "GPU-BOUND";
                    bottleneckColor = IM_COL32(255, 150, 100, 255);
                }
                else if (cpuTotal > gpuTotal * 1.2)
                {
                    bottleneckText = "CPU-BOUND";
                    bottleneckColor = IM_COL32(100, 180, 255, 255);
                }
                else
                {
                    bottleneckText = "BALANCED";
                    bottleneckColor = IM_COL32(150, 255, 150, 255);
                }
            }

            if (bottleneckText)
            {
                drawList->AddText(ImVec2(basePos.x, basePos.y + indicatorY), bottleneckColor, bottleneckText);
            }
        }

        ImGui::End();
    }
};

} // namespace Engine