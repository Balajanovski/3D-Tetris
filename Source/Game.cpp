// 
// Created by Balajanovski on 21/04/2018.
//

#include "Game.h"

#include "Util/RandomNumberGen.cpp"

Game::Game() :
        current_tetromino(static_cast<TetrominoUtil::TetrominoType>(RandomNumberGen::rng(0, 5)), *this)
{

}

void Game::tick() {

    current_tetromino.translate_down();
}

void Game::add_landed(const Tetromino& t) {
    landed.insert(t);

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