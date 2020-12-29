//
// Created by Poppro on 12/23/2020.
//

#include "World.h"

#include <fstream>
#include <iostream>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

World::World(const std::string& fileName, std::pair<int, int> worldSize) {
    std::cout << "Building World" << std::endl;
    this->worldSize = worldSize;
    blocks.resize(worldSize.first, std::vector<std::vector<Block>>(worldSize.second, std::vector<Block>(256, Block(0))));
    std::ifstream ifs(fileName);
    json worldMap;
    ifs >> worldMap;
    for (const auto& block : worldMap["elements"]) {
        try {
            blocks[block["pos"][0]][block["pos"][2]][block["pos"][1]] = Block(block["id"]);
        } catch (std::out_of_range &ex) {
            std::cout << "Out of range block index on world initialization for given world size" << std::endl;
            exit(0);
        }
    }
    std::cout << "World Built" << std::endl;
}

void World::clearBlock(int x, int y, int z) {
    if (x >= worldSize.first || y >= worldSize.second || z >= 256 ||
        x < 0 || y < 0 || z < 0) {
        //std::cout << "Invalid block access" << std::endl;
        return;
    }
    blocks[x][y][z] = Block(0);
}


Block World::getBlock(int x, int y, int z) const {
    if (x >= worldSize.first || y >= worldSize.second || z >= 256 ||
        x < 0 || y < 0 || z < 0) {
        //std::cout << "Invalid block access" << std::endl;
        return Block(0);
    }
    return blocks[x][y][z];
}

std::pair<int, int> World::getDimensions() const {
    return worldSize;
}
