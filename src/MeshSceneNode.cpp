#include "MeshSceneNode.h"
#include "pch.h"
#include <glm/ext.hpp>

MeshSceneNode::MeshSceneNode(Ref<Mesh> mesh, Ref<Material> material) : SceneNode(), m_mesh(mesh), m_material(material)
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), {1.0f, 0.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), {0.0f, 1.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), m_scale);

    m_renderable = CreateRef<Renderable>(m_mesh, m_material, transform);
}

void MeshSceneNode::setPosition(const glm::vec3& position)
{
    m_position = position;
    updateTransform();
}

void MeshSceneNode::setRotation(const glm::vec3& rotation)
{
    m_rotation = rotation;
    updateTransform();
}

void MeshSceneNode::setScale(const glm::vec3& scale)
{
    m_scale = scale;
    updateTransform();
}

void MeshSceneNode::updateTransform()
{
    m_renderable->transform = glm::translate(glm::mat4(1.0f), m_position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), {1.0f, 0.0f, 0.0f}) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), {0.0f, 1.0f, 0.0f}) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), {0.0f, 0.0f, 1.0f}) *
                              glm::scale(glm::mat4(1.0f), m_scale);
}