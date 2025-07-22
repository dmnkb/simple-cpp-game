#include "PanelFrametime.h"
#include "imgui.h"

void PanelFrametime::render()
{
    static bool open = true;
    ImGui::Begin("Frametime", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

    // Mock timings: (RenderPassName, FrameTimeInMs)
    std::vector<std::pair<std::string, float>> timings = {
        {"Shadow", 3.5f}, {"Lighting", 2.2f}, {"PostFX", 1.3f}, {"UI", 0.7f}, {"GPU Wait", 0.3f},
    };

    float totalTime = 0.0f;
    for (const auto& [_, t] : timings)
        totalTime += t;

    const float barHeight = 26.0f;
    const float paddingY = 6.0f;
    const float fullWidth = ImGui::GetContentRegionAvail().x;

    ImVec2 basePos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float x = basePos.x;

    for (size_t i = 0; i < timings.size(); ++i)
    {
        const auto& [name, time] = timings[i];
        float width = (time / totalTime) * fullWidth;

        ImU32 color = ImColor::HSV(i / float(timings.size()), 0.6f, 0.9f);
        ImVec2 p0 = ImVec2(x, basePos.y);
        ImVec2 p1 = ImVec2(x + width, basePos.y + barHeight);

        // Draw bar
        drawList->AddRectFilled(p0, p1, color);
        // drawList->AddRect(p0, p1, IM_COL32(0, 0, 0, 255));

        // Tooltip on hover
        if (ImGui::IsMouseHoveringRect(p0, p1))
            ImGui::SetTooltip("%s: %.2f ms (%.1f%%)", name.c_str(), time, (time / totalTime) * 100.0f);

        // Label text
        char label[64];
        snprintf(label, sizeof(label), "%s (%.1fms)", name.c_str(), time);

        ImGui::PushFont(ImGui::GetFont());

        // Clip label inside bar
        drawList->PushClipRect(p0, p1, true);
        drawList->AddText(ImVec2(x + 4.0f, basePos.y + 5.0f), ImColor(0, 0, 0), label);
        drawList->PopClipRect();

        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopFont();

        x += width;
    }

    // Reserve layout space so following widgets don't overlap
    ImGui::Dummy(ImVec2(fullWidth, barHeight));
    ImGui::Text("Total Frame Time: %.2f ms", totalTime);

    ImGui::End();
}
