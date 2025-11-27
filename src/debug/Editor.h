#pragma once

#include "debug/Player.h"
#include "scene/Scene.h"

namespace Engine
{

struct DebugState
{
    // General
    bool showWireframe = false;
    bool showBoundingBoxes = false;
    bool pauseTime = false;

    // Viewport Camera
    Camera viewportCam;
    // std::variant<OrbitPanCtrl, FlySpectatorCtrl> viewportCamController;

    // Cascade Shadow Maps
    bool csmDebugEnabled = false;

    int selectedCascade = 0;
} static g_debugState;

class Editor
{
  public:
    Editor(const Ref<Scene>& activeScene);
    ~Editor() = default;

    void onUpdate(float fps, const Ref<Scene>& activeScene, const double deltaTime);

  private:
    void onImGuiRender(float fps, const Ref<Scene>& activeScene, const double deltaTime);
    void throwAwayDemoScene(const Ref<Scene>& activeScene);

  private:
    Ref<Player> m_viewportCamController;
};

} // namespace Engine