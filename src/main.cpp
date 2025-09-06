#include <iostream>
#include "game.h"

void test()
{
    std::cout<<"test"<<std::endl;
}

int main(int, char**){
    Game& game = Game::getInstance();
    game.init();
    game.run();


    return EXIT_SUCCESS;
}
