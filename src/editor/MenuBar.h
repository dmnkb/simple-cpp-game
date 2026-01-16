#pragma once

#include "imgui.h"
#include <functional>
#include <iostream>

namespace Engine
{

struct MenuBar
{
    static void render(std::function<void()> onRequestQuit, std::function<void()> onSaveProject)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save", "Cmd+S"))
                {
                    onSaveProject();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Quit", "Cmd+Q"))
                {
                    onRequestQuit();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                // future
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
};

} // namespace Engine