#pragma once

#include <linmath.h>

class Camera
{
  public:
    Camera(float fov, float aspect, float near, float far);

    void setPosition(float x, float y, float z);
    void lookAt(float x, float y, float z);

    void getProjectionMatrix(mat4x4 proj) const;
    void getViewMatrix(mat4x4 view) const;

  private:
    float fov;
    float aspect;
    float near;
    float far;

    vec3 position;
    vec3 target;
};