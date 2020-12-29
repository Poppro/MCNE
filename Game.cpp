//
// Created by Poppro on 12/23/2020.
//

#include "Game.h"

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

std::vector<PyreNet::LayerDefinition> layerDefs;

bool zeros(int* i){
    for (int j = 0; j < THREAD_COUNT; ++j) {
        if (*(i + j) != 0)
            return false;
    }
    return true;
}

Game::Game(int playerCount, const std::string &worldFile, std::istream* loadNN = nullptr) {
    this->playerCount = playerCount;
    world = new World(worldFile, std::pair<int, int>(100,100));

    players.reserve(playerCount);

    layerDefs.emplace_back(50, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(50, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(25, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(50, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(50, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(7, PyreNet::LayerDefinition::activationType::relu);

    for (int i = 0; i < playerCount; ++i) {
        auto* nn = new PyreNet::NeuralNet(NN_INPUT_SIZE, layerDefs);
        nn->mutate(-1.0/16.0, 1.0/16.0);
        if (loadNN)
            *loadNN >> *nn;
        players.emplace_back(nn, Position(49, 47, 69), Eigen::Vector3d(1,0,0), new World(*world));
    }

    // Thread pool to run game ticks on players
    for (int i = 0; i < THREAD_COUNT; i++) {
        tickCounts[i] = 0;
        int start = i * playerCount/THREAD_COUNT;
        int end = start + playerCount/THREAD_COUNT;
        pool.emplace_back(&Game::runTicks, this, i, start, end);
    }
}

void Game::runGame(int tickCount, int epochCount) {
    int bestScore = 0;
    std::unique_lock<std::shared_mutex> lock(tickMutex);
    for (int e = 0; e < epochCount; ++e) {
        std::cout << "----- Epoch " << e << " -----" << std::endl;
        int t = tickCount + (int)log2(bestScore + 1) * 5;
        for (int& tick : tickCounts)
            tick = t;
        epochCv.notify_all();
        tickDoneCv.wait(tickMutex, [this]() { return zeros(tickCounts); });

        // Post game analysis & record keeping
        Player* bestPlayer = &players[0];
        int bestI = 0;
        for (int i = 0; i < players.size(); ++i) {
            Player& player = players[i];
            if (player.getScore() > bestPlayer->getScore()) {
                bestI = i;
                bestPlayer = &player;
            }
            std::ofstream ofs("epoch/p" + std::to_string(i) + "_e" + std::to_string(e) + ".txt", std::ofstream::out);
            player.writeHistory(ofs);
            ofs.close();
        }
        bestScore = bestPlayer->getScore();
        std::cout << "Best Score: " << bestScore << " (Player " << bestI << ")" << std::endl;
        PyreNet::NeuralNet bestNN = *bestPlayer->getNN();
        std::ofstream ofs("epoch/nn_e" + std::to_string(e) + ".txt", std::ofstream::out);
        ofs << bestNN;
        ofs.close();

        // Setup for next epoch
        players.clear();
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        auto stdMutate = std::normal_distribution<double>(0, 1.0/16.0);
        for (int i = 0; i < playerCount; ++i) {
            auto* nn = new PyreNet::NeuralNet(bestNN);
            double mutateBound = abs(stdMutate(generator));
            if (bestScore == 0) {
                delete nn;
                nn = new PyreNet::NeuralNet(NN_INPUT_SIZE, layerDefs);
                nn->mutate(-1.0/16.0, 1.0/16.0);
            } else {
                nn->mutate(-mutateBound, mutateBound);
                if (i > (playerCount)) { // Introduce some fresh ideas
                    delete nn;
                    nn = new PyreNet::NeuralNet(NN_INPUT_SIZE, layerDefs);
                    nn->mutate(-1.0/16.0, 1.0/16.0);
                }
            }
            players.emplace_back(nn, Position(49, 47, 69), Eigen::Vector3d(1,0,0), new World(*world));
        }
    }
}

[[noreturn]] void Game::runTicks(int id, int start, int end) {
    while (true) {
        std::shared_lock<std::shared_mutex> lock(tickMutex);
        while (tickCounts[id] > 0) {
            for (int i = start; i < end; ++i)
                players[i].makeMove();
            tickCounts[id]--;
        }
        tickDoneCv.notify_one();
        epochCv.wait(lock);
    }
}
