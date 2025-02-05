#pragma once

#include "Material.h"
#include "Mesh.h"
#include "pch.h"

// Current:
// RenderQueue
// ├── Material
// │   ├── Mesh1
// │   │   ├── mat4 Transform1
// │   │   ├── mat4 Transform2
// │   │   └── ...
// │   ├── Mesh2
// │   │   ├── mat4 Transform1
// │   │   ├── mat4 Transform2
// │   │   └── ...

// TODO: Future idea:
// RenderQueue
// ├── Mesh1
// │   ├── SubMesh1 (Material1)
// │   │   ├── InstanceDataBuffer1
// │   │   │   ├── mat4 Transform1
// │   │   │   ├── AnimationState1
// │   │   │   ├── AnimationWeights1
// │   │   │   └── ...
// │   ├── SubMesh2 (Material2)
// │   │   └── InstanceDataBuffer1
// │   │       ├── mat4 Transform1
// │   │       ├── AnimationState1
// │   │       └── ...
// │   └── ...
// ├── Mesh2
// │   ├── SubMesh1 (Material1)
// │   │   └── InstanceDataBuffer1
// │   └── ...
// └── ...

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;
