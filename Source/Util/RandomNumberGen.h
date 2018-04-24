// 
// Created by Balajanovski on 24/04/2018.
//

#ifndef INC_3D_TETRIS_RANDOMNUMBERGEN_H
#define INC_3D_TETRIS_RANDOMNUMBERGEN_H

#include <random>

namespace {
    std::mt19937 gen;
    bool gen_initalised = false;
}

namespace RandomNumberGen {
    int rng(int lower_bound, int upper_bound);
};


#endif //INC_3D_TETRIS_RANDOMNUMBERGEN_H
