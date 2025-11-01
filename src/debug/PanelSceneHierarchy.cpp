#include "PanelSceneHierarchy.h"
#include "core/Profiler.h"
#include "imgui.h"
#include "pch.h"
#include "renderer/ClearColor.h"
#include "renderer/RendererAPI.h"
#include "util/MemoryUsage.h"
#include "util/uuid.h"

namespace Engine
{

void PanelSceneHierarchy::render(const Scene& scene)
{
    // ImGui::ShowDemoWindow();
    // ImGui::ShowStyleEditor();
    constexpr float footerHeight = 220.0f;

    static bool open = true;
    static int itemClicked = -1;
    static std::unordered_map<int, UUID> lightDict = {};

    const auto& spotLights = scene.getSpotLights();
    const auto& pointLights = scene.getPointLights();
    const auto& dirLight = scene.getDirectionalLight();

    int spotLightCount = static_cast<int>(scene.getSpotLights().size());
    int pointLightCount = static_cast<int>(scene.getPointLights().size());

    ImGui::Begin("hierarchy", &open);
    ImGui::BeginChild("##hier_tree", ImVec2(0, -footerHeight));
    {

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Lights"))
        {

            for (size_t i = 0; i < spotLightCount; ++i)
            {
                ImGui::PushID(i);

                const char* label = ("Spot Light " + std::to_string(i)).c_str();
                ImGui::Selectable(label, false, ImGuiTreeNodeFlags_SpanAvailWidth);

                lightDict[i] = spotLights[i]->getIdentifier();

                if (ImGui::IsItemClicked())
                {
                    itemClicked = static_cast<int>(i);
                }

                ImGui::PopID();
            }
            for (size_t i = 0; i < pointLightCount; ++i)
            {
                ImGui::PushID(static_cast<int>(i + spotLightCount));

                const char* label = ("Point Light " + std::to_string(i)).c_str();
                ImGui::Selectable(label, false, ImGuiTreeNodeFlags_SpanAvailWidth);

                lightDict[static_cast<int>(i + spotLightCount)] = pointLights[i]->getIdentifier();

                if (ImGui::IsItemClicked())
                {
                    itemClicked = static_cast<int>(i + spotLightCount);
                }
                ImGui::PopID();
            }
            ImGui::PushID(static_cast<int>(spotLightCount + pointLightCount));
            ImGui::Selectable("Directional Light", false, ImGuiTreeNodeFlags_SpanAvailWidth);
            lightDict[static_cast<int>(spotLightCount + pointLightCount)] = dirLight->getIdentifier();
            if (ImGui::IsItemClicked())
            {
                itemClicked = static_cast<int>(spotLightCount + pointLightCount);
            }
            ImGui::PopID();

            ImGui::TreePop();
        }
    }
    ImGui::EndChild();

    ImGui::Separator();

    ImGui::BeginChild("##hier_details", ImVec2(0, footerHeight));
    {
        if (itemClicked == -1)
        {
            ImGui::Text("Properties: None");
        }
        else if (itemClicked < spotLightCount)
        {
            // Spot
            if (auto it = lightDict.find(itemClicked); it != lightDict.end())
            {
                auto identifier = it->second;
                ImGui::Text("Spot Light ID: %s", identifier.to_string().c_str());

                auto light = scene.getSpotLightByID(identifier);
                if (light)
                {
                    ImGui::Text("Position:");
                    static glm::vec3 position = light->getSpotLightProperties().position;
                    ImGui::SliderFloat("X", (float*)&position.x, -100.0f, 100.0f, "%.0f");
                    ImGui::SliderFloat("Y", (float*)&position.y, -100.0f, 100.0f, "%.0f");
                    ImGui::SliderFloat("Z", (float*)&position.z, -100.0f, 100.0f, "%.0f");
                    light->setPosition(position);
                    ImGui::Separator();
                    ImGui::Text("Direction:");
                    static glm::vec3 direction = light->getSpotLightProperties().direction;
                    ImGui::SliderFloat("Dir X", (float*)&direction.x, -1.0f, 1.0f, "%.2f");
                    ImGui::SliderFloat("Dir Y", (float*)&direction.y, -1.0f, 1.0f, "%.2f");
                    ImGui::SliderFloat("Dir Z", (float*)&direction.z, -1.0f, 1.0f, "%.2f");
                    light->setDirection(direction);
                }
            }
        }
        else if (itemClicked < spotLightCount + pointLightCount)
        {
            // Point
            if (auto it = lightDict.find(itemClicked); it != lightDict.end())
            {
                auto identifier = it->second;

                auto light = scene.getPointLightByID(identifier);
                if (light)
                {
                    ImGui::Text("Position:");
                    static glm::vec3 position = light->getPointLightProperties().position;
                    ImGui::SliderFloat("X", (float*)&position.x, -100.0f, 100.0f, "%.0f");
                    ImGui::SliderFloat("Y", (float*)&position.y, -100.0f, 100.0f, "%.0f");
                    ImGui::SliderFloat("Z", (float*)&position.z, -100.0f, 100.0f, "%.0f");
                    light->setPosition(position);
                }
            }
        }
        else
        {
            // Directional
            if (auto it = lightDict.find(itemClicked); it != lightDict.end())
            {
                auto identifier = it->second;
                ImGui::Text("Directional Light ID: %s", identifier.to_string().c_str());

                auto light = scene.getDirectionalLightByID(identifier);
                ;
                if (light)
                {
                    ImGui::Text("Direction:");
                    static glm::vec3 direction = light->getDirectionalLightProperties().direction;
                    ImGui::SliderFloat("Dir X", (float*)&direction.x, -1.0f, 1.0f, "%.2f");
                    ImGui::SliderFloat("Dir Y", (float*)&direction.y, -1.0f, 1.0f, "%.2f");
                    ImGui::SliderFloat("Dir Z", (float*)&direction.z, -1.0f, 1.0f, "%.2f");
                    light->setDirection(direction);
                }
            }
        }
    }

    ImGui::EndChild();

    ImGui::End();
}

} // namespace Engine