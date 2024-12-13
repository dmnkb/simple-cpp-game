#include "MeshSceneNode.h"
#include "pch.h"
#include <glm/glm.hpp>

MeshSceneNode::MeshSceneNode(std::shared_ptr<Mesh> mesh, const std::string& name, std::shared_ptr<SceneNode> parent,
                             std::shared_ptr<Shader> shader, const Texture& texture)
    : SceneNode(name, parent), m_mesh(mesh), m_shader(shader), m_texture(texture)
{
}

const Renderable MeshSceneNode::prepareRenderable()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_localPos) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.x), {1.0f, 0.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.y), {0.0f, 1.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_localRot.z), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), m_localScale);

    return Renderable({m_mesh, m_shader, transform});
}
