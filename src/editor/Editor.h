#pragma once

#include "assets/AssetManager.h"
#include "assets/AssetRegistry.h"
#include "editor/FlySpectatorCtrl.h"
#include "scene/Scene.h"

namespace Engine
{

class Editor
{
  public:
    Editor(const Ref<Scene>& activeScene);
    ~Editor() = default;

    void onLoad();
    void onUpdate(float fps, const Ref<Scene>& activeScene, const double deltaTime,
                  std::function<void(UUID)> onSelectScene);

  private:
    void onImGuiRender(float fps, const Ref<Scene>& activeScene, const double deltaTime,
                       std::function<void(UUID)> onSelectScene);
    void throwAwayDemoScene(const Ref<Scene>& activeScene);

  private:
    Ref<FlySpectatorCtrl> m_viewportCamController;
};

} // namespace Engine