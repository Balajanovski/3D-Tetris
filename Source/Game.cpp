// 
// Created by Balajanovski on 21/04/2018.
//

#include "Game.h"

#ifndef NDEBUG
#include <iostream>
#endif

Game::Game() :
        // Initialise current tetromino for it has no default constructor
        view_component("Shaders/vertex.vert", "Shaders/fragment.frag"),
        input_queue(view_component.get_window()),
        current_tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 5)), this)

{
    current_tetromino = Tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 5)), this);
}

void Game::begin() {
    // Game loop
    double previous_time = glfwGetTime();
    double lag           = 0;
    while (!game_over) {
        double current_time = glfwGetTime();
        double elapsed = current_time - previous_time;
        previous_time = current_time;
        lag += elapsed;

        glfwPollEvents();

        while (lag >= MS_PER_UPDATE) {
            if (!game_over) {
                tick();
            }
            lag -= MS_PER_UPDATE;
        }

        // Drawing code
        view_component.clear_screen();
        if (current_tetromino.get_state() != TetrominoUtil::TetrominoState::LANDED) {
            view_component.draw_tetromino(current_tetromino);
        }

        for (auto& t : landed) {
            view_component.draw_tetromino(t);
        }

        view_component.swap_buffers();
    }
}

void Game::tick() {
    if (current_tetromino.get_state() == TetrominoUtil::TetrominoState::LANDED) {
        current_tetromino =
                Tetromino(static_cast<TetrominoUtil::TetrominoType>(rng_component.rng(0, 5)), this);
    }

    // Handle input
    int input_key = input_queue.fetch();
    switch (input_key) {
        case GLFW_KEY_LEFT :
            current_tetromino.translate_left();
#ifndef NDEBUG
            std::cerr << "Input: Left\n";
#endif
            break;
        case GLFW_KEY_RIGHT :
            current_tetromino.translate_right();
#ifndef NDEBUG
            std::cerr << "Input: Right\n";
#endif
            break;
        case GLFW_KEY_UP :
            current_tetromino.rotate_right();
#ifndef NDEBUG
            std::cerr << "Input: Up\n";
#endif
            break;
        case GLFW_KEY_DOWN :
            current_tetromino.rotate_left();
#ifndef NDEBUG
            std::cerr << "Input: Down\n";
#endif
            break;
        case GLFW_KEY_SPACE :
#ifndef NDEBUG
            std::cerr << "Input: Space\n";
#endif
            while (current_tetromino.get_state() != TetrominoUtil::TetrominoState::LANDED) {
                current_tetromino.translate_down();
            }
            return; // Exit function, because last translate down is redundant
            break;  // Break statement is redundant, yet there for stylistic reasons
        case GLFW_KEY_ESCAPE :
#ifndef NDEBUG
            std::cerr << "Input: Escape\n";
#endif
            game_over = true;
            return; // Exit function, because quit command was invoked
            break;
        default:
            break;
    }

    // Handle game over
    for (auto& t : landed) {
        if (t.get_top_left_point().y <= 1) {
            game_over = true;
        }
    }

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