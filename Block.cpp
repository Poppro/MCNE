//
// Created by Poppro on 12/24/2020.
//

#include "Block.h"

std::unordered_map<int, int> blockValues;

Block::Block(int id) {
    this->id = id;
    if (blockValues.empty()) {
        blockValues[2] = 1; // grass
        blockValues[3] = 1; // dirt
        blockValues[1] = 2; // stone
        blockValues[17] = 5; // wood
        blockValues[16] = 100; //coal
        blockValues[15] = 500; // iron
        blockValues[14] = 1000; // gold
        blockValues[56] = 10000; // diamond
    }
}

int Block::getValue() {
    if (blockValues.find(id) != blockValues.end()) {
        return blockValues[id];
    }
    return 0;
}

int Block::getId() {
    return id;
}

bool Block::isCollider() {
    return id != 0 && id != 31;
}