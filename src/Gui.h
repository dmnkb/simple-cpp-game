#pragma once

#include "Event.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class GuiElement
{
  public:
    virtual void render() = 0;

  private:
    glm::vec2 pos;
};

class GuiText : public GuiElement
{
    using CB = std::function<void()>;

  public:
    GuiText(std::string text) : m_Text(text) {}

    void setCallback(CB callback)
    {
        m_Callback = callback;
    }

    void render()
    {
        // bool open;
        // ImGui::Begin("Hello, ImGui!", &open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        // ImGui::Text(m_Text.c_str());
        // ImGui::End();
        ImGui::ShowDemoWindow();
    }

  private:
    std::string m_Text = "";
    CB m_Callback;
};

class Gui
{
  public:
    static Gui& getInstance(GLFWwindow* window)
    {
        if (!s_Instance)
        {
            s_Instance.reset(new Gui(window));
        }
        return *s_Instance;
    }
    ~Gui();

    // Disable copying and assignment
    Gui(const Gui&) = delete;
    Gui& operator=(const Gui&) = delete;

    static void pushElement(const std::string& id, std::shared_ptr<GuiElement> elem)
    {
        s_GuiElements[id] = elem;
    }

    static void removeElement(std::string id)
    {
        s_GuiElements.erase(id);
    }

    static void beginFrame();

    static void bind()
    {
        for (auto& pair : s_GuiElements)
        {
            auto foo = pair.first;
            auto& elem = pair.second;
            if (elem)
            {
                elem->render();
            }
        }
    }

    static void render();

  private:
    // Private constructor to enforce Singleton pattern
    Gui(GLFWwindow* window);

    // Static pointer to the single instance
    static std::unique_ptr<Gui> s_Instance;
    static std::unordered_map<std::string, std::shared_ptr<GuiElement>> s_GuiElements;
};