#include "DebugUI.h"
#include "PanelFrametime.h"
#include "PanelStats.h"
#include "imgui.h"

void DebugUI::render(float fps, const std::map<std::string, int>& drawCallsPerPass, const Scene& scene)
{
    PanelStats::render(fps, drawCallsPerPass, scene);
    PanelFrametime::render();
}
