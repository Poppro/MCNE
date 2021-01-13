//
// Created by Poppro on 12/23/2020.
//

#include "Player.h"

#include <iostream>
#include <fstream>

#include "RayCaster.h"
#include "constants.h"

void Player::addScore(int val) {
    score += val;
}

void Player::moveForward() {
    Position tmp = pos;
    if (abs(direction.x()) > abs(direction.y())) {
        if (direction.x() > 0) {
            tmp.x += 1;
        } else {
            tmp.x -= 1;
        }
    } else {
        if (direction.y() > 0) {
            tmp.y += 1;
        } else {
            tmp.y -= 1;
        }
    }
    if (!checkCollision(tmp))
        pos = tmp;
    makeFall();
}

void Player::jump() {
    makeFall();
    pos.z += 1;
}

void Player::rotateUp(double deg) {
    Eigen::Vector3d zAxis = Eigen::Vector3d(0, 0, 1);
    Eigen::Vector3d tilt = direction.cross(zAxis);
    tilt.normalize();
    direction = Eigen::AngleAxisd(deg * M_PI / 180.0, tilt) * direction;
}

void Player::rotateDown(double deg) {
    rotateUp(-deg);
}

void Player::rotateRight(double deg) {
    Eigen::Vector3d zAxis = Eigen::Vector3d(0, 0, 1);
    direction = Eigen::AngleAxisd(deg * M_PI / 180.0, zAxis) * direction;
}

void Player::rotateLeft(double deg) {
    rotateRight(-deg);
}

void Player::dig() {
    RayCaster rayCaster(1, 0, 2);
    std::vector<BlockInfo> digBlock = rayCaster.castRays(*this, *world);
    if (digBlock[0].distance > 0 && digBlock[0].block.getId() != 7) {
        addScore(digBlock[0].block.getValue());
        world->clearBlock(digBlock[0].pos.x, digBlock[0].pos.y, digBlock[0].pos.z);
        breakHistory.pop_back();
        breakHistory.push_back(digBlock[0].pos);
    } else { // log attempted break
        breakHistory.pop_back();
        breakHistory.emplace_back(1, 1, 1);
    }
    makeFall();
}

void Player::makeFall() {
    while (!world->getBlock(pos.x, pos.y, pos.z - 1).isCollider())
        pos.z -= 1;
}

void Player::makeMove() {
    RayCaster rayCaster(N_RAYS, FOV, DEPTH);
    std::vector<BlockInfo> castingView = rayCaster.castRays(*this, *world);
    std::vector<double> netInput;
    netInput.reserve(NN_INPUT_SIZE);
    for (BlockInfo& bi : castingView) {
        netInput.push_back(bi.block.getId());
        netInput.push_back(bi.distance);
    }
    netInput.push_back(pos.z); // add height coord
    netInput.push_back(direction.x());
    netInput.push_back(direction.y());
    netInput.push_back(direction.z());

    std::vector<double> result = nn->predict(netInput);
    double max = 0;
    int maxIndex = 0;
    for (int i = 0 ; i < result.size(); ++i) {
        if (result[i] > max) {
            max = result[i];
            maxIndex = i;
        }
    }


    breakHistory.emplace_back(0, 0, 0);
    switch (maxIndex) {
        case 0:
            moveForward();
            break;
        case 1:
            dig();
            break;
        case 2:
            rotateRight(90);
            break;
        case 3:
            rotateLeft(90);
            break;
        case 4:
            rotateUp(30);
            break;
        case 5:
            rotateDown(30);
            break;
        case 6:
            jump();
            break;
        default:
            std::cout << "Unexpected network output size " << std::endl;
            exit(0);
    }
    posHistory.push_back(pos);
    directionHistory.push_back(direction);
}

// Return true if there is a collision at the specified block
bool Player::checkCollision(Position p) {
    return world->getBlock(p.x, p.y, p.z).isCollider() || world->getBlock(p.x, p.y, p.z + 1).isCollider();
}

PyreNet::NeuralNet* Player::getNN() const {
    return nn;
}

int Player::getScore() const {
    return score;
}

Position Player::getPosition() const {
    return pos;
}

Eigen::Vector3d Player::getDirection() const {
    return direction;
}

void Player::writeHistory(std::ofstream& ofs) {
    for (int i = 0; i < posHistory.size(); ++i) {
        Position& cpos = posHistory[i];
        Position& bpos = breakHistory[i];
        Eigen::Vector3d& dir = directionHistory[i];
        ofs << std::to_string(cpos.x) << "." << std::to_string(cpos.y) << "." << std::to_string(cpos.z) << ",";
        ofs << std::to_string(dir.x()) << "." << std::to_string(dir.y()) << "." <<std::to_string(dir.z()) << ",";
        ofs << std::to_string(bpos.x) << "." << std::to_string(bpos.y) << "." << std::to_string(bpos.z);
        ofs << std::endl;
    }
}