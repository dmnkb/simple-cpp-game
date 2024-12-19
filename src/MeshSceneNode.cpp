#include "MeshSceneNode.h"
#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

MeshSceneNode::MeshSceneNode(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, const Texture& texture)
    : SceneNode(), m_mesh(mesh), m_shader(shader), m_texture(texture)
{
}

const Renderable MeshSceneNode::prepareRenderable()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), {1.0f, 0.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), {0.0f, 1.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), m_scale);

    return Renderable({m_mesh, m_shader, transform});
}
