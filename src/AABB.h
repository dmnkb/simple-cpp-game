
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