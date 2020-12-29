//
// Created by Poppro on 12/24/2020.
//

#ifndef MCEVOLUTION_RAYCASTER_H
#define MCEVOLUTION_RAYCASTER_H

#include <vector>

#include "Player.h"
#include "World.h"

struct BlockInfo {
    Block block;
    int distance;
    Position pos;
    BlockInfo(Block block, int distance, Position pos) : block(block), distance(distance), pos(pos) {}
};

class RayCaster {
public:
    // nxn grid. fov in degrees.
    RayCaster(int n, int fov, int depth) : n(n), fov(fov), depth(depth) {}
    // Returns a 1D array of block ids hit by the raycast
    std::vector<BlockInfo> castRays(const Player& player, const World& world);
private:
    double fov;
    int n;
    int depth;
};


#endif //MCEVOLUTION_RAYCASTER_H
