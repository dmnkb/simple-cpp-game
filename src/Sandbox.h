#pragma once

#include "LearnOpenGLShader.h"
#include "Model.h"
#include "pch.h"

/**
 * A demo layer to test implementations with
 */
class Sandbox
{
  public:
    void init();
    void update(double deltyaTime);

  private:
    LearnOpenGLShader ourShader;
    Model ourModel;
};
