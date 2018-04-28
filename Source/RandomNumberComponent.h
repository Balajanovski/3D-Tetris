// 
// Created by Balajanovski on 28/04/2018.
//

#ifndef INC_3D_TETRIS_RANDOMNUMBERCOMPONENT_H
#define INC_3D_TETRIS_RANDOMNUMBERCOMPONENT_H

#include <random>

class RandomNumberComponent {
public:
    RandomNumberComponent();

    void seed();
    void seed(int s);

    int rng (int upper_bound, int lower_bound);
private:
    std::mt19937 gen;
};


#endif //INC_3D_TETRIS_RANDOMNUMBERCOMPONENT_H
