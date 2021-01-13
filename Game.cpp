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

void mutateLayers(PyreNet::NeuralNet& nn, double factor) {
    nn.mutate_gaussian(0, factor * 1.0/sqrt((double)NN_INPUT_SIZE),0);
    nn.mutate_gaussian(0, factor * 1.0/sqrt(50.0), 1);
    nn.mutate_gaussian(0, factor * 1.0/sqrt(50.0), 2);
    nn.mutate_gaussian(0, factor * 1.0/sqrt(100.0), 3);
    nn.mutate_gaussian(0, factor * 1.0/sqrt(100.0), 4);
    nn.mutate_gaussian(0, factor * 1.0/sqrt(100.0), 5);
    nn.mutate_gaussian(0, factor * 1.0/sqrt(50.0), 6);
    nn.mutate_gaussian(0, factor * 1.0/sqrt(50.0), 7);
}

Game::Game(int playerCount, const std::string &worldFile, std::istream* loadNN = nullptr) {
    this->playerCount = playerCount;
    world = new World(worldFile, std::pair<int, int>(100,100));

    players.reserve(playerCount);

    layerDefs.emplace_back(50, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(50, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(100, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(100, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(100, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(50, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(50, PyreNet::LayerDefinition::activationType::relu);
    layerDefs.emplace_back(7, PyreNet::LayerDefinition::activationType::relu);

    PyreNet::NeuralNet nnLoaded(NN_INPUT_SIZE, layerDefs);
    if (loadNN)
        nnLoaded = PyreNet::NeuralNet(*loadNN);
    for (int i = 0; i < playerCount; ++i) {
        PyreNet::NeuralNet* nn;
        if (loadNN) {
            nn = new PyreNet::NeuralNet(nnLoaded);
        } else {
            nn = new PyreNet::NeuralNet(NN_INPUT_SIZE, layerDefs);
            mutateLayers(*nn, 1);
        }
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
    int bestEver = 0;
    PyreNet::NeuralNet bestEverNN = *players.front().getNN();
    std::unique_lock<std::shared_mutex> lock(tickMutex);
    for (int e = 0; e < epochCount; ++e) {
        std::cout << "----- Epoch " << e << " -----" << std::endl;
        int t = tickCount + (int)log2(bestScore + 1) * 5;
        for (int& tick : tickCounts)
            tick = t;
        epochCv.notify_all();
        tickDoneCv.wait(tickMutex, [this]() { return zeros(tickCounts); });

        // Post game analysis & record keeping
        std::ofstream ofsp("epoch/p_e" + std::to_string(e) + ".txt", std::ofstream::out);
        Player* bestPlayer = &players[0];
        int bestI = 0;
        for (int i = 0; i < players.size(); ++i) {
            Player& player = players[i];
            if (player.getScore() > bestPlayer->getScore()) {
                bestI = i;
                bestPlayer = &player;
            }
            ofsp << "p" << i << std::endl;
            player.writeHistory(ofsp);
        }
        ofsp.close();

        bestScore = bestPlayer->getScore();
        std::cout << "Best Score: " << bestScore << " (Player " << bestI << ")" << std::endl;
        PyreNet::NeuralNet bestNN = *bestPlayer->getNN();
        std::ofstream ofsnn("epoch/nn_e" + std::to_string(e) + ".txt", std::ofstream::out);
        ofsnn << bestNN;
        ofsnn.close();

        if (bestScore >= bestEver) {
            bestEver = bestScore;
            bestEverNN = bestNN;
        } else {
            bestNN = bestEverNN;
        }

        // Setup for next epoch
        players.clear();
        for (int i = 0; i < playerCount; ++i) {
            auto* nn = new PyreNet::NeuralNet(bestNN);
            if (bestScore == 0) {
                delete nn;
                nn = new PyreNet::NeuralNet(NN_INPUT_SIZE, layerDefs);
                mutateLayers(*nn, 1);
            } else {
                mutateLayers(*nn, 0.2);
                if (i > (playerCount)) { // Introduce some fresh ideas
                    delete nn;
                    nn = new PyreNet::NeuralNet(NN_INPUT_SIZE, layerDefs);
                    mutateLayers(*nn, 1);
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
