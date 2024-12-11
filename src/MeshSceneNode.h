#pragma once

#include "Mesh.h"
#include "SceneNode.h"

class MeshSceneNode : public SceneNode
{
  public:
    MeshSceneNode(std::shared_ptr<Mesh> mesh, const std::string& name = "",
                  std::shared_ptr<SceneNode> parent = nullptr);

  private:
    std::shared_ptr<Mesh> m_mesh;
};