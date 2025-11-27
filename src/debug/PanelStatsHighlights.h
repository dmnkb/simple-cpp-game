#pragma once

#include "imgui.h"

#include "pch.h"
#include "renderer/Camera.h"
#include "scene/Scene.h"

namespace Engine
{

struct PanelStatsHighlights
{
    static void render(const float fps, const Ref<Scene>& scene)
    {
        static bool open = true;
        ImGui::Begin("Camera Info", &open,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
        const auto& camera = scene->getActiveCamera();

        ImGui::Text("%s", std::format("FPS: {:.2f} FPS", fps).c_str());
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", camera.getPosition().x, camera.getPosition().y,
                    camera.getPosition().z);
        ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", camera.getRotation().x, camera.getRotation().y,
                    camera.getRotation().z);
        ImGui::End();
    }
};

} // namespace Engine