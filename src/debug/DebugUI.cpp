#include "DebugUI.h"
#include "PanelFrametime.h"
#include "PanelScene.h"
#include "PanelSceneHierarchy.h"
#include "PanelStats.h"
#include "imgui.h"
#include "pch.h"

namespace Engine
{

void DebugUI::render(float fps, Scene& scene)
{
    PanelStats::render(fps, scene);
    PanelFrametime::render();
    PanelSceneHierarchy::render(scene);
    PanelScene::render(fps, scene);
}

} // namespace Engine