#include "PanelFrametime.h"
#include "core/Profiler.h"
#include "imgui.h"
#include "pch.h"
#include <algorithm>
#include <string>
#include <vector>

namespace Engine
{

// TODO: Add a "total" and check The Cherno's Sponsa with Shadows; ~8 ms as a benchmark
void PanelFrametime::render()
{
    static bool open = true;
    ImGui::Begin("Profiler", &open);

    // Suppose this returns std::map<std::string, int> (per your errors)
    const auto timingsMap = Engine::Profiler::getFrameTimeList();

    // Copy to a vector so we can sort
    std::vector<std::pair<std::string, double>> cpuTimings;
    cpuTimings.reserve(timingsMap.size());
    for (const auto& [name, t] : timingsMap)
        cpuTimings.emplace_back(name, static_cast<double>(t));

    // Sort descending by time (note: const refs in comparator)
    std::sort(cpuTimings.begin(), cpuTimings.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    double longestDuration = cpuTimings.empty() ? 0.0 : cpuTimings.front().second;

    // Total
    double cpuTotal = 0.0;
    for (const auto& it : cpuTimings)
        cpuTotal += it.second;

    float availableWidth = std::min(ImGui::GetContentRegionAvail().x, 800.0f);
    float availableHeight = std::min(ImGui::GetContentRegionAvail().y, 200.0f);
    const float barHeight = 22.0f;
    ImVec2 basePos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    for (size_t i = 0; i < cpuTimings.size(); ++i)
    {
        const auto& [name, time] = cpuTimings[i];

        float minWidth = 2.0f;

        const float maxFrameBudget = 16.67f;
        // 60 FPS = 100%.
        float widthPercentage = (100.f / maxFrameBudget) * static_cast<float>(time);
        float calculatedWidth = availableWidth / 100.f * widthPercentage;
        float width = std::max(minWidth, calculatedWidth);

        ImVec2 p0 = ImVec2(basePos.x, basePos.y + (i * barHeight));
        ImVec2 p1 = ImVec2(basePos.x + width, basePos.y + (i * barHeight) + barHeight);

        bool isHovering = ImGui::IsMouseHoveringRect(p0, ImVec2(p1.x + availableWidth, p1.y));

        float alpha = ((calculatedWidth < minWidth) && !isHovering) ? .35f : 1.0f;
        ImVec4 c = colors[i % colors.size()];
        c.w = alpha; // set alpha
        ImU32 col = ImGui::GetColorU32(c);
        drawList->AddRectFilled(p0, p1, col);

        if (isHovering)
            ImGui::SetTooltip("%.2f ms", static_cast<float>(time));

        char labelBuf[64];
        snprintf(labelBuf, sizeof(labelBuf), "%s %.1fms", name.c_str(), static_cast<float>(time));
        drawList->PushClipRect(p0, ImVec2(availableWidth + basePos.x, availableHeight + basePos.y), true);
        drawList->AddText(ImVec2(basePos.x + 8.0f, p0.y + 3.0f), IM_COL32(255, 255, 255, 255), labelBuf);
        drawList->PopClipRect();
    }

    ImGui::End();
}

} // namespace Engine