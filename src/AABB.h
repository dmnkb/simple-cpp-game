
#include <glm/glm.hpp>

struct BoundingBox
{
    glm::vec3 min;
    glm::vec3 max;
};

inline bool checkCollision(const BoundingBox& box1, const BoundingBox& box2)
{
    return (box1.min.x <= box2.max.x && box1.max.x >= box2.min.x) &&
           (box1.min.y <= box2.max.y && box1.max.y >= box2.min.y) &&
           (box1.min.z <= box2.max.z && box1.max.z >= box2.min.z);
}

inline glm::vec3 getCollisionSide(const BoundingBox& box1, const BoundingBox& box2)
{
    glm::vec3 collisionSide(0.0f);

    // Calculate overlap distances for each axis
    float overlapX1 = box1.max.x - box2.min.x; // Collision with the left face of box2
    float overlapX2 = box2.max.x - box1.min.x; // Collision with the right face of box2
    float overlapY1 = box1.max.y - box2.min.y; // Collision with the bottom face of box2
    float overlapY2 = box2.max.y - box1.min.y; // Collision with the top face of box2
    float overlapZ1 = box1.max.z - box2.min.z; // Collision with the back face of box2
    float overlapZ2 = box2.max.z - box1.min.z; // Collision with the front face of box2

    // Find the smallest overlap to determine the collision side
    float minOverlap = std::numeric_limits<float>::max();

    if (overlapX1 >= 0 && overlapX1 < minOverlap)
    {
        minOverlap = overlapX1;
        collisionSide = glm::vec3(-1.0f, 0.0f, 0.0f); // Collision on the negative X face
    }
    if (overlapX2 >= 0 && overlapX2 < minOverlap)
    {
        minOverlap = overlapX2;
        collisionSide = glm::vec3(1.0f, 0.0f, 0.0f); // Collision on the positive X face
    }
    if (overlapY1 >= 0 && overlapY1 < minOverlap)
    {
        minOverlap = overlapY1;
        collisionSide = glm::vec3(0.0f, -1.0f, 0.0f); // Collision on the negative Y face
    }
    if (overlapY2 >= 0 && overlapY2 < minOverlap)
    {
        minOverlap = overlapY2;
        collisionSide = glm::vec3(0.0f, 1.0f, 0.0f); // Collision on the positive Y face
    }
    if (overlapZ1 >= 0 && overlapZ1 < minOverlap)
    {
        minOverlap = overlapZ1;
        collisionSide = glm::vec3(0.0f, 0.0f, -1.0f); // Collision on the negative Z face
    }
    if (overlapZ2 >= 0 && overlapZ2 < minOverlap)
    {
        minOverlap = overlapZ2;
        collisionSide = glm::vec3(0.0f, 0.0f, 1.0f); // Collision on the positive Z face
    }

    return collisionSide;
}
