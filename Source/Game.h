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
    void reset();

    void add_landed(const Tetromino& t);
    bool check_collision(const Tetromino& new_pos) const;

    RandomNumberComponent rng_component;
private:
    void tick();
    int window_control(); // Returns fetched key
    void handle_row_clearing();

    ViewComponent view_component;
    InputQueue    input_queue;

    Tetromino current_tetromino;
    Tetromino ghost_tetromino; // Used to indicate where the tetromino will land
    std::vector<Tetromino> landed;

    bool game_over = false;
    bool close_game = false;
    bool paused = false;
    unsigned int score = 0;

    // Last time the current tetromino moved down
    double previous_tetromino_move_time;
};


#endif //INC_3D_TETRIS_GAME_H

