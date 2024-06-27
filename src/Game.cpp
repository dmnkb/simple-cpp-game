#include "Game.h"

Game::Game()
    : renderer(new Renderer()),
      camera(45.0f * (3.14159265f / 180.0f), (float)renderer->windowWidth / renderer->windowHeight, 0.1f, 100.0f),
      camX(0.0f), camZ(5.0f)
{
  plane = new Plane(renderer->windowWidth, renderer->windowHeight);
}

void Game::run()
{
  while (renderer->isWindowOpen)
  {
    renderer->beginRender();

    plane->draw(camera);

    // TODO: Times delta time
    if (renderer->keyW)
      camZ -= (float)glfwGetTime() * 0.01;
    if (renderer->keyA)
      camX -= (float)glfwGetTime() * 0.01;
    if (renderer->keyS)
      camZ += (float)glfwGetTime() * 0.01;
    if (renderer->keyD)
      camX += (float)glfwGetTime() * 0.01;

    camera.setPosition(camX, 0, camZ);

    renderer->endRender();
  }
}

Game::~Game()
{
  delete plane;

  exit(EXIT_SUCCESS);
}