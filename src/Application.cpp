#include "Application.h"
#include "core/Profiler.h"
#include "core/Window.h"
#include "pch.h"

namespace Engine
{

Application::Application()
{
    // TODO: Support multiple windows and docking
    Window::WindowProps windowProps = {
        .initialWidth = 1080,
        .initialHeight = 720,
        .title = "Game",
    };
    Window::init(windowProps);

    EventManager::registerListeners(typeid(KeyEvent).name(),
                                    [this](const Ref<Event> event) { this->onKeyEvent(event); });

    m_editorLayer = CreateScope<Editor>(m_activeScene);
    m_imguiLayer = CreateScope<ImGuiLayer>();
    m_imguiLayer->onAttach(Window::glfwWindow);
    m_renderer = CreateScope<Renderer>();
}

void Application::run()
{
    int previousDrawCalls = 0;
    int previousVertexCount = 0;
    double fps = 0.0;

    while (Window::open)
    {
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        m_DeltaTime = float(currentTime - lastTime);

        m_FrameCount++;
        if (currentTime - m_FPSUpdateTime >= 1.0)
        {
            fps = double(m_FrameCount) / (currentTime - m_FPSUpdateTime);
            m_FPSUpdateTime = currentTime;
            m_FrameCount = 0;
        }

        Window::pollEvents();

        Profiler::resetStats();

        EventManager::processEvents();

        if (m_activeScene) m_renderer->render(m_activeScene);

        m_imguiLayer->beginFrame();

        m_editorLayer->onUpdate(float(fps), m_activeScene, m_DeltaTime,
                                [&](UUID id) { m_activeScene = AssetManager::getOrImport<Scene>(id); });

        m_imguiLayer->endFrame();

        Window::swapBuffers();

        lastTime = currentTime;
    }
}

void Application::onKeyEvent(const Ref<Event> event)
{
    auto keyEvent = std::dynamic_pointer_cast<KeyEvent>(event);
    if (!keyEvent) return;

    if (keyEvent->key == GLFW_KEY_ESCAPE && keyEvent->action == GLFW_PRESS)
    {
        if (m_CanLockCursor)
        {
            Window::toggleCursorLock();
            m_CanLockCursor = false;
        }
    }
    if (keyEvent->key == GLFW_KEY_ESCAPE && keyEvent->action == GLFW_RELEASE)
    {
        m_CanLockCursor = true;
    }
}

Application::~Application()
{
    Window::shutdown();
    exit(EXIT_SUCCESS);
}

} // namespace Engine
