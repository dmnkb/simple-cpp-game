#include "Game.h"

Game::Game()
    : renderer(new Renderer()),
      camera(45.0f * (3.14159265f / 180.0f),
             (float)renderer->windowWidth / renderer->windowHeight, 0.1f, 10.0f)
{
  Tri1 = new Triangle(renderer->windowWidth, renderer->windowHeight);
  Tri2 = new Triangle(renderer->windowWidth * 2, renderer->windowHeight * 2);
}

void Game::run()
{
  while (renderer->isWindowOpen)
  {
    renderer->beginRender();

    Tri1->draw(camera);
    Tri2->draw(camera);

    camera.setPosition(0, sin((float)glfwGetTime()) * 2, 2);

    renderer->endRender();
  }
}

Game::~Game()
{
  delete Tri1;
  delete Tri2;

  exit(EXIT_SUCCESS);
}