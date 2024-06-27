#pragma once

#include "Camera.h"
#include "Plane.h"
#include "Renderer.h"

class Game
{
public:
  Game();

  void run();

  ~Game();

private:
  Renderer *renderer;
  Camera camera;

  float camX, camZ;

  Plane *plane;
};