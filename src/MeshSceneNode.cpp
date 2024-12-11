#include "MeshSceneNode.h"
#include "pch.h"

MeshSceneNode::MeshSceneNode(std::shared_ptr<Mesh> mesh, const std::string& name, std::shared_ptr<SceneNode> parent)
    : SceneNode(name, parent), m_mesh(mesh)
{
}
