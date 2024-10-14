#include <iostream>
#include "Game.h"

// using namespace sf;

int main()
{
    // Init Game Engine
    Game game;

    // Game loop
    while (game.running())
    {
        std::cout << "Game loop is running..." << std::endl;  // Debug print

        // Update
        game.update();

        // Render
        game.render();
    }

    std::cout << "Game loop ended" << std::endl;

    return 0;
}