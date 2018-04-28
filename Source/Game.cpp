// 
// Created by Balajanovski on 21/04/2018.
//

#include "Game.h"

Game::Game() :
        // Initialise current tetromino for it has no default constructor
        current_tetromino(static_cast<TetrominoUtil::TetrominoType>(0), *this)
{
    current_tetromino = Tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 5)), *this);
}

void Game::tick() {

    current_tetromino.translate_down();
}

void Game::add_landed(const Tetromino& t) {
    landed.push_back(t);

    if (t.highest_block() <= 1) {
        game_over = true;
    }
}

bool Game::check_collision(const Tetromino& new_pos) const {
    for (auto iter = landed.cbegin(); iter != landed.cend(); ++iter) {
        if (iter->tetromino_overlaps(new_pos)) {
            return true;
        }
    }

    return false;
}