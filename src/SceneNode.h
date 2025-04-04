#pragma once

/**
 * As taken from: https://irrlicht.sourceforge.io/docu/classirr_1_1scene_1_1_i_scene_node.html
 *
 * "A scene node is a node in the hierarchical scene graph. Every scene node may have children,
 * which are also scene nodes. Children move relative to their parent's position.
 * If the parent of a node is not visible, its children won't be visible either.
 * In this way, it is for example easily possible to attach a light to a moving car,
 * or to place a walking character on a moving platform on a moving ship."
 */

#include "pch.h"
#include <glm/glm.hpp>
#include <memory>

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
  public:
    SceneNode(){};
    virtual ~SceneNode();

    void addChild(Ref<SceneNode> child);
    void removeChild(SceneNode* child);
    void setParent(std::weak_ptr<SceneNode> parent);

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);
    void setName(const std::string& name);

    const std::string& getName();

  protected:
    std::weak_ptr<SceneNode> m_parent;
    std::vector<Ref<SceneNode>> m_children;

    glm::vec3 m_position, m_rotation = glm::vec3{0.0f};
    glm::vec3 m_scale = glm::vec3{1.0f};
    std::string m_name = "";
};