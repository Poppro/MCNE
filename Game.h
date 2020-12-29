//
// Created by Poppro on 12/23/2020.
//

#ifndef MCEVOLUTION_GAME_H
#define MCEVOLUTION_GAME_H

#include <condition_variable>
#include <shared_mutex>

#include "World.h"
#include "Player.h"

#include "constants.h"

class Game {
public:
    Game(int playerCount, const std::string &worldFile, std::istream* loadNN);

    void runGame(int tickCount, int epochCount);
private:
    [[noreturn]] void runTicks(int tickCount, int start, int end);
private:
    World* world;
    std::vector<Player> players;
    int playerCount;
    std::vector<std::thread> pool;
    int tickCounts[THREAD_COUNT];
    std::shared_mutex tickMutex;
    std::condition_variable_any epochCv;
    std::condition_variable_any tickDoneCv;
};


#endif //MCEVOLUTION_GAME_H
