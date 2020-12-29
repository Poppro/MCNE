//
// Created by Poppro on 12/27/2020.
//

#ifndef MCEVOLUTION_CONSTANTS_H
#define MCEVOLUTION_CONSTANTS_H

const int N_RAYS = 3;
const int FOV = 100;
const int DEPTH = 7;

const int NN_INPUT_SIZE = 2*N_RAYS*N_RAYS + 4;

constexpr int THREAD_COUNT = 10; // Right now this needs to divide player count

#endif //MCEVOLUTION_CONSTANTS_H
