#include "imgui.h"
#include "pch.h"

#include "DebugUI.h"
#include "PanelCamInfo.h"
#include "PanelFrametime.h"
#include "PanelScene.h"
#include "PanelSceneHierarchy.h"
#include "PanelStats.h"

namespace Engine
{

void DebugUI::render(float fps, Scene& scene)
{
    PanelStats::render(fps, scene);
    PanelFrametime::render();
    PanelSceneHierarchy::render(scene);
    PanelScene::render(fps, scene);
    PanelCamInfo::render(scene);
}

} // namespace Engine