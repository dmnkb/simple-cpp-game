#include "PanelFrametime.h"
#include "Profiler.h"
#include "imgui.h"
#include "pch.h"
#include <algorithm>
#include <string>
#include <vector>

namespace Engine
{

void PanelFrametime::render()
{
    static bool open = true;
    ImGui::Begin("Frame Breakdown", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

    const auto cpuTimings = Engine::Profiler::getAll();

    double longestDuration = 0.0;

    auto maxIt = std::max_element(cpuTimings.begin(), cpuTimings.end(),
                                  [](const auto& a, const auto& b) { return a.second < b.second; });

    if (maxIt != cpuTimings.end())
        longestDuration = maxIt->second;

    // Mock data: Each pair is (Pass Name, Duration in ms)
    // std::vector<std::pair<std::string, float>> cpuTimings = {
    //     {"Game Logic", 1.2f},
    //     {"Draw Submission", 1.1f},
    //     {"AI", 0.8f},
    //     {"Physics", 0.6f},
    //     {"Culling", 0.5f},
    //     {"UI", 0.5f},
    //     {"Animations", 0.4f},
    //     {"Input", 0.3f},
    //     {"Occlusion Queries", 0.3f},
    //     {"Script Execution", 0.3f},
    //     {"Pathfinding", 0.3f},
    //     {"Audio Processing", 0.2f},
    //     {"Networking", 0.2f},
    //     {"Garbage Collection", 0.2f},
    //     {"Scene Management", 0.1f},
    //     {"Profiling Overhead", 0.05f},
    // };

    // Calculate totals
    float cpuTotal = 0.0f;

    for (const auto& [_, t] : cpuTimings)
        cpuTotal += t;

    float availableWidth = std::min(ImGui::GetContentRegionAvail().x, 800.0f);
    const float barHeight = 22.0f;
    ImVec2 basePos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    for (size_t i = 0; i < cpuTimings.size(); ++i)
    {
        const auto& [name, time] = cpuTimings[i];

        float minWidth = 5.0f;
        float width =
            std::max(minWidth, longestDuration > 0.0
                                   ? (availableWidth / static_cast<float>(longestDuration)) * static_cast<float>(time)
                                   : minWidth);

        ImVec2 p0 = ImVec2(basePos.x, basePos.y + (i * barHeight));
        ImVec2 p1 = ImVec2(basePos.x + width, basePos.y + (i * barHeight) + barHeight);

        ImU32 color = ImColor(colors[i % colors.size()]);

        drawList->AddRectFilled(p0, p1, color);

        if (ImGui::IsMouseHoveringRect(p0, p1))
            ImGui::SetTooltip("%s: %.2f ms", name.c_str(), time);

        char labelBuf[64];
        snprintf(labelBuf, sizeof(labelBuf), "%s (%.1fms)", name.c_str(), time);
        drawList->PushClipRect(p0, p1, true);
        drawList->AddText(ImVec2(basePos.x + 4.0f, p0.y + 3.0f), IM_COL32(0, 0, 0, 100), labelBuf);
        drawList->PopClipRect();
    }

    ImGui::End();
}

} // namespace Engine