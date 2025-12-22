#pragma once

#include "core/UUID.h"
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

    // Assets
    // TODO: Can be generalized? Like selectedAsset of type UUID + AssetType?
    // Or even selectedResource of type  std::variant<Scene, Material, Mesh, Shader, Texture>?
    // Can mutiple different assets be selected at once?
    UUID selectedScene = UUID::zero();
    UUID selectedMaterial = UUID::zero();
    UUID selectedMesh = UUID::zero();
    UUID selectedShader = UUID::zero();
    UUID selectedTexture = UUID::zero();

    // Camera
    // std::variant<OrbitPanCtrl, FlySpectatorCtrl> viewportCamController;
};

inline EditorState g_editorState{};

} // namespace Engine