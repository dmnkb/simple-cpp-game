#include "SceneNode.h"
#include "pch.h"

void SceneNode::addChild(Ref<SceneNode> child)
{
    // `shared_from_this()` enables to get a valid shared_ptr instance to this.
    child->setParent(shared_from_this());
    // std::unique_ptr cannot be copied — only moved.
    m_children.push_back(std::move(child));
}

void SceneNode::removeChild(SceneNode* child)
{
    auto it = std::remove_if(m_children.begin(), m_children.end(),
                             [child](const Ref<SceneNode>& node) { return node.get() == child; });
    m_children.erase(it, m_children.end());
}

void SceneNode::setParent(std::weak_ptr<SceneNode> parent)
{
    m_parent = parent;
}

void SceneNode::setPosition(const glm::vec3& position)
{
    m_position = position;
}

void SceneNode::setRotation(const glm::vec3& rotation)
{
    m_rotation = rotation;
}

void SceneNode::setScale(const glm::vec3& scale)
{
    m_scale = scale;
}

void SceneNode::setName(const std::string& name)
{
    m_name = name;
}

const std::string& SceneNode::getName()
{
    return m_name;
}

// TODO: update world transform of this and cascading children
// void SceneNode::updateWorldTransform(const glm::mat4& parentTransform) {
//     worldTransform = parentTransform * localTransform;
//     for (auto& child : children) {
//         child->updateWorldTransform(worldTransform);
//     }
// }

SceneNode::~SceneNode()
{
    // Since the children are managed by unique_ptr, they will be automatically cleaned up.
}