#pragma once

#include "editor/FlySpectatorCtrl.h"
#include "scene/Scene.h"

namespace Engine
{

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
    Ref<FlySpectatorCtrl> m_viewportCamController;
};

} // namespace Engine