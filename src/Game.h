#pragma once

#include "Camera.h"
#include "Renderer.h"
#include "Triangle.h"

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

  Triangle *Tri1;
};