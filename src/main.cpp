#include <iostream>
#include "game.h"

int main(int, char**){
    Game& game = Game::getInstance();
    game.init();
    game.run();


    return EXIT_SUCCESS;
}
