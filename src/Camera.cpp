#include "Camera.h"

Camera::Camera(float fov, float aspect, float near, float far) : fov(fov), aspect(aspect), near(near), far(far)
{
    vec3 defaultPosition = {0.0f, 0.0f, 5.0f};
    vec3 defaultTarget = {0.0f, 0.0f, 0.0f};
    memcpy(position, defaultPosition, sizeof(vec3));
    memcpy(target, defaultTarget, sizeof(vec3));
}

void Camera::setPosition(float x, float y, float z)
{
    position[0] = x;
    position[1] = y;
    position[2] = z;
}

void Camera::lookAt(float x, float y, float z)
{
    target[0] = x;
    target[1] = y;
    target[2] = z;
}

void Camera::getProjectionMatrix(mat4x4 proj) const { mat4x4_perspective(proj, fov, aspect, near, far); }

void Camera::getViewMatrix(mat4x4 view) const
{
    vec3 up = {0.0f, 1.0f, 0.0f};
    mat4x4_look_at(view, position, target, up);
}
