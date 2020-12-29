#include <iostream>
#include <fstream>

#include "Game.h"

int main() {
    //std::ifstream ifs("nn_load.txt");

    Game game(100, "schem.json", nullptr);
    game.runGame(50, 10000);
    return 0;
}
