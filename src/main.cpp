#include "Game.h"

int main(void)
{
    Game* game = new Game();

    game->run();

    delete game;
}
