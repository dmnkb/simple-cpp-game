#pragma once

#include "pch.h"
#include "scene/Entity.h"

namespace Engine
{

struct EditorState
{
    // General
    bool showWireframe = false;
    bool showBoundingBoxes = false;
    bool pauseTime = false;

    // ECS
    Entity selectedEntity = {};

    // Camera
    // std::variant<OrbitPanCtrl, FlySpectatorCtrl> viewportCamController;

} static g_editorState;

} // namespace Engine