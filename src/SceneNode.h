#pragma once

/**
 * As taken from https://irrlicht.sourceforge.io/docu/classirr_1_1scene_1_1_i_scene_node.html
 *
 * A scene node is a node in the hierarchical scene graph. Every scene node may have children,
 * which are also scene nodes. Children move relative to their parent's position.
 * If the parent of a node is not visible, its children won't be visible either.
 * In this way, it is for example easily possible to attach a light to a moving car,
 * or to place a walking character on a moving platform on a moving ship.
 */

#include "pch.h"
#include <glm/glm.hpp>
#include <memory>

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
  public:
    SceneNode(const std::string& name = "", std::shared_ptr<SceneNode> parent = nullptr);
    virtual ~SceneNode();

    void addChild(std::unique_ptr<SceneNode> child);
    void removeChild(SceneNode* child);
    void setParent(std::shared_ptr<SceneNode> parent);

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

  private:
    std::string m_name;
    glm::vec3 m_position, m_rotation, m_scale = glm::vec3{0.0f};
    std::shared_ptr<SceneNode> m_parent = nullptr;
    std::vector<std::unique_ptr<SceneNode>> m_children;
};