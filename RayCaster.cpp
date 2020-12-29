//
// Created by Poppro on 12/24/2020.
//

#include "RayCaster.h"

#include <cmath>

int getIntBlockPos(double pos) {
    return ceil(pos - 0.5);
}

std::vector<BlockInfo> RayCaster::castRays(const Player& player, const World& world) {
    std::vector<BlockInfo> resp(n*n, BlockInfo(Block(0), 0, Position(0, 0, 0)));
    Eigen::Vector3d centerRay = player.getDirection();
    Position playerPosition = player.getPosition();
    Eigen::Vector3d zAxis = Eigen::Vector3d(0, 0, 1);
    Eigen::Vector3d tilt = centerRay.cross(zAxis).normalized();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int row = i - n/2;
            int col = j - n/2;
            Eigen::Vector3d ray;
            ray = Eigen::AngleAxisd(col * (2.0*fov/(double)n * M_PI / 180.0), zAxis) * centerRay;
            ray = Eigen::AngleAxisd(row * (2.0*fov/(double)n * M_PI / 180.0), tilt) * ray;
            ray = (sqrt(2)/2.0 + 0.01) * ray;
            for (int d = 0; d < depth; ++d) {
                Position worldPosition = Position(playerPosition.x + getIntBlockPos(ray.x()),
                        playerPosition.y + getIntBlockPos(ray.y()),
                        playerPosition.z + getIntBlockPos(ray.z()));
                Block block = world.getBlock(worldPosition.x, worldPosition.y, worldPosition.z);
                if (block.getId() != 0) {
                    resp[i*n + j] = BlockInfo(block, d + 1, worldPosition);
                    break;
                }
                ray = ray + ray.normalized();
            }
        }
    }
    return resp;
}
