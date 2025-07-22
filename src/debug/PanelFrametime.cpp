#include "PanelFrametime.h"
#include "imgui.h"
#include <algorithm>
#include <string>
#include <vector>

void PanelFrametime::render()
{
    static bool open = true;
    ImGui::Begin("Frame Breakdown", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

    // Mock data: Each pair is (Pass Name, Duration in ms)
    std::vector<std::pair<std::string, float>> cpuTimings = {
        {"Input", 0.3f},   {"Game Logic", 1.2f}, {"AI", 0.8f}, {"Physics", 0.6f},
        {"Culling", 0.5f}, {"Animations", 0.4f}, {"UI", 0.5f}, {"Draw Submission", 1.1f}};

    std::vector<std::pair<std::string, float>> gpuTimings = {
        {"Shadow", 3.5f}, {"Lighting", 2.2f}, {"PostFX", 1.3f}, {"UI", 0.7f}};

    // Calculate totals
    float cpuTotal = 0.0f;
    float gpuTotal = 0.0f;
    for (const auto& [_, t] : cpuTimings)
        cpuTotal += t;
    for (const auto& [_, t] : gpuTimings)
        gpuTotal += t;

    float maxTime = std::max(cpuTotal, gpuTotal);
    float labelColumnWidth = 40.0f;
    float totalLabelWidth = 60.0f;
    float availableWidth = std::min(ImGui::GetContentRegionAvail().x - labelColumnWidth - totalLabelWidth, 800.0f);
    const float barHeight = 22.0f;
    ImVec2 basePos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    auto drawBar = [&](const std::vector<std::pair<std::string, float>>& timings, float actualTime, float maxTime,
                       float yOffset, const char* label)
    {
        float x = basePos.x + labelColumnWidth;
        float startX = x;

        drawList->AddText(ImVec2(basePos.x, basePos.y + yOffset + 3.0f), IM_COL32(255, 255, 255, 255), label);

        for (size_t i = 0; i < timings.size(); ++i)
        {
            const auto& [name, time] = timings[i];
            if (time <= 0.0f)
                continue;

            float width = (time / maxTime) * availableWidth;
            ImVec2 p0 = ImVec2(x, basePos.y + yOffset);
            ImVec2 p1 = ImVec2(x + width, basePos.y + yOffset + barHeight);
            ImU32 color = ImColor::HSV(i / float(timings.size()), 0.6f, 0.9f);

            drawList->AddRectFilled(p0, p1, color);

            if (ImGui::IsMouseHoveringRect(p0, p1))
                ImGui::SetTooltip("%s: %.2f ms", name.c_str(), time);

            char labelBuf[64];
            snprintf(labelBuf, sizeof(labelBuf), "%s (%.1fms)", name.c_str(), time);
            drawList->PushClipRect(p0, p1, true);
            drawList->AddText(ImVec2(x + 4.0f, p0.y + 3.0f), IM_COL32(0, 0, 0, 255), labelBuf);
            drawList->PopClipRect();

            x += width;
        }

        char totalLabel[32];
        snprintf(totalLabel, sizeof(totalLabel), "%.2f ms", actualTime);
        ImVec2 labelPos = ImVec2(startX + (actualTime / maxTime) * availableWidth + 8.0f, basePos.y + yOffset + 2.0f);
        drawList->AddText(labelPos, IM_COL32(255, 255, 255, 255), totalLabel);

        ImGui::Dummy(ImVec2(availableWidth + labelColumnWidth + totalLabelWidth, barHeight));
    };

    drawBar(cpuTimings, cpuTotal, maxTime, 0.0f, "CPU");
    drawBar(gpuTimings, gpuTotal, maxTime, barHeight + 2.0f, "GPU");

    ImGui::End();
}
