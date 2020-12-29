//
// Created by Poppro on 12/23/2020.
//

#ifndef MCEVOLUTION_PLAYER_H
#define MCEVOLUTION_PLAYER_H

#include <PyreNet.h>

#include <utility>

#include "Eigen/Dense"
#include "Block.h"
#include "World.h"

struct Position {
    int x;
    int y;
    int z;
    Position(int x, int y, int z) : x(x), y(y), z(z) {}
};

class Player {
public:
    Player(PyreNet::NeuralNet* nn, Position pos, Eigen::Vector3d direction, World* world) :
        nn(nn), pos(pos), direction(std::move(direction)), world(world) {}
    void makeMove();
    PyreNet::NeuralNet* getNN() const;
    int getScore() const;
    Position getPosition() const;
    Eigen::Vector3d getDirection() const;
    void writeHistory(std::ofstream& ofs);
    ~Player() {
        delete nn;
        delete world;
    }
private:
    void addScore(int val);
    void moveForward();
    void jump();
    void rotateRight(double deg);
    void rotateLeft(double deg);
    void rotateDown(double deg);
    void rotateUp(double deg);
    void dig();
    void makeFall();
    bool checkCollision(Position p);
private:
    PyreNet::NeuralNet* nn;
    World* world;
    Position pos;
    Eigen::Vector3d direction;
    std::vector<Position> posHistory;
    std::vector<Position> breakHistory;
    std::vector<Eigen::Vector3d> directionHistory;
    int score = 0;
};


#endif //MCEVOLUTION_PLAYER_H
