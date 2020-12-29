//
// Created by Poppro on 12/24/2020.
//

#ifndef MCEVOLUTION_BLOCK_H
#define MCEVOLUTION_BLOCK_H

#include <unordered_map>

class Block {
public:
    explicit Block(int id);
    int getValue();
    int getId();
    bool isCollider();
private:
    int id;
};


#endif //MCEVOLUTION_BLOCK_H
