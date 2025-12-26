#pragma once

#include "assets/Asset.h"
#include "core/Core.h"
#include "core/EventManager.h"
#include "editor/Editor.h"
#include "editor/ImGuiLayer.h"
#include "renderer/Renderer.h"
#include "scene/Scene.h"

namespace Engine
{

class Application
{
  public:
    Application();
    ~Application();

    void run();
    void onKeyEvent(const Ref<Event> event);

  private:
    // Editor is a tool layer that operates on whichever scene the app marks as active.
    Scope<Editor> m_editorLayer;
    Scope<ImGuiLayer> m_imguiLayer;
    Scope<Renderer> m_renderer;
    Ref<AssetManager> m_assetManager;
    Ref<AssetRegistry> m_assetRegistry;
    Ref<Scene> m_activeScene;

    // TODO: Think about how to best handle editor vs. runtime mode
    bool m_isEditorMode = true;

    double m_DeltaTime = 0.0;
    double m_FPSUpdateTime = 0.0;
    int m_FrameCount = 0;
    bool m_CanLockCursor = true;
};

} // namespace Engine