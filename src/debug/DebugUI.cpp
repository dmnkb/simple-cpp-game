#include "DebugUI.h"
#include "PanelFrametime.h"
#include "PanelSceneHierarchy.h"
#include "PanelStats.h"
#include "imgui.h"
#include "pch.h"

namespace Engine
{

void DebugUI::render(float fps, const Scene& scene)
{
    PanelStats::render(fps, scene);
    PanelFrametime::render();
    PanelSceneHierarchy::render(scene);
}

} // namespace Engine