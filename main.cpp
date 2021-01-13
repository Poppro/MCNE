#include <iostream>
#include <fstream>

#include "Game.h"

int main() {
    std::ifstream ifs("nn_e51.txt");

    Game game(100, "schem.json", &ifs);
    game.runGame(50, 10000);
    return 0;
}
