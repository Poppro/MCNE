//
// Created by Poppro on 12/23/2020.
//

#ifndef MCEVOLUTION_WORLD_H
#define MCEVOLUTION_WORLD_H

#include <string>
#include <vector>

#include "Block.h"

// Worlds are defaulted to 256 height
class World {
public:
    World(const std::string& fileName, std::pair<int, int> worldSize);
    void clearBlock(int x, int y, int z);
    Block getBlock(int x, int y, int z) const;
    std::pair<int, int> getDimensions() const;
private:
    std::vector<std::vector<std::vector<Block>>> blocks;
    std::pair<int, int> worldSize;
};


#endif //MCEVOLUTION_WORLD_H
