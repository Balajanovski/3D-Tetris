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

    void begin();
    void add_landed(Tetromino& t);
    bool check_collision(const Tetromino& new_pos) const;

    RandomNumberComponent rng_component;
private:
    static constexpr double MS_PER_UPDATE = 3;

    void tick();
    void handle_row_clearing();

    ViewComponent view_component;
    InputQueue    input_queue;

    Tetromino current_tetromino;
    std::vector<Tetromino> landed;

    bool game_over = false;

    // Last time the current tetromino moved down
    double previous_tetromino_move_time;

    // Represents whether a block is taking up a spot
    // Used for row clearing
    struct block_tracker {
        bool block_filled;
        Tetromino* block_parent;
    };
    block_tracker block_field[GAME_HEIGHT][GAME_WIDTH];
    static const block_tracker EMPTY_BLOCK_TRACKER;
};


#endif //INC_3D_TETRIS_GAME_H

