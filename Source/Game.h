// 
// Created by Balajanovski on 21/04/2018.
//

#ifndef INC_3D_TETRIS_GAME_H
#define INC_3D_TETRIS_GAME_H

#include "Tetromino.h"
#include "Constants.h"

#include "InputQueue.h"
#include "ViewComponent.h"
#include "RandomNumberComponent.h"

#include <vector>
#include <glm/vec2.hpp>

class Game {
public:
    Game();

    void tick();
    void add_landed(const Tetromino& t);
    bool check_collision(const Tetromino& new_pos) const;

    RandomNumberComponent rng_component;
private:
    ViewComponent view_component;
    InputQueue    input_queue;

    Tetromino current_tetromino;
    std::vector<Tetromino> landed;

    bool game_over = false;
};


#endif //INC_3D_TETRIS_GAME_H

