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
    Editor(const Ref<Scene>& activeScene, const Ref<AssetManager>& assetManager,
           const Ref<AssetRegistry>& assetRegistry);
    ~Editor() = default;

    void onLoad();
    void onUpdate(float fps, const Ref<Scene>& activeScene, const double deltaTime);

  private:
    void onImGuiRender(float fps, const Ref<Scene>& activeScene, const double deltaTime);
    void throwAwayDemoScene(const Ref<Scene>& activeScene);

  private:
    Ref<AssetManager> m_assetManager;
    Ref<AssetRegistry> m_assetRegistry;
    Ref<FlySpectatorCtrl> m_viewportCamController;
};

} // namespace Engine