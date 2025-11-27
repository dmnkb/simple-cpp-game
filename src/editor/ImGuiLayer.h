#pragma once

struct GLFWwindow; // Forward-declared to avoid including GLFW here

namespace Engine
{

class ImGuiLayer
{
  public:
    ImGuiLayer() = default;
    ~ImGuiLayer() = default;

    // Called once when the engine initializes
    void onAttach(GLFWwindow* window);

    // Called once when shutting down
    void onDetach();

    // Called at the beginning of the frame (before building UI)
    void beginFrame();

    // Called after all UI rendering is done (submits UI draw calls)
    void endFrame();

    // Optional: expose visibility toggling for editor/editor UI
    void setEnabled(bool enable)
    {
        m_enabled = enable;
    }
    bool isEnabled() const
    {
        return m_enabled;
    }

  private:
    bool m_enabled = true;
    bool m_initialized = false;
};

} // namespace Engine
